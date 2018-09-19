#  Turn off plugin loading.
# 1) We don't need this
# 2) LLVM is not smart enough to understand cross compiling for Andoid on Windows

include(${LLVM_SOURCE_DIR}/cmake/modules/HandleLLVMOptions.cmake)

set(LLVM_ENABLE_PLUGINS OFF)