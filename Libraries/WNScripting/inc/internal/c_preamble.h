// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_INTERNAL_C_PREMBLE_H__
#define __WN_SCRIPTING_INTERNAL_C_PREMBLE_H__

namespace wn {
namespace scripting {
namespace internal {
const char* c_premable =
    "#include <stddef.h>\n"
    "#include <stdint.h>\n"
    "#pragma message(\"These need to be re-defined correctly "
    "         right now they are not thread-safe at all\")\n"
    "#define _wns_atomic_dec(x) (x--)\n"
    "#define _wns_atomic_inc(x) (x++)\n"
    "#define _wns_fence() do {} while(false)\n"
    "#define __wns_inline__ static inline;\n"
    "typdef void(*_vtable_function)();\n"
    "typedef const _vtable_function* _vtable;\n"
    "typedef void (*_undefined_function)();\n"
    "";
}  // namespace internal
}  // namespace scripting
}  // namespace wn

#endif
