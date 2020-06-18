// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationEntry/inc/WNApplicationHelper.h"
#include "WNContainers/inc/WNList.h"
#include "WNContainers/inc/WNString.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNSampler.h"
#include "WNGraphics/inc/WNShaderModule.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "WNMath/inc/WNVector.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNWindow/inc/WNInputContext.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"
#include "rocket_test/inc/rocket_assets.h"

#include "Rocket/Controls/Controls.h"
#include "Rocket/Core.h"
#include "Rocket/Core/RenderInterface.h"
#include "Rocket/Debugger/Debugger.h"

#include <stb_public.h>
#include <time.h>

using wn::containers::contiguous_range;
using wn::runtime::graphics::adapter_ptr;
using device_factory = wn::runtime::graphics::factory;
using wn::memory::make_unique;
using wn::memory::unique_ptr;
using wn::multi_tasking::job_signal;
using wn::runtime::graphics::arena;
using wn::runtime::graphics::buffer;
using wn::runtime::graphics::graphics_error;
using wn::runtime::graphics::image;
using wn::runtime::graphics::image_view;
using wn::runtime::window::event_type;
using wn::runtime::window::input_event;
using wn::runtime::window::key_code;
using wn::runtime::window::mouse_button;
using wn::runtime::window::window_factory;

static const uint32_t k_width = 1024;
static const uint32_t k_height = 768;

int get_key_modifier_state(wn::runtime::window::window* _window) {
  bool ctrl = _window->get_key_state(key_code::key_lctrl) ||
              _window->get_key_state(key_code::key_rctrl) ||
              _window->get_key_state(key_code::key_any_ctrl);

  bool alt = _window->get_key_state(key_code::key_lalt) ||
             _window->get_key_state(key_code::key_ralt) ||
             _window->get_key_state(key_code::key_any_alt);

  bool shift = _window->get_key_state(key_code::key_lshift) ||
               _window->get_key_state(key_code::key_rshift) ||
               _window->get_key_state(key_code::key_any_shift);

  return Rocket::Core::Input::KeyModifier::KM_CTRL * ctrl +
         Rocket::Core::Input::KeyModifier::KM_ALT * alt +
         Rocket::Core::Input::KeyModifier::KM_SHIFT * shift;
}

int mouse_button_to_index(mouse_button _button) {
  switch (_button) {
    case mouse_button::mouse_l:
      return 0;
    case mouse_button::mouse_r:
      return 1;
    case mouse_button::mouse_m:
      return 2;
    default:
      return -1;
  }
}

Rocket::Core::Input::KeyIdentifier key_code_to_key_index(key_code _key_code) {
  if (_key_code >= key_code::key_0 && _key_code <= key_code::key_9) {
    return static_cast<Rocket::Core::Input::KeyIdentifier>(
        Rocket::Core::Input::KeyIdentifier::KI_0 + static_cast<int>(_key_code) -
        static_cast<int>(key_code::key_0));
  }
  if (_key_code >= key_code::key_a && _key_code <= key_code::key_z) {
    return static_cast<Rocket::Core::Input::KeyIdentifier>(
        Rocket::Core::Input::KeyIdentifier::KI_A + static_cast<int>(_key_code) -
        static_cast<int>(key_code::key_a));
  }
  if (_key_code >= key_code::key_num_0 && _key_code <= key_code::key_num_9) {
    return static_cast<Rocket::Core::Input::KeyIdentifier>(
        Rocket::Core::Input::KeyIdentifier::KI_NUMPAD0 +
        static_cast<int>(_key_code) - static_cast<int>(key_code::key_num_0));
  }

  switch (_key_code) {
    case key_code::key_space:
      return Rocket::Core::Input::KeyIdentifier::KI_SPACE;
    case key_code::key_lshift:
      return Rocket::Core::Input::KeyIdentifier::KI_LSHIFT;
    case key_code::key_rshift:
      return Rocket::Core::Input::KeyIdentifier::KI_RSHIFT;
    case key_code::key_lctrl:
      return Rocket::Core::Input::KeyIdentifier::KI_LCONTROL;
    case key_code::key_rctrl:
      return Rocket::Core::Input::KeyIdentifier::KI_RCONTROL;
    case key_code::key_lalt:
      return Rocket::Core::Input::KeyIdentifier::KI_LMENU;
    case key_code::key_ralt:
      return Rocket::Core::Input::KeyIdentifier::KI_RMENU;
    case key_code::key_backspace:
      return Rocket::Core::Input::KeyIdentifier::KI_BACK;
    case key_code::key_del:
      return Rocket::Core::Input::KeyIdentifier::KI_DELETE;
    case key_code::key_left:
      return Rocket::Core::Input::KeyIdentifier::KI_LEFT;
    case key_code::key_right:
      return Rocket::Core::Input::KeyIdentifier::KI_RIGHT;
    case key_code::key_up:
      return Rocket::Core::Input::KeyIdentifier::KI_UP;
    case key_code::key_down:
      return Rocket::Core::Input::KeyIdentifier::KI_DOWN;
    case key_code::key_tab:
      return Rocket::Core::Input::KeyIdentifier::KI_TAB;
    default:
      return Rocket::Core::Input::KeyIdentifier::KI_UNKNOWN;
  }
}

class WNSystemInterface : public Rocket::Core::SystemInterface {
public:
  WNSystemInterface(
      wn::logging::log* _log, wn::runtime::window::window* _window)
    : m_log(_log), m_start(time(0)), m_window(_window) {}

  bool LogMessage(Rocket::Core::Log::Type type,
      const Rocket::Core::String& message) override {
    switch (type) {
      case Rocket::Core::Log::LT_ALWAYS:
        m_log->log_critical(message.CString());
        break;
      case Rocket::Core::Log::LT_ASSERT:
        m_log->log_critical("ROCKET_ASSERT: ", message.CString());
        break;
      case Rocket::Core::Log::LT_DEBUG:
        m_log->log_debug(message.CString());
        break;
      case Rocket::Core::Log::LT_ERROR:
        m_log->log_error(message.CString());
        break;
      case Rocket::Core::Log::LT_INFO:
        m_log->log_info(message.CString());
        break;
      case Rocket::Core::Log::LT_WARNING:
        m_log->log_warning(message.CString());
        break;
      case Rocket::Core::Log::LT_MAX:
        m_log->log_critical(message.CString());
        break;
    }
    return true;
  }

  float GetElapsedTime() override {
    return float(difftime(time(0), m_start));
  }

  // Activate keyboard (for touchscreen devices)
  void ActivateKeyboard() override {
    m_window->show_keyboard();
  }

  // Deactivate keyboard (for touchscreen devices)
  void DeactivateKeyboard() override {
    m_window->hide_keyboard();
  }

private:
  wn::logging::log* m_log;
  time_t m_start;
  wn::runtime::window::window* m_window;
};

wn::containers::string_view get_string_view(
    const Rocket::Core::String& string) {
  return wn::containers::string_view(string.CString());
}

class WNFileInterface : public Rocket::Core::FileInterface {
  struct WNFileInterfaceFile {
    wn::file_system::file_ptr m_file;
    size_t m_offset;
  };

public:
  WNFileInterface(wn::logging::log* _log, wn::file_system::mapping_ptr _mapping)
    : m_log(_log), m_mapping(wn::core::move(_mapping)) {}

