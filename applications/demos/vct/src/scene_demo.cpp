// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "scene_demo.h"

using namespace wn;
using namespace wn::runtime;

bool scene_demo::render_scene(bool _exit, float _time_since_last_update) {
  m_frame_accumulator += _time_since_last_update;
  m_time += _time_since_last_update;
  m_frames_time_counter += 1;

  // Keep the roundey-bits from our accumulator.
  if (m_frame_accumulator > 5.0) {
    m_log->log_info("Framerate: ", (float)m_frames_time_counter / 5.0f, " fps");
    m_log->log_info(
        "Frame Time: ", 5000.0f / (float)m_frames_time_counter, "ms");
    m_log->flush();
    m_frame_accumulator -= 5.0f;
    m_frames_time_counter = 0;
  }

  // Which of our double-buffers are we.
  uint32_t frame_index = m_frame_number & 0x1;

  per_frame_data* frame_data = &m_frame_data[frame_index];

  // If m_frame_number == frame_index, then we are the first
  // use of this buffer. We don't have to wait on previous uses.
  if (m_frame_number != frame_index) {
    frame_data->m_frame_start_fence->wait();
    frame_data->m_frame_start_fence->reset();
  }
  if (_exit) {
    return true;
  }
  ++m_frame_number;

  float x_t = 0;
  float z_t = 0;
  if (m_window->get_key_state(window::key_code::key_w)) {
    z_t += k_scale * _time_since_last_update;
  }
  if (m_window->get_key_state(window::key_code::key_s)) {
    z_t -= k_scale * _time_since_last_update;
  }
  if (m_window->get_key_state(window::key_code::key_a)) {
    x_t += k_scale * _time_since_last_update;
  }
  if (m_window->get_key_state(window::key_code::key_d)) {
    x_t -= k_scale * _time_since_last_update;
  }

  float mouse_move_x = 0;
  float mouse_move_y = 0;

  if (m_window->get_mouse_state(window::mouse_button::mouse_l)) {
    uint32_t mouse_x = m_window->get_cursor_x();
    uint32_t mouse_y = m_window->get_cursor_y();
    if (m_last_mouse_pressed) {
      mouse_move_x = static_cast<float>(
          static_cast<int32_t>(mouse_x) - static_cast<int32_t>(m_last_mouse_x));
      mouse_move_y = static_cast<float>(
          static_cast<int32_t>(mouse_y) - static_cast<int32_t>(m_last_mouse_y));
      // WHY are we getting these jumps?
      if (mouse_move_x > 100) {
        mouse_move_x = 0;
      }
      if (mouse_move_x < -100) {
        mouse_move_x = 0;
      }
      if (mouse_move_y > 100) {
        mouse_move_y = 0;
      }
      if (mouse_move_y < -100) {
        mouse_move_y = 0;
      }
    }
    m_last_mouse_x = mouse_x;
    m_last_mouse_y = mouse_y;
    m_last_mouse_pressed = true;
  } else {
    m_last_mouse_pressed = false;
  }

  m_yaw = (m_yaw - mouse_move_x * 0.05f);
  m_pitch = (m_pitch + mouse_move_y * 0.05f);

  math::mat44f forward = math::get_rotation_x(m_pitch * 0.05f) *
                         math::get_rotation_y(m_yaw * 0.05f);

  math::mat44f face_forward = math::get_rotation_y(-m_yaw * 0.05f) *
                              math::get_rotation_x(-m_pitch * 0.05f);

  math::mat44f move_forward =
      face_forward * math::get_translation(x_t, 0.0f, z_t);

  m_camera_matrix = math::get_translation(
                        move_forward[12], move_forward[13], move_forward[14]) *
                    m_camera_matrix;

  math::mat44f camera_matrix = forward * m_camera_matrix;

  frame_data->m_global_constants_ptr = reinterpret_cast<global_constants*>(
      frame_data->m_global_constants->map());

  frame_data->m_global_constants_ptr->m_camera_projection =
      math::get_perspective_matrix(1.5708f, 1.0f, 0.1f, 3000.0f) *
      m_device->get_perspective_fixup_matrix();

  frame_data->m_global_constants_ptr->m_camera_transform = camera_matrix;
  // Objects don't actually have a transform yet.
  // They do have a position, but that wont actually get used
  //  by the normal matrix, since it is translation invariant.
  frame_data->m_global_constants_ptr->m_normal_object =
      math::mat44f({1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                       0.0, 0.0, 0.0, 1.0})
          .transposed();

  math::vec4f light_pos = math::vec4f({(float)sin(m_time * 0.5f) * 500.0f,
      1000.0f, (float)cos(m_time * 0.5f) * 500.0f, 1.0f});

  frame_data->m_global_constants_ptr->m_light_position = -light_pos;

  frame_data->m_global_constants->unmap();

  uint32_t idx = m_swapchain->get_next_backbuffer_index(
      nullptr, frame_data->m_present_image_ready_signal.get());

  // Got an invalid swapchain index.. PROBABLY an invalid swapchain.
  if (idx > m_swapchain->info().num_buffers) {
    return false;
  }

  graphics::image* swapchain_image = m_swapchain->get_image_for_index(idx);
  // Poor man's reset. Fix this!
  frame_data->m_command_allocator->reset();
  frame_data->m_command_list =
      frame_data->m_command_allocator->create_command_list();

  graphics::command_list_ptr& list = frame_data->m_command_list;
  per_swapchain_image_data* swapchain_data = &m_swapchain_data[idx];

  // If we have never seen this swapchain, then it is unknown layout.
  // Otherwise it is in present, since that is what we left it as.
  if (!swapchain_data->m_seen) {
    list->transition_resource(*swapchain_image,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, 1, graphics::resource_state::initial,
        graphics::resource_state::render_target);
    list->transition_resource(*swapchain_data->m_depth_buffer,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::depth),
        0, 1, graphics::resource_state::initial,
        graphics::resource_state::depth_target);
    swapchain_data->m_seen = true;
  } else {
    list->transition_resource(*swapchain_image,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, 1, graphics::resource_state::present,
        graphics::resource_state::render_target);
  }

  list->transition_resource(*frame_data->m_global_constants,
      graphics::resource_state::host_write,
      graphics::resource_state::read_only_buffer);

  graphics::clear_value clear[2]{};
  clear[0].color.float_vals[0] = 0.1f;
  clear[0].color.float_vals[1] = 0.1f;
  clear[0].color.float_vals[2] = 0.1f;
  clear[0].color.float_vals[3] = 1.0f;

  clear[1].depth.depth = 1.0f;

  list->begin_render_pass(m_render_pass.get(),
      swapchain_data->m_swapchain_framebuffer.get(), {0, 0, k_width, k_height},
      containers::contiguous_range<graphics::clear_value>(clear, 2));
  for (auto& m : m_entities) {
    // Don't render anything that does not have a diffuse texture.
    if (!m.second->has_diffuse()) {
      continue;
    }
    entity* model = m.second.get();
    // THIS IS SUPER DUMB.
    // Very minimum: dont change pipeline/layout if you dont have to
    // Better, batch all draws by pipeline/layout
    // EVEN BETTER! do that AND sort close-to-far.
    //   We currently don't have any data structures for this...
    //   We should implement that.
    // For now.. be dumb.
    uint32_t pipeline_index = model->get_pipeline_index();

    pipeline_data& dat = *m_pipelines[pipeline_index];
    list->bind_graphics_pipeline_layout(&dat.m_pipeline_layout);
    list->bind_graphics_pipeline(&dat.m_pipeline);
    const graphics::descriptor_set* sets[]{
        model->descriptor_set(),
        frame_data->m_global_set.get(),
    };
    list->bind_graphics_descriptor_sets(sets, 0);
    list->bind_vertex_buffer(0, model->vertex_buffer());
    list->bind_index_buffer(graphics::index_type::u32, model->index_buffer());

    list->draw_indexed(model->num_indices(), 1, 0, 0, 0);
  }

  list->end_render_pass();
  list->transition_resource(*swapchain_image,
      static_cast<wn::runtime::graphics::image_components>(
          wn::runtime::graphics::image_component::color),
      0, 1, graphics::resource_state::render_target,
      graphics::resource_state::present);
  list->transition_resource(*frame_data->m_global_constants,
      graphics::resource_state::read_only_buffer,
      graphics::resource_state::host_write);

  list->finalize();

  m_queue->enqueue_command_lists({list.get()},
      {core::make_pair(static_cast<graphics::pipeline_stages>(
                           graphics::pipeline_stage::color_attachment_output),
          frame_data->m_present_image_ready_signal.get())},
      frame_data->m_frame_start_fence.get(),
      {frame_data->m_rendering_done_signal.get()});

  m_swapchain->present(
      m_queue.get(), frame_data->m_rendering_done_signal.get(), idx);
  return true;
}
