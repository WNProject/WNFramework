#ifndef _WN_GRAPHICS_TEST_PIXEL_TEST_FIXTURE_H__
#define _WN_GRAPHICS_TEST_PIXEL_TEST_FIXTURE_H__

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "WNGraphics/inc/WNShaderModule.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMath/inc/WNBasic.h"

#include "WNGraphics/test/inc/WNPixelTestConfig.h"

#include <stb_public.h>

namespace wn {
namespace runtime {
namespace graphics {
namespace testing {

using pixel = wn::containers::array<uint8_t, 4>;

template <size_t width, size_t height>
using image =
    wn::containers::array<wn::containers::array<pixel, width>, height>;

struct comparison_result {
  // The absolute_error is simply a measure of how "incorrect"
  // a given image is. It is only valid to be compared against
  // other results from the same comparator. It has no units.
  float absolute_error = 0.f;
  // The success/fail result of the comparison.
  bool success = false;
};

template <uint8_t low_threshold = 1, uint8_t high_threshold = 100,
    size_t max_exceeded_low = 0>
struct fuzzy_comparator {
  template <size_t width, size_t height>
  static comparison_result compare(
      const image<width, height>& a, const image<width, height>& b) {
    float total_error = 0;
    size_t low_threshold_violations = 0;
    size_t high_threshold_violations = 0;
    for (size_t x = 0; x < width; ++x) {
      for (size_t y = 0; y < height; ++y) {
        bool exceeded_low = false;
        bool exceeded_high = false;
        for (size_t i = 0; i < 4; ++i) {
          int32_t a_val = a[y][x][i];
          int32_t b_val = b[y][x][i];
          uint32_t error = wn::math::abs(a_val - b_val);
          total_error += error;
          if (error > low_threshold) {
            exceeded_low = true;
          }
          if (error > high_threshold) {
            exceeded_high = true;
          }
        }
        low_threshold_violations += exceeded_low;
        high_threshold_violations += exceeded_high;
      }
    }
    bool success =
        high_threshold_violations > 0
            ? false
            : low_threshold_violations > max_exceeded_low ? false : true;
    comparison_result result;
    result.absolute_error = total_error;
    result.success = success;
    return result;
  }
};

using default_fuzzy_comparator = fuzzy_comparator<>;

inline bool should_recreate_image() {
  for (int32_t i = 0; i < wn::testing::k_arg_c; ++i) {
    if (strcmp(wn::testing::k_arg_v[i], "-regenerate-images") == 0) {
      return true;
    }
  }
  return false;
}

inline std::string get_image_filename() {
  const ::testing::TestInfo* const test_info =
      ::testing::UnitTest::GetInstance()->current_test_info();
  std::string file_path;
  bool set_path = false;
  for (int32_t i = 0; i < wn::testing::k_arg_c; ++i) {
    if (strcmp(wn::testing::k_arg_v[i], "-image_path") == 0) {
      set_path = true;
    } else if (set_path == true) {
      file_path += wn::testing::k_arg_v[i];
    }
  }
  if (file_path.empty()) {
    file_path += k_test_image_path;
  }
  file_path += "/";
  file_path +=
      test_info->test_case_name() + std::string(".") + test_info->name();
  file_path += ".tga";
  return core::move(file_path);
}

template <size_t width = 100, size_t height = 100,
    typename comparator = default_fuzzy_comparator>
class pixel_test : public ::testing::Test {
protected:
  pixel_test()
    : m_static_log(&m_logger),
      m_log(m_static_log.log()),
      m_logger(&m_buffer),
      m_buffer(&m_allocator) {}

  virtual ~pixel_test() {}

  void load_image(image<width, height>& image) {
    int w = 0;
    int h = 0;
    int channels_in_file = 4;
    stbi_uc* file =
        stbi_load(get_image_filename().c_str(), &w, &h, &channels_in_file, 4);
    ASSERT_EQ(width, w);
    ASSERT_EQ(height, h);
    ASSERT_EQ(4, channels_in_file);
    ASSERT_NE(nullptr, file);
    for (size_t x = 0; x < width; ++x) {
      for (size_t y = 0; y < height; ++y) {
        for (size_t i = 0; i < 4; ++i) {
          image[y][x][i] = file[y * width * 4 + 4 * x + i];
        }
      }
    }
    stbi_image_free(file);
  }