  Rocket::Core::FileHandle Open(const Rocket::Core::String& _path) override {
    wn::file_system::result res = wn::file_system::result::ok;
    wn::file_system::file_ptr file =
        m_mapping->open_file(get_string_view(_path), res);
    if (res != wn::file_system::result::ok) {
      return reinterpret_cast<Rocket::Core::FileHandle>(nullptr);
    }

    // This makes me sad, eventually fix this
    WNFileInterfaceFile* f = new WNFileInterfaceFile();
    f->m_offset = 0;
    f->m_file = wn::core::move(file);
    return reinterpret_cast<Rocket::Core::FileHandle>(f);
  }

  void Close(Rocket::Core::FileHandle _file) override {
    WNFileInterfaceFile* f = reinterpret_cast<WNFileInterfaceFile*>(_file);
    delete f;
  }

  size_t Read(
      void* _buffer, size_t _size, Rocket::Core::FileHandle _file) override {
    WNFileInterfaceFile* f = reinterpret_cast<WNFileInterfaceFile*>(_file);

    size_t to_read = wn::math::min(_size, f->m_file->size() - f->m_offset);
    wn::memory::memcpy(_buffer, f->m_file->data() + f->m_offset, to_read);
    f->m_offset += to_read;
    return to_read;
  }

  bool Seek(
      Rocket::Core::FileHandle _file, long _offset, int _origin) override {
    WNFileInterfaceFile* f = reinterpret_cast<WNFileInterfaceFile*>(_file);
    WN_RELEASE_ASSERT(_offset >= 0, "_offset must be positive");
    size_t offs = static_cast<size_t>(_offset);
    switch (_origin) {
      case SEEK_SET:
        if (offs > f->m_file->size()) {
          return false;
        }
        f->m_offset = offs;
        break;
      case SEEK_CUR:
        if (offs + f->m_offset > f->m_file->size()) {
          return false;
        }
        f->m_offset += offs;
        break;
      case SEEK_END:
        if (offs > f->m_file->size()) {
          return false;
        }
        f->m_offset = f->m_file->size() - offs;
        break;
    }
    return true;
  }
  size_t Tell(Rocket::Core::FileHandle _file) override {
    WNFileInterfaceFile* f = reinterpret_cast<WNFileInterfaceFile*>(_file);
    return f->m_offset;
  }

private:
  wn::logging::log* m_log;
  wn::file_system::mapping_ptr m_mapping;
};

struct PerTargetData {
private:
  static wn::runtime::graphics::framebuffer get_framebuffer(
      wn::runtime::graphics::device* _device,
      wn::runtime::graphics::render_pass* _pass,
      wn::runtime::graphics::image_view* _view,
      wn::runtime::graphics::image* _image) {
    const wn::runtime::graphics::image_view* views[] = {_view};
    wn::runtime::graphics::framebuffer_create_info framebuffer_create = {_pass,
        wn::containers::contiguous_range<
            const wn::runtime::graphics::image_view*>(views),
        static_cast<uint32_t>(_image->get_width()),
        static_cast<uint32_t>(_image->get_height()), 1};
    return _device->create_framebuffer(framebuffer_create);
  }

public:
  explicit PerTargetData(wn::memory::allocator* _allocator,
      wn::runtime::graphics::device* _device,
      wn::runtime::graphics::render_pass* _pass,
      wn::runtime::graphics::image* _image, uint32_t _buffer_arena_index,
      uint32_t _texture_arena_index)
    : m_vertex_buffers(_allocator),
      m_index_buffers(_allocator),
      m_upload_buffers(_allocator),
      m_memory_arenas(_allocator),
      m_pools(_allocator),
      m_sets(_allocator),
      m_image(_image),
      m_image_view(_device->create_image_view(_image, 0, 1,
          static_cast<wn::runtime::graphics::image_components>(
              wn::runtime::graphics::image_component::color))),
      m_framebuffer(get_framebuffer(_device, _pass, &m_image_view, _image)),
      m_command_pool(
          wn::memory::make_unique<wn::runtime::graphics::command_allocator>(
              _allocator, _device->create_command_allocator())),
      m_setup_command_pool(
          wn::memory::make_unique<wn::runtime::graphics::command_allocator>(
              _allocator, _device->create_command_allocator())),
      m_buffer_arena_index(_buffer_arena_index),
      m_texture_arena_index(_texture_arena_index) {}

  PerTargetData(PerTargetData&& _other)
    : m_vertex_buffers(wn::core::move(_other.m_vertex_buffers)),
      m_index_buffers(wn::core::move(_other.m_index_buffers)),
      m_upload_buffers(wn::core::move(_other.m_upload_buffers)),
      m_memory_arenas(wn::core::move(_other.m_memory_arenas)),
      m_pools(wn::core::move(_other.m_pools)),
      m_sets(wn::core::move(_other.m_sets)),
      m_image(wn::core::move(_other.m_image)),
      m_image_view(wn::core::move(_other.m_image_view)),
      m_framebuffer(wn::core::move(_other.m_framebuffer)),
      m_command_pool(wn::core::move(_other.m_command_pool)),
      m_setup_command_pool(wn::core::move(_other.m_setup_command_pool)),
      m_command_list(wn::core::move(_other.m_command_list)),
      m_setup_command_list(wn::core::move(_other.m_setup_command_list)),
      m_buffer_arena_index(wn::core::move(_other.m_buffer_arena_index)),
      m_texture_arena_index(wn::core::move(_other.m_texture_arena_index)) {}

  wn::containers::dynamic_array<wn::runtime::graphics::buffer> m_vertex_buffers;
  wn::containers::dynamic_array<wn::runtime::graphics::buffer> m_index_buffers;
  wn::containers::dynamic_array<wn::runtime::graphics::buffer> m_upload_buffers;
  wn::containers::dynamic_array<wn::runtime::graphics::arena> m_memory_arenas;
  wn::containers::dynamic_array<wn::runtime::graphics::descriptor_pool> m_pools;
  wn::containers::dynamic_array<wn::runtime::graphics::descriptor_set> m_sets;
  wn::runtime::graphics::image* m_image;
  wn::runtime::graphics::image_view m_image_view;
  wn::runtime::graphics::framebuffer m_framebuffer;
  wn::memory::unique_ptr<wn::runtime::graphics::command_allocator>
      m_command_pool;
  wn::memory::unique_ptr<wn::runtime::graphics::command_allocator>
      m_setup_command_pool;
  wn::runtime::graphics::command_list_ptr m_command_list;
  wn::runtime::graphics::command_list_ptr m_setup_command_list;
  uint32_t m_buffer_arena_index;
  uint32_t m_texture_arena_index;
};

struct WNTexture {
  WNTexture(wn::runtime::graphics::image image,
      wn::runtime::graphics::arena arena,
      wn::runtime::graphics::image_view view)
    : image(wn::core::move(image)),
      view(wn::core::move(view)),
      arena(wn::core::move(arena)) {}
  wn::runtime::graphics::image image;
  wn::runtime::graphics::image_view view;
  wn::runtime::graphics::arena arena;
  uint32_t used_frames = 0;
};

struct WNGeometry {
  WNGeometry(wn::runtime::graphics::buffer _vertex_buffer,
      wn::runtime::graphics::arena _vertex_arena,
      wn::runtime::graphics::buffer _index_buffer,
      wn::runtime::graphics::arena _index_arena, uint32_t _num_indices)
    : m_vertex_buffer(wn::core::move(_vertex_buffer)),
      m_vertex_arena(wn::core::move(_vertex_arena)),
      m_index_buffer(wn::core::move(_index_buffer)),
      m_index_arena(wn::core::move(_index_arena)),
      m_num_indices(_num_indices) {}

