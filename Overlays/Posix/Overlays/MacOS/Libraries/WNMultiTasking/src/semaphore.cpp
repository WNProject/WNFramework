#include "WNMultiTasking/inc/internal/semaphore_base.h"

namespace wn {
namespace multi_tasking {
namespace internal {

std::atomic_int64_t semaphore_base::k_semaphore_counter;

}
}  // namespace multi_tasking
}  // namespace wn