  void write_image(image<width, height>& image) {
    stbi_write_tga(
        get_image_filename().c_str(), width, height, 4, &image[0][0][0]);
  }

  void run_test(
      const std::function<void(wn::runtime::graphics::adapter::api_type,
          wn::runtime::graphics::device*, wn::runtime::graphics::queue*,
          wn::runtime::graphics::image*)>& func) {
    containers::dynamic_array<testing::image<width, height>> result_images(
        &m_allocator);

    wn::runtime::graphics::factory device_factory(&m_allocator, m_log);
    for (auto& adapter : device_factory.query_adapters()) {
      wn::runtime::graphics::device_ptr device =
          adapter->make_device(&m_allocator, m_log);
      ASSERT_NE(nullptr, device);
      wn::runtime::graphics::queue_ptr queue = device->create_queue();

      const wn::containers::contiguous_range<
          const wn::runtime::graphics::arena_properties>
          arena_properties = device->get_arena_properties();
      size_t image_arena_index = 0;
      bool found_arena = false;

      for (; image_arena_index < arena_properties.size(); ++image_arena_index) {
        if (arena_properties[image_arena_index].device_local &&
            arena_properties[image_arena_index].allow_render_targets) {
          found_arena = true;
          break;
        }
      }

      ASSERT_TRUE(found_arena);

      size_t buffer_arena_index = 0;
      for (; buffer_arena_index < arena_properties.size();
           ++buffer_arena_index) {
        if (arena_properties[buffer_arena_index].host_visible &&
            arena_properties[buffer_arena_index].allow_buffers) {
          found_arena = true;
          break;
        }
      }
      wn::runtime::graphics::clear_value value{};
      wn::runtime::graphics::image image = device->create_image(
          wn::runtime::graphics::image_create_info{width, height,
              wn::runtime::graphics::data_format::r8g8b8a8_unorm,
              static_cast<wn::runtime::graphics::resource_states>(
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::copy_source) |
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::render_target))},
          value);

      wn::runtime::graphics::image_memory_requirements image_requirements =
          image.get_memory_requirements();
      wn::runtime::graphics::arena image_arena =
          device->create_arena(image_arena_index, image_requirements.size);
      image.bind_memory(&image_arena, 0);
      wn::runtime::graphics::command_allocator alloc =
          device->create_command_allocator();

      wn::runtime::graphics::command_list_ptr setup_command_list =
          alloc.create_command_list();
      setup_command_list->transition_resource(image,
          wn::runtime::graphics::resource_state::initial,
          wn::runtime::graphics::resource_state::render_target);
      setup_command_list->finalize();
      queue->enqueue_command_list(setup_command_list.get());

      wn::runtime::graphics::image::image_buffer_resource_info resource_info =
          image.get_buffer_requirements();

      wn::runtime::graphics::buffer buffer =
          device->create_buffer(resource_info.total_memory_required,
              static_cast<wn::runtime::graphics::resource_states>(
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::host_read,
                      wn::runtime::graphics::resource_state::copy_dest)));

      wn::runtime::graphics::buffer_memory_requirements buffer_requirements =
          buffer.get_memory_requirements();
      wn::runtime::graphics::arena buffer_arena =
          device->create_arena(buffer_arena_index, buffer_requirements.size);
      buffer.bind_memory(&buffer_arena, 0);

      func(adapter->api(), device.get(), queue.get(), &image);

      wn::runtime::graphics::fence completion_fence = device->create_fence();
      queue->enqueue_fence(completion_fence);
      completion_fence.wait();
      completion_fence.reset();