  wn::runtime::graphics::buffer m_vertex_buffer;
  wn::runtime::graphics::arena m_vertex_arena;
  wn::runtime::graphics::buffer m_index_buffer;
  wn::runtime::graphics::arena m_index_arena;
  uint32_t m_num_indices;
  wn::memory::unique_ptr<wn::runtime::graphics::descriptor_pool> m_pool;
  wn::memory::unique_ptr<wn::runtime::graphics::descriptor_set> m_set;
  WNTexture* m_texture = nullptr;
  uint32_t used_frames = 0;
};

wn::runtime::graphics::shader_module get_module(
    wn::file_system::mapping* _mapping, wn::runtime::graphics::device* _device,
    wn::containers::string_view _file_name) {
  wn::file_system::result res;
  auto file = _mapping->open_file(_file_name, res);
  return _device->create_shader_module(file->range());
}

class WNRenderer : public Rocket::Core::RenderInterface {
public:
  WNRenderer(wn::memory::allocator* _allocator,
      wn::runtime::graphics::device* _device,
      wn::containers::contiguous_range<wn::runtime::graphics::image*>
          _target_images,
      wn::file_system::mapping* _mapping, Rocket::Core::Context* _context,
      uint32_t _width, uint32_t _height)
    : Rocket::Core::RenderInterface(_context),
      m_allocator(_allocator),
      m_device(_device),
      m_init_command_allocator(
          wn::memory::make_unique<wn::runtime::graphics::command_allocator>(
              _allocator, _device->create_command_allocator())),
      m_vertex_shader(get_module(_mapping, _device,
          wn::containers::string(_allocator, "assets/built_shaders/ui.vs") +
              _device->get_shader_suffix().data())),
      m_pixel_shader(get_module(_mapping, _device,
          wn::containers::string(_allocator, "assets/built_shaders/ui_tex.ps") +
              _device->get_shader_suffix().data())),
      m_pixel_shader_no_texture(get_module(_mapping, _device,
          wn::containers::string(
              _allocator, "assets/built_shaders/ui_no_tex.ps") +
              _device->get_shader_suffix().data())),
      m_per_target_data(_allocator),
      m_cleanup_textures(_allocator),
      m_cleanup_geometry(_allocator),
      m_sampler(_device->create_sampler({})),
      m_mapping(_mapping),
      m_texture_upload_arenas(_allocator),
      m_texture_upload_buffers(_allocator),
      m_resource_init_command_list(
          m_init_command_allocator->create_command_list()),
      m_screen_multiplier(_device->get_2d_transform_scale()),
      m_width(_width),
      m_height(_height) {
    wn::runtime::graphics::descriptor_binding_info binding_infos[]{
        {
            0,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(
                wn::runtime::graphics::shader_stage::pixel),  // stage
            wn::runtime::graphics::descriptor_type::sampler,  // type
        },
        {
            1,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(wn::runtime::graphics::shader_stage::vertex) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::shader_stage::pixel),   // stage
            wn::runtime::graphics::descriptor_type::sampled_image  // type
        },
    };

    m_descriptor_set_layout =
        make_unique<wn::runtime::graphics::descriptor_set_layout>(
            _allocator, _device->create_descriptor_set_layout(binding_infos));
    m_textureless_descriptor_set_layout =
        make_unique<wn::runtime::graphics::descriptor_set_layout>(
            _allocator, _device->create_descriptor_set_layout({}));

    const wn::runtime::graphics::push_constant_range ranges[] = {{
        static_cast<uint32_t>(
            wn::runtime::graphics::shader_stage::vertex),  // valid_stages
        0,                                                 // register_base
        2,                                                 // num_uint32_values
        0,                                                 // offset_in_uint32s
    }};

    const wn::runtime::graphics::descriptor_set_layout* layouts[] = {
        m_descriptor_set_layout.get()};
    m_pipeline_layout = make_unique<wn::runtime::graphics::pipeline_layout>(
        _allocator, _device->create_pipeline_layout(layouts, ranges));

    layouts[0] = m_textureless_descriptor_set_layout.get();
    m_textureless_pipeline_layout =
        make_unique<wn::runtime::graphics::pipeline_layout>(
            _allocator, _device->create_pipeline_layout(layouts, ranges));

    wn::runtime::graphics::render_pass_attachment attachment[] = {{}};
    attachment[0].attachment_load_op = wn::runtime::graphics::load_op::clear;

    const wn::runtime::graphics::attachment_reference color_attachments[] = {
        {}};
    wn::runtime::graphics::subpass_description subpasses[] = {{}};
    subpasses[0].color_attachments = color_attachments;
    m_render_pass = make_unique<wn::runtime::graphics::render_pass>(_allocator,
        _device->create_render_pass(attachment, subpasses, nullptr));