      wn::runtime::graphics::command_list_ptr copy_list =
          alloc.create_command_list();
      copy_list->transition_resource(buffer,
          wn::runtime::graphics::resource_state::initial,
          wn::runtime::graphics::resource_state::copy_dest);
      copy_list->transition_resource(image,
          wn::runtime::graphics::resource_state::render_target,
          wn::runtime::graphics::resource_state::copy_source);

      copy_list->copy_image_to_buffer(image, buffer, 0);
      copy_list->transition_resource(buffer,
          wn::runtime::graphics::resource_state::copy_dest,
          wn::runtime::graphics::resource_state::host_read);
      copy_list->finalize();

      queue->enqueue_command_list(copy_list.get());
      queue->enqueue_fence(completion_fence);
      completion_fence.wait();

      testing::image<width, height> processed_image;

      void* image_data = buffer.map();
      ASSERT_NE(image_data, nullptr);

      uint8_t* image_pixels = static_cast<uint8_t*>(image_data);
      for (size_t y = 0; y < width; ++y) {
        for (size_t x = 0; x < height; ++x) {
          processed_image[y][x][0] =
              image_pixels[y * resource_info.row_pitch_in_bytes + 4 * x];
          processed_image[y][x][1] =
              image_pixels[y * resource_info.row_pitch_in_bytes + 4 * x + 1];
          processed_image[y][x][2] =
              image_pixels[y * resource_info.row_pitch_in_bytes + 4 * x + 2];
          processed_image[y][x][3] =
              image_pixels[y * resource_info.row_pitch_in_bytes + 4 * x + 3];
        }
      }

      if (should_recreate_image()) {
        result_images.push_back(processed_image);
        continue;
      }

      testing::image<width, height> expected_image;
      load_image(expected_image);
      comparison_result result =
          comparator::compare(expected_image, processed_image);
      EXPECT_TRUE(result.success) << " Total Error: " << result.absolute_error;
    }

    if (should_recreate_image()) {
      ASSERT_FALSE(result_images.empty());
      if (result_images.size() == 1 || result_images.size() == 2) {
        // If there are only one or 2, then either it is the "right"
        // choice, or it does not matter since the error between them
        // will be symmetrical.
        write_image(result_images[0]);
      } else {
        // Otherwise we have 3+ images, choose the one that
        // minimizes the errors between the images.
        wn::containers::dynamic_array<float> errors(
            &m_allocator, result_images.size());
        for (size_t i = 0; i < result_images.size(); ++i) {
          for (size_t j = 0; j < result_images.size(); ++j) {
            if (i == j) {
              continue;
            }
            // If we are negative, that means we are not viable at all.
            if (errors[i] >= 0.f) {
              comparison_result result =
                  comparator::compare(result_images[i], result_images[j]);
              if (result.success == false) {
                errors[i] = -1.0f;
                continue;
              }
              errors[i] += result.absolute_error;
            }
          }
        }
        float lowest_error = errors[0];
        size_t lowest_image = 0;
        for (size_t i = 1; i < errors.size(); ++i) {
          if (lowest_error < 0) {
            lowest_error = errors[i];
            lowest_image = i;
            continue;
          }
          if (errors[i] >= 0 && errors[i] < lowest_error) {
            lowest_image = i;
            lowest_error = errors[i];
          }
        }
        ASSERT_GE(lowest_error, 0);
        write_image(result_images[lowest_image]);
      }
    }
  }

  uint32_t get_width() const {
    return width;
  }

  uint32_t get_height() const {
    return height;
  }

  static void flush_buffer(void* v, const char* bytes, size_t length,
      const wn::logging::color_element*, size_t) {
    wn::containers::string* s = static_cast<wn::containers::string*>(v);
    s->append(bytes, length);
  }

  using buffer_logger = logging::buffer_logger<flush_buffer>;
  using log_buff = wn::containers::string;

  wn::testing::allocator m_allocator;
  logging::static_log<> m_static_log;
  logging::log* m_log;
  buffer_logger m_logger;
  log_buff m_buffer;
};

}  // namespace testing
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // _WN_GRAPHICS_TEST_PIXEL_TEST_FIXTURE_H__