    m_scissorless_pipeline =
        make_unique<wn::runtime::graphics::graphics_pipeline>(_allocator,
            _device->create_graphics_pipeline(
                wn::runtime::graphics::graphics_pipeline_description(_allocator)
                    .add_vertex_stream(0, sizeof(Rocket::Core::Vertex),
                        wn::runtime::graphics::stream_frequency::per_vertex)
                    .add_vertex_attribute(0, 0, "POSITION", 0,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .add_vertex_attribute(0, 1, "COLOR", 8,
                        wn::runtime::graphics::data_format::r8g8b8a8_unorm)
                    .add_vertex_attribute(0, 2, "TEXCOORD", 12,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .set_index_type(wn::runtime::graphics::index_type::u32)
                    .set_static_viewport(
                        0, {0, 0, static_cast<float>(m_width),
                               static_cast<float>(m_height), 0, 1})
                    .set_cull_mode(wn::runtime::graphics::cull_mode::none)
                    .set_static_scissor(0, {0, 0, m_width, m_height})
                    .add_color_output(0)
                    .set_shader(wn::runtime::graphics::shader_stage::vertex,
                        &m_vertex_shader, "main")
                    .set_shader(wn::runtime::graphics::shader_stage::pixel,
                        &m_pixel_shader, "main")
                    .enable_blending(0,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add),
                m_pipeline_layout.get(), m_render_pass.get(), 0));

    m_scissored_pipeline =
        make_unique<wn::runtime::graphics::graphics_pipeline>(_allocator,
            _device->create_graphics_pipeline(
                wn::runtime::graphics::graphics_pipeline_description(_allocator)
                    .add_vertex_stream(0, sizeof(Rocket::Core::Vertex),
                        wn::runtime::graphics::stream_frequency::per_vertex)
                    .add_vertex_attribute(0, 0, "POSITION", 0,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .add_vertex_attribute(0, 1, "COLOR", 8,
                        wn::runtime::graphics::data_format::r8g8b8a8_unorm)
                    .add_vertex_attribute(0, 2, "TEXCOORD", 12,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .set_num_scissors(1)
                    .set_static_viewport(
                        0, {0, 0, static_cast<float>(m_width),
                               static_cast<float>(m_height), 0, 1})
                    .set_cull_mode(wn::runtime::graphics::cull_mode::none)
                    .set_index_type(wn::runtime::graphics::index_type::u32)
                    .add_color_output(0)
                    .set_shader(wn::runtime::graphics::shader_stage::vertex,
                        &m_vertex_shader, "main")
                    .set_shader(wn::runtime::graphics::shader_stage::pixel,
                        &m_pixel_shader, "main")
                    .enable_blending(0,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add),
                m_pipeline_layout.get(), m_render_pass.get(), 0));

    m_scissorless_textureless_pipeline =
        make_unique<wn::runtime::graphics::graphics_pipeline>(_allocator,
            _device->create_graphics_pipeline(
                wn::runtime::graphics::graphics_pipeline_description(_allocator)
                    .add_vertex_stream(0, sizeof(Rocket::Core::Vertex),
                        wn::runtime::graphics::stream_frequency::per_vertex)
                    .add_vertex_attribute(0, 0, "POSITION", 0,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .add_vertex_attribute(0, 1, "COLOR", 8,
                        wn::runtime::graphics::data_format::r8g8b8a8_unorm)
                    .add_vertex_attribute(0, 2, "TEXCOORD", 12,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .set_index_type(wn::runtime::graphics::index_type::u32)
                    .add_color_output(0)
                    .set_static_viewport(
                        0, {0, 0, static_cast<float>(m_width),
                               static_cast<float>(m_height), 0, 1})
                    .set_cull_mode(wn::runtime::graphics::cull_mode::none)
                    .set_static_scissor(0, {0, 0, m_width, m_height})
                    .set_shader(wn::runtime::graphics::shader_stage::vertex,
                        &m_vertex_shader, "main")
                    .set_shader(wn::runtime::graphics::shader_stage::pixel,
                        &m_pixel_shader_no_texture, "main")
                    .enable_blending(0,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add),
                m_textureless_pipeline_layout.get(), m_render_pass.get(), 0));

    m_scissored_textureless_pipeline =
        make_unique<wn::runtime::graphics::graphics_pipeline>(_allocator,
            _device->create_graphics_pipeline(
                wn::runtime::graphics::graphics_pipeline_description(_allocator)
                    .add_vertex_stream(0, sizeof(Rocket::Core::Vertex),
                        wn::runtime::graphics::stream_frequency::per_vertex)
                    .add_vertex_attribute(0, 0, "POSITION", 0,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .add_vertex_attribute(0, 1, "COLOR", 8,
                        wn::runtime::graphics::data_format::r8g8b8a8_unorm)
                    .add_vertex_attribute(0, 2, "TEXCOORD", 12,
                        wn::runtime::graphics::data_format::r32g32_sfloat)
                    .set_num_scissors(1)
                    .set_static_viewport(
                        0, {0, 0, static_cast<float>(m_width),
                               static_cast<float>(m_height), 0, 1})
                    .set_index_type(wn::runtime::graphics::index_type::u32)
                    .add_color_output(0)
                    .set_shader(wn::runtime::graphics::shader_stage::vertex,
                        &m_vertex_shader, "main")
                    .set_shader(wn::runtime::graphics::shader_stage::pixel,
                        &m_pixel_shader_no_texture, "main")
                    .enable_blending(0,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add,
                        wn::runtime::graphics::blend_factor::src_alpha,
                        wn::runtime::graphics::blend_factor::inv_src_alpha,
                        wn::runtime::graphics::blend_op::add),
                m_textureless_pipeline_layout.get(), m_render_pass.get(), 0));

    const wn::containers::contiguous_range<
        const wn::runtime::graphics::arena_properties>
        arena_properties = _device->get_arena_properties();
    m_buffer_arena_index = 0;
    bool found_arena = false;

    for (; m_buffer_arena_index < arena_properties.size();
         ++m_buffer_arena_index) {
      if (arena_properties[m_buffer_arena_index].host_visible &&
          arena_properties[m_buffer_arena_index].allow_buffers) {
        found_arena = true;
        break;
      }
    }
    WN_RELEASE_ASSERT(found_arena, "Cannot find arena");
    found_arena = false;
    for (; m_texture_arena_index < arena_properties.size();
         ++m_texture_arena_index) {
      if (arena_properties[m_texture_arena_index].allow_images) {
        found_arena = true;
        break;
      }
    }
    WN_RELEASE_ASSERT(found_arena, "Cannot find arena");
    m_buffer_arena_is_coherent =
        arena_properties[m_buffer_arena_index].host_coherent;

    for (size_t i = 0; i < _target_images.size(); ++i) {
      m_per_target_data.emplace_back(
          PerTargetData(_allocator, _device, m_render_pass.get(),
              _target_images[i], m_buffer_arena_index, m_texture_arena_index));
    }
  }

  template <typename T>
  wn::core::pair<buffer, arena> create_and_initialize_buffer(const T* _data,
      uint32_t _count, wn::runtime::graphics::resource_state _usage) {
    const size_t size = _count * sizeof(T);
    buffer buffer = m_device->create_buffer(
        size, static_cast<wn::runtime::graphics::resource_states>(
                  static_cast<uint32_t>(_usage) |
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::host_write)));
    arena memory = m_device->create_arena(
        m_buffer_arena_index, buffer.get_memory_requirements().size);
    buffer.bind_memory(&memory);
    void* data = buffer.map();
    wn::memory::memcpy(data, _data, size);
    buffer.unmap();
    return wn::core::make_pair(wn::core::move(buffer), wn::core::move(memory));
  }

  struct image_data {
    image_data(buffer upload_buffer, arena upload_arena, image texture,
        arena texture_arena, image_view texture_view)
      : upload_buffer(wn::core::move(upload_buffer)),
        upload_arena(wn::core::move(upload_arena)),
        texture(wn::core::move(texture)),
        texture_arena(wn::core::move(texture_arena)),
        texture_view(wn::core::move(texture_view)) {}
    buffer upload_buffer;
    arena upload_arena;
    image texture;
    arena texture_arena;
    image_view texture_view;
  };

  image_data create_and_initialize_texture(uint32_t width, uint32_t height,
      const uint32_t* pixels, wn::runtime::graphics::command_list* setup_list) {
    // TODO: figure out how to get the data.
    wn::runtime::graphics::image_create_info create_info = {width, height,
        wn::runtime::graphics::data_format::r8g8b8a8_unorm,
        static_cast<wn::runtime::graphics::resource_states>(
            static_cast<uint32_t>(
                wn::runtime::graphics::resource_state::texture) |
            static_cast<uint32_t>(
                wn::runtime::graphics::resource_state::copy_dest)),
        1};
    wn::runtime::graphics::clear_value cc;
    wn::runtime::graphics::image texture =
        m_device->create_image(create_info, cc);
    auto mem_reqs = texture.get_memory_requirements();
    wn::runtime::graphics::arena texture_arena =
        m_device->create_arena(m_texture_arena_index, mem_reqs.size);
    texture.bind_memory(&texture_arena, 0);

    auto buffer_reqs = texture.get_buffer_requirements(0);
    buffer upload_buffer =
        m_device->create_buffer(buffer_reqs.total_memory_required,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write)));
    arena buffer_arena = m_device->create_arena(
        m_buffer_arena_index, upload_buffer.get_memory_requirements().size);
    upload_buffer.bind_memory(&buffer_arena);

    void* data = upload_buffer.map();
    uint32_t* dst_base = reinterpret_cast<uint32_t*>(data);
    const uint32_t* src_base = reinterpret_cast<const uint32_t*>(pixels);

    for (size_t i = 0; i < height; ++i) {
      memcpy(dst_base, src_base, width * 4);
      src_base += width;
      dst_base += buffer_reqs.row_pitch_in_bytes / 4;
    }
    upload_buffer.unmap();

    setup_list->transition_resource(upload_buffer,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::copy_source);
    setup_list->transition_resource(texture,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, 1, wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);

    setup_list->copy_buffer_to_image(upload_buffer, 0, texture, 0);
    setup_list->transition_resource(texture,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, 1, wn::runtime::graphics::resource_state::copy_dest,
        wn::runtime::graphics::resource_state::texture);

    wn::runtime::graphics::image_view view =
        m_device->create_image_view(&texture, 0, 1,
            static_cast<wn::runtime::graphics::image_components>(
                wn::runtime::graphics::image_component::color));
    return image_data(wn::core::move(upload_buffer),
        wn::core::move(buffer_arena), wn::core::move(texture),
        wn::core::move(texture_arena), wn::core::move(view));
  }

  void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices,
      int* indices, int num_indices, Rocket::Core::TextureHandle texture,
      const Rocket::Core::Vector2f& translation) override {
    PerTargetData& dat = m_per_target_data[m_current_frame];
    // Coordinate space conversions
    for (int i = 0; i < num_vertices; ++i) {
      vertices[i].position.x =
          m_screen_multiplier[0] *
              (2.0f * vertices[i].position.x / static_cast<float>(m_width)) -
          1.0f;
      vertices[i].position.y =
          m_screen_multiplier[1] *
          ((-2.0f * vertices[i].position.y / static_cast<float>(m_height)) +
              1.0f);
    }

    auto vertex = create_and_initialize_buffer(vertices, num_vertices,
        wn::runtime::graphics::resource_state::vertex_buffer);
    auto index = create_and_initialize_buffer(indices, num_indices,
        wn::runtime::graphics::resource_state::index_buffer);

    dat.m_setup_command_list->transition_resource(vertex.first,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::vertex_buffer);
    dat.m_setup_command_list->transition_resource(index.first,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::index_buffer);

    wn::runtime::graphics::graphics_pipeline* next_pipeline =
        m_scissor_enabled
            ? (texture ? m_scissored_pipeline.get()
                       : m_scissored_textureless_pipeline.get())
            : (texture ? m_scissorless_pipeline.get()
                       : m_scissorless_textureless_pipeline.get());

    wn::runtime::graphics::pipeline_layout* next_layout =
        texture ? m_pipeline_layout.get() : m_textureless_pipeline_layout.get();

    bool pipeline_changed = false;
    if (next_layout != m_last_pipeline_layout) {
      dat.m_command_list->bind_graphics_pipeline_layout(next_layout);
      m_last_pipeline_layout = next_layout;
      pipeline_changed = true;
    }

    if (next_pipeline != m_last_bound_pipeline) {
      m_last_bound_pipeline = next_pipeline;
      dat.m_command_list->bind_graphics_pipeline(next_pipeline);
    }

    if (m_scissor_enabled &&
        (pipeline_changed || m_last_set_scissor != m_set_scissor)) {
      dat.m_command_list->set_scissor(m_set_scissor);
      m_last_set_scissor = {};
    }

    // This is super unideal for now, but fix it later when we start caching
    // everything.
    if (texture) {
      WNTexture* t = reinterpret_cast<WNTexture*>(texture);
      t->used_frames = 1 << m_current_frame;

      const wn::runtime::graphics::descriptor_pool_create_info pool_infos[] = {
          {1, wn::runtime::graphics::descriptor_type::sampler},
          {1, wn::runtime::graphics::descriptor_type::sampled_image}};

      auto pool = m_device->create_descriptor_pool(pool_infos);
      wn::runtime::graphics::descriptor_set set =
          pool.create_descriptor_set(m_descriptor_set_layout.get());
      wn::runtime::graphics::sampler_descriptor samp[] = {{0, 0, &m_sampler}};
      wn::runtime::graphics::image_descriptor im[] = {
          {1, 0, wn::runtime::graphics::descriptor_type::sampled_image,
              &t->view, wn::runtime::graphics::resource_state::texture}};
      set.update_descriptors({}, im, samp);
      const wn::runtime::graphics::descriptor_set* s = &set;
      wn::containers::contiguous_range<
          const wn::runtime::graphics::descriptor_set*>
          sets(&s, 1);

      dat.m_command_list->bind_graphics_descriptor_sets(sets, 0);
      dat.m_pools.push_back(wn::core::move(pool));
      dat.m_sets.push_back(wn::core::move(set));
    }

    dat.m_command_list->bind_vertex_buffer(0, &vertex.first);
    dat.m_command_list->bind_index_buffer(
        wn::runtime::graphics::index_type::u32, &index.first);
    float constants[2] = {
        m_screen_multiplier[0] *
            (2.0f * translation.x / static_cast<float>(m_width)),
        m_screen_multiplier[1] *
            (-2.0f * translation.y / static_cast<float>(m_height))};

    dat.m_command_list->push_graphics_contants(
        0, 0, reinterpret_cast<const uint32_t*>(constants), 2);
    dat.m_command_list->draw_indexed(num_indices, 1, 0, 0, 0);

    dat.m_vertex_buffers.push_back(wn::core::move(vertex.first));
    dat.m_index_buffers.push_back(wn::core::move(index.first));
    dat.m_memory_arenas.push_back(wn::core::move(vertex.second));
    dat.m_memory_arenas.push_back(wn::core::move(index.second));
  }

  void EnableScissorRegion(bool enable) override {
    m_scissor_enabled = enable;
    if (!enable) {
      m_last_set_scissor = {};
    }
  }

  void SetScissorRegion(int x, int y, int width, int height) override {
    m_set_scissor = {static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  bool LoadTexture(Rocket::Core::TextureHandle& texture_handle,
      Rocket::Core::Vector2i& texture_dimensions,
      const Rocket::Core::String& source) override {
    wn::file_system::result r;
    wn::file_system::file_ptr file =
        m_mapping->open_file(get_string_view(source), r);
    if (r != wn::file_system::result::ok) {
      return false;
    }

    int width;
    int height;
    int channels;
    stbi_uc* data = stbi_load_from_memory(file->typed_data<uint8_t>(),
        static_cast<int>(file->size()), &width, &height, &channels, 4);
    if (data == nullptr) {
      return false;
    }
    texture_dimensions.x = width;
    texture_dimensions.y = height;
    // FOR NOW: if we have a 1-channel texture, then splat it across
    // all colors.
    if (channels == 1) {
      for (size_t j = 0; j < static_cast<size_t>(height); ++j) {
        for (size_t i = 0; i < static_cast<size_t>(width); ++i) {
          data[j * width * 4 + i * 4 + 1] = data[j * width * 4 + i * 4];
          data[j * width * 4 + i * 4 + 2] = data[j * width * 4 + i * 4];
          data[j * width * 4 + i * 4 + 3] = data[j * width * 4 + i * 4];
        }
      }
    }

    image_data image = create_and_initialize_texture(texture_dimensions.x,
        texture_dimensions.y, reinterpret_cast<const uint32_t*>(data),
        m_resource_init_command_list.get());
    m_resource_command_list_dirty = true;

    WNTexture* t = m_allocator->construct<WNTexture>(
        wn::core::move(image.texture), wn::core::move(image.texture_arena),
        wn::core::move(image.texture_view));
    texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(t);
    m_texture_upload_arenas.push_back(wn::core::move(image.upload_arena));
    m_texture_upload_buffers.push_back(wn::core::move(image.upload_buffer));
    return true;
  }

  bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
      const Rocket::Core::byte* source,
      const Rocket::Core::Vector2i& source_dimensions) override {
    image_data image = create_and_initialize_texture(source_dimensions.x,
        source_dimensions.y, reinterpret_cast<const uint32_t*>(source),
        m_resource_init_command_list.get());
    m_resource_command_list_dirty = true;

    WNTexture* t = m_allocator->construct<WNTexture>(
        wn::core::move(image.texture), wn::core::move(image.texture_arena),
        wn::core::move(image.texture_view));
    t->used_frames = 1 << m_current_frame;
    texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(t);

    m_texture_upload_arenas.push_back(wn::core::move(image.upload_arena));
    m_texture_upload_buffers.push_back(wn::core::move(image.upload_buffer));
    return true;
  }

  void ReleaseTexture(Rocket::Core::TextureHandle texture_handle) override {
    m_cleanup_textures.push_back(wn::memory::unique_ptr<WNTexture>(
        m_allocator, reinterpret_cast<WNTexture*>(texture_handle)));
  }

  Rocket::Core::CompiledGeometryHandle CompileGeometry(
      Rocket::Core::Vertex* vertices, int num_vertices, int* indices,
      int num_indices, Rocket::Core::TextureHandle _texture) override {
    WNTexture* texture = reinterpret_cast<WNTexture*>(_texture);

    // Coordinate space conversions
    for (int i = 0; i < num_vertices; ++i) {
      vertices[i].position.x =
          m_screen_multiplier[0] *
          ((2.0f * vertices[i].position.x / static_cast<float>(m_width)) -
              1.0f);
      vertices[i].position.y =
          m_screen_multiplier[1] *
          (-(2.0f * vertices[i].position.y / static_cast<float>(m_height)) +
              1.0f);
    }

    auto vertex = create_and_initialize_buffer(vertices, num_vertices,
        wn::runtime::graphics::resource_state::vertex_buffer);
    auto index = create_and_initialize_buffer(indices, num_indices,
        wn::runtime::graphics::resource_state::index_buffer);

    m_resource_init_command_list->transition_resource(vertex.first,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::vertex_buffer);
    m_resource_init_command_list->transition_resource(index.first,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::index_buffer);

    WNGeometry* t = m_allocator->construct<WNGeometry>(
        wn::core::move(vertex.first), wn::core::move(vertex.second),
        wn::core::move(index.first), wn::core::move(index.second), num_indices);

    if (texture) {
      const wn::runtime::graphics::descriptor_pool_create_info pool_infos[] = {
          {1, wn::runtime::graphics::descriptor_type::sampler},
          {1, wn::runtime::graphics::descriptor_type::sampled_image}};

      auto pool =
          wn::memory::make_unique<wn::runtime::graphics::descriptor_pool>(
              m_allocator, m_device->create_descriptor_pool(pool_infos));
      auto set = wn::memory::make_unique<wn::runtime::graphics::descriptor_set>(
          m_allocator,
          pool->create_descriptor_set(m_descriptor_set_layout.get()));

      wn::runtime::graphics::sampler_descriptor samp[] = {{0, 0, &m_sampler}};
      wn::runtime::graphics::image_descriptor im[] = {
          {1, 0, wn::runtime::graphics::descriptor_type::sampled_image,
              &texture->view, wn::runtime::graphics::resource_state::texture}};
      set->update_descriptors({}, im, samp);
      t->m_set = wn::core::move(set);
      t->m_pool = wn::core::move(pool);
      t->m_texture = texture;
    }
    m_resource_command_list_dirty = true;
    return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(t);
  }

  void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle _geometry,
      const Rocket::Core::Vector2f& translation) override {
    WNGeometry* geometry = reinterpret_cast<WNGeometry*>(_geometry);
    PerTargetData& dat = m_per_target_data[m_current_frame];

    wn::runtime::graphics::graphics_pipeline* next_pipeline =
        m_scissor_enabled
            ? (geometry->m_texture ? m_scissored_pipeline.get()
                                   : m_scissored_textureless_pipeline.get())
            : (geometry->m_texture ? m_scissorless_pipeline.get()
                                   : m_scissorless_textureless_pipeline.get());

    wn::runtime::graphics::pipeline_layout* next_layout =
        geometry->m_texture ? m_pipeline_layout.get()
                            : m_textureless_pipeline_layout.get();

    bool pipeline_changed = false;
    if (next_layout != m_last_pipeline_layout) {
      dat.m_command_list->bind_graphics_pipeline_layout(next_layout);
      m_last_pipeline_layout = next_layout;
      pipeline_changed = true;
    }

    if (next_pipeline != m_last_bound_pipeline) {
      m_last_bound_pipeline = next_pipeline;
      dat.m_command_list->bind_graphics_pipeline(next_pipeline);
    }

    if (m_scissor_enabled &&
        (pipeline_changed || m_last_set_scissor != m_set_scissor)) {
      dat.m_command_list->set_scissor(m_set_scissor);
      m_last_set_scissor = {};
    }
    // This is super unideal for now, but fix it later when we start caching
    // everything.
    if (geometry->m_texture) {
      geometry->m_texture->used_frames = 1 << m_current_frame;

      const wn::runtime::graphics::descriptor_set* s = geometry->m_set.get();
      wn::containers::contiguous_range<
          const wn::runtime::graphics::descriptor_set*>
          sets(&s, 1);
      dat.m_command_list->bind_graphics_descriptor_sets(sets, 0);
    }

    dat.m_command_list->bind_vertex_buffer(0, &geometry->m_vertex_buffer);
    dat.m_command_list->bind_index_buffer(
        wn::runtime::graphics::index_type::u32, &geometry->m_index_buffer);
    float constants[2] = {
        m_screen_multiplier[0] *
            (2.0f * translation.x / static_cast<float>(m_width)),
        m_screen_multiplier[1] *
            (-2.0f * translation.y / static_cast<float>(m_height))};

    dat.m_command_list->push_graphics_contants(
        0, 0, reinterpret_cast<const uint32_t*>(constants), 2);
    dat.m_command_list->draw_indexed(geometry->m_num_indices, 1, 0, 0, 0);
  }

  void ReleaseCompiledGeometry(
      Rocket::Core::CompiledGeometryHandle geometry) override {
    m_cleanup_geometry.push_back(wn::memory::unique_ptr<WNGeometry>(
        m_allocator, reinterpret_cast<WNGeometry*>(geometry)));
  }

  void setup_frame(uint32_t _frame) {
    for (auto it = m_cleanup_textures.begin(); it != m_cleanup_textures.end();
         ++it) {
      (*it)->used_frames &= ~(1 << _frame);
      if (!(*it)->used_frames) {
        it = m_cleanup_textures.erase(it);
        continue;
      }
      ++it;
    }
    for (auto it = m_cleanup_geometry.begin(); it != m_cleanup_geometry.end();
         ++it) {
      (*it)->used_frames &= ~(1 << _frame);
      if (!(*it)->used_frames) {
        it = m_cleanup_geometry.erase(it);
        continue;
      }
      ++it;
    }

    m_current_frame = _frame;

    PerTargetData& dat = m_per_target_data[m_current_frame];
    wn::runtime::graphics::render_area area{0, 0,
        static_cast<uint32_t>(dat.m_image->get_width()),
        static_cast<uint32_t>(dat.m_image->get_height())};
    wn::runtime::graphics::clear_value cc[]{{}};
    cc[0].color.float_vals[0] = 0.25f;
    cc[0].color.float_vals[1] = 0.25f;
    cc[0].color.float_vals[2] = 0.25f;
    cc[0].color.float_vals[3] = 1.0f;
    dat.m_command_pool->reset();
    dat.m_setup_command_pool->reset();

    dat.m_command_list = dat.m_command_pool->create_command_list();
    dat.m_setup_command_list = dat.m_setup_command_pool->create_command_list();
    dat.m_command_list->begin_render_pass(
        m_render_pass.get(), &dat.m_framebuffer, area, cc);
    dat.m_vertex_buffers.clear();
    dat.m_index_buffers.clear();
    dat.m_upload_buffers.clear();
    dat.m_memory_arenas.clear();
    m_last_bound_pipeline = nullptr;
    m_last_pipeline_layout = nullptr;
  }

  wn::core::pair<wn::runtime::graphics::command_list*,
      wn::runtime::graphics::command_list*>
  finalize_frame() {
    PerTargetData& dat = m_per_target_data[m_current_frame];
    dat.m_command_list->end_render_pass();
    dat.m_command_list->finalize();
    dat.m_setup_command_list->finalize();
    return wn::core::make_pair(
        dat.m_setup_command_list.get(), dat.m_command_list.get());
  }

  struct resource_init_res {
    resource_init_res(resource_init_res&& _other)
      : command_allocator(wn::core::move(_other.command_allocator)),
        command_list(wn::core::move(_other.command_list)),
        buffer_arenas(wn::core::move(_other.buffer_arenas)),
        upload_buffers(wn::core::move(_other.upload_buffers)) {}
    resource_init_res() {}
    wn::memory::unique_ptr<wn::runtime::graphics::command_allocator>
        command_allocator;
    wn::runtime::graphics::command_list_ptr command_list;
    wn::containers::deque<wn::runtime::graphics::arena> buffer_arenas;
    wn::containers::deque<wn::runtime::graphics::buffer> upload_buffers;
  };
  resource_init_res get_resource_init_resources() {
    if (m_resource_command_list_dirty) {
      m_resource_command_list_dirty = false;
      m_resource_init_command_list->finalize();
      resource_init_res r;
      r.command_list = wn::core::move(m_resource_init_command_list);
      r.buffer_arenas = wn::core::move(m_texture_upload_arenas);
      r.upload_buffers = wn::core::move(m_texture_upload_buffers);
      r.command_allocator = wn::core::move(m_init_command_allocator);
      m_init_command_allocator =
          wn::memory::make_unique<wn::runtime::graphics::command_allocator>(
              m_allocator, m_device->create_command_allocator());
      m_resource_init_command_list =
          m_init_command_allocator->create_command_list();
      m_texture_upload_arenas =
          wn::containers::deque<wn::runtime::graphics::arena>(m_allocator);
      m_texture_upload_buffers =
          wn::containers::deque<wn::runtime::graphics::buffer>(m_allocator);
      return wn::core::move(r);
    }
    return resource_init_res{};
  }

private:
  wn::memory::allocator* m_allocator;
  wn::runtime::graphics::device* m_device;
  unique_ptr<wn::runtime::graphics::command_allocator> m_init_command_allocator;
  wn::runtime::graphics::shader_module m_vertex_shader;
  wn::runtime::graphics::shader_module m_pixel_shader;
  wn::runtime::graphics::shader_module m_pixel_shader_no_texture;
  wn::containers::dynamic_array<PerTargetData> m_per_target_data;
  wn::containers::list<wn::memory::unique_ptr<WNTexture>> m_cleanup_textures;
  wn::containers::list<wn::memory::unique_ptr<WNGeometry>> m_cleanup_geometry;
  wn::runtime::graphics::sampler m_sampler;
  unique_ptr<wn::runtime::graphics::descriptor_set_layout>
      m_descriptor_set_layout;
  unique_ptr<wn::runtime::graphics::descriptor_set_layout>
      m_textureless_descriptor_set_layout;
  unique_ptr<wn::runtime::graphics::pipeline_layout> m_pipeline_layout;
  unique_ptr<wn::runtime::graphics::pipeline_layout>
      m_textureless_pipeline_layout;
  unique_ptr<wn::runtime::graphics::render_pass> m_render_pass;
  unique_ptr<wn::runtime::graphics::graphics_pipeline> m_scissorless_pipeline;
  unique_ptr<wn::runtime::graphics::graphics_pipeline> m_scissored_pipeline;

  unique_ptr<wn::runtime::graphics::graphics_pipeline>
      m_scissorless_textureless_pipeline;
  unique_ptr<wn::runtime::graphics::graphics_pipeline>
      m_scissored_textureless_pipeline;

  uint32_t m_current_frame = 0;
  uint32_t m_buffer_arena_index = 0;
  uint32_t m_texture_arena_index = 0;
  bool m_buffer_arena_is_coherent = false;

  wn::runtime::graphics::graphics_pipeline* m_last_bound_pipeline = nullptr;
  wn::runtime::graphics::pipeline_layout* m_last_pipeline_layout = nullptr;
  bool m_scissor_enabled = false;
  wn::runtime::graphics::scissor m_set_scissor = {};
  wn::runtime::graphics::scissor m_last_set_scissor = {};
  wn::file_system::mapping* m_mapping;

  wn::containers::deque<wn::runtime::graphics::arena> m_texture_upload_arenas;
  wn::containers::deque<wn::runtime::graphics::buffer> m_texture_upload_buffers;

  wn::runtime::graphics::command_list_ptr m_resource_init_command_list;
  bool m_resource_command_list_dirty = false;
  wn::math::vec2f m_screen_multiplier;
  uint32_t m_width;
  uint32_t m_height;
};

int32_t wn_application_main(
    const wn::runtime::application::application_data* _data) {
  uint32_t force_adapter = 0;
  bool choosing_adapter = false;

  for (size_t i = 0; i < static_cast<size_t>(_data->executable_data->argc);
       ++i) {
    if (wn::containers::string_view("--force_adapter") ==
        wn::containers::string_view(_data->executable_data->argv[i])) {
      choosing_adapter = true;
    } else if (choosing_adapter) {
      choosing_adapter = false;
      wn::memory::readuint32(_data->executable_data->argv[i], force_adapter,
          strnlen(_data->executable_data->argv[i], 10));
    }
  }

  wn::logging::log* log = _data->default_log;
  wn::memory::allocator* allocator = _data->system_allocator;

  log->set_log_level(wn::logging::log_level::info);

  window_factory window_factory(allocator, log);
  device_factory device_factory(allocator, log);

  job_signal s(0);
  auto window =
      window_factory.create_window(wn::runtime::window::window_type::system,
          wn::multi_tasking::job_pool::this_job_pool(), &s, _data, 0, 0,
          k_width, k_height);

  contiguous_range<const adapter_ptr> adapters =
      device_factory.query_adapters();

  if (adapters.empty()) {
    log->log_error("Could not find any adapters");
    return -1;
  }
  const adapter_ptr& adapter = adapters[force_adapter];
  log->log_info("Selected adapter ", force_adapter, ": ", adapter->name());
  log->flush();
  s.wait_until(1);
  float multiplier =
      static_cast<float>(wn::runtime::window::k_default_density) /
      static_cast<float>(window->get_dpi());
  uint32_t width = static_cast<uint32_t>(window->get_width() * multiplier);
  uint32_t height = static_cast<uint32_t>(window->get_height() * multiplier);
  if (!window->is_valid()) {
    log->log_error("Could not create a valid window");
    return -1;
  }

  auto input_context = window->get_input_context();
  auto surface = adapter->make_surface(allocator, window.get());
  if (surface.second != graphics_error::ok) {
    // TODO(awoloszyn): Retry policy with different adapters.
    log->log_error("Could not use adapter ", adapter->name(), " for rendering");
    return -1;
  }

  const wn::runtime::graphics::swapchain_create_info create_info = {
      surface.first.valid_formats()[0], 3,
      wn::runtime::graphics::swap_mode::fifo,
      wn::runtime::graphics::discard_policy::discard};

  auto device = adapter->make_device(allocator, log, {});

  wn::runtime::graphics::queue_ptr queue = device->create_queue();
  auto swapchain = device->create_swapchain(
      surface.first, create_info, queue.get(), multiplier);

  wn::runtime::graphics::fence image_fence = device->create_fence();

  wn::containers::dynamic_array<wn::runtime::graphics::fence> fences(allocator);
  wn::containers::dynamic_array<bool> unused(allocator);
  wn::containers::dynamic_array<wn::runtime::graphics::image*> swapchain_images(
      allocator);

  fences.reserve(swapchain->info().num_buffers);

  for (uint32_t i = 0; i < swapchain->info().num_buffers; ++i) {
    unused.push_back(true);
    fences.emplace_back(device->create_fence());
    swapchain_images.push_back(swapchain->get_image_for_index(i));
  }

  // Lets set up Rocket

  wn::file_system::factory fs_factory(allocator, _data->executable_data);
  wn::file_system::mapping_ptr file_system = fs_factory.make_mapping(
      allocator, wn::file_system::mapping_type::memory_backed);
  file_system->initialize_files(rocket_assets::get_files());

  Rocket::Core::Context context;
  WNRenderer renderer(allocator, device.get(), swapchain_images,
      file_system.get(), &context, width, height);
  WNSystemInterface system_interface(log, window.get());
  WNFileInterface file_interface(log, wn::core::move(file_system));

  Rocket::Core::SetRenderInterface(&context, &renderer);
  Rocket::Core::SetSystemInterface(&context, &system_interface);
  Rocket::Core::SetFileInterface(&context, &file_interface);

  Rocket::Core::Initialise(&context);
  Rocket::Core::DocumentContext* documents =
      Rocket::Core::CreateDocumentContext(
          &context, "main", Rocket::Core::Vector2i(width, height));

  Rocket::Core::FontDatabase::LoadFontFace(&context,
      "assets/fonts/FiraCode-Regular.ttf", "Fira Code",
      Rocket::Core::Font::Style::STYLE_NORMAL,
      Rocket::Core::Font::Weight::WEIGHT_NORMAL);

  Rocket::Debugger::Initialise(&context, documents);
  Rocket::Controls::Initialise(&context);
  // Rocket::Debugger::SetVisible(&context, true);
  Rocket::Core::ElementDocument* document =
      documents->LoadDocument("assets/main.rml");
  if (document != NULL) {
    document->Show();
    document->RemoveReference();
  }

  // DONE SETTING UP ROCKET
  bool debugger_visible = false;

  for (;;) {
    image_fence.reset();
    uint32_t idx = swapchain->get_next_backbuffer_index(&image_fence, nullptr);
    if (!unused[idx]) {
      // As the code sits right now we don't need this.
      // we know this will be free, since we have already waited
      // on completion fence first.
      // However once we have semaphores, all other fences can be
      // removed except for this fence, and we will need it again.
      fences[idx].wait();
      fences[idx].reset();
    } else {
      unused[idx] = false;
    }
    image_fence.wait();

    wn::runtime::graphics::command_allocator alloc =
        device->create_command_allocator();

    wn::runtime::graphics::command_list_ptr setup_list =
        alloc.create_command_list();

    setup_list->transition_resource(*swapchain->get_image_for_index(idx),
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, 1, wn::runtime::graphics::resource_state::present,
        wn::runtime::graphics::resource_state::render_target);
    setup_list->finalize();
    queue->enqueue_command_list(setup_list.get());

    documents->Update();  // Does/can this EVER call graphics functions? If not
                          // pull it out.

    renderer.setup_frame(idx);
    auto resources = renderer.get_resource_init_resources();
    // TODO(awoloszyn): Be careful here, we have to make sure we are
    //    done rendering before we delete this.
    //    for now this works because we have a fence at the end
    //    of this loop
    if (resources.command_list) {
      queue->enqueue_command_list(resources.command_list.get());
    }

    documents->Render();
    // Not sure if these will actually come up again, but good to be sure
    auto new_resources = renderer.get_resource_init_resources();
    if (new_resources.command_list) {
      queue->enqueue_command_list(new_resources.command_list.get());
    }
    wn::core::pair<wn::runtime::graphics::command_list*,
        wn::runtime::graphics::command_list*>
        render_lists = renderer.finalize_frame();
    queue->enqueue_command_list(render_lists.first);
    queue->enqueue_command_list(render_lists.second);

    wn::runtime::graphics::command_list_ptr teardown_list =
        alloc.create_command_list();
    teardown_list->transition_resource(*swapchain->get_image_for_index(idx),
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, 1, wn::runtime::graphics::resource_state::render_target,
        wn::runtime::graphics::resource_state::present);
    teardown_list->finalize();
    queue->enqueue_command_list(teardown_list.get());

    wn::runtime::graphics::fence completion_fence = device->create_fence();
    queue->enqueue_fence(fences[idx]);
    queue->enqueue_fence(completion_fence);
    completion_fence.wait();

    swapchain->present(queue.get(), nullptr, idx);
    wn::runtime::window::input_event evt;
    int key_modifier_state = get_key_modifier_state(window.get());
    while (input_context->get_event(&evt)) {
      switch (evt.type()) {
        case wn::runtime::window::event_type::key_down:
          documents->ProcessKeyDown(
              key_code_to_key_index(evt.get_keycode()), key_modifier_state);
          if (evt.get_keycode() == key_code::key_l &&
              window->get_key_state(key_code::key_any_ctrl)) {
            debugger_visible ^= true;
            Rocket::Debugger::SetVisible(&context, debugger_visible);
          }
          break;
        case wn::runtime::window::event_type::key_up:
          documents->ProcessKeyUp(
              key_code_to_key_index(evt.get_keycode()), key_modifier_state);
          break;
        case wn::runtime::window::event_type::mouse_down:
          documents->ProcessMouseButtonDown(
              mouse_button_to_index(evt.get_mouse_button()),
              key_modifier_state);
          break;
        case wn::runtime::window::event_type::mouse_up:
          documents->ProcessMouseButtonUp(
              mouse_button_to_index(evt.get_mouse_button()),
              key_modifier_state);
          break;
        case wn::runtime::window::event_type::mouse_move:
          documents->ProcessMouseMove(
              static_cast<int>(evt.get_mouse_x() * multiplier),
              static_cast<int>(evt.get_mouse_y() * multiplier),
              key_modifier_state);
          break;
        case wn::runtime::window::event_type::text_input:
          documents->ProcessTextInput(
              static_cast<Rocket::Core::word>(evt.get_character()));
          break;
      }
    }
  }
  // window->release_input_context(input_context);

  return 0;
}
