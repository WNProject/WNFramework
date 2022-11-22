# Code Conventions (C/C++)

In general the conventions outlined in the [Google C++ Style Guide] are what we
are following. We have some modifications with respect to naming and a few rules
to be outlined below.

## Naming

### Namespaces

In general everything should be in the `wn` namespace. Furthermore every set of
functionality, generally library, should be in it's own sub-namespace. Internal
only functionality should be in a further namespace called internal. At the end
of every namespace a comment should be included describing the namespace that
was closed. Anonymous namespaces should be closed with the comment
`// namespace` to help with identifying start and finish.

```cpp
namespace wn {
namespace containers {
namespace internal {

class deque_iterator {};

}  // namespace internal

class deque {};

}  // namespace containers
}  // namespace wn

namespace {
}  // namespace
```

### Classes

Class names should be all lower-case with word separation done with underscores.
They are already expected to be in a `wn::<library>::namespace` so the name
should not repeat this information.

```cpp
class my_foo;
struct my_bar;
```

### Functions & Class Methods

Free functions follow the same naming conventions as classes. Their names should
be descriptive of the operation they perform, and longer names should be
preferred over potentially unclear abbreviations.

### Arguments

Function arguments should follow the same rules as functions and methods, but
must be prefixed with an `_` denoting they are an input/output to/from this
method.

```cpp
int foo(int _bar, int* _baz);
```

### Local Variables

Local variables should be the same as arguments but should omit the underscore.

```cpp
int foo() {
    int bar = 4;
}
```

### Member Variables

Member variables should be prefixed with `m_` so that they can be readily
identified.

### Static Variables

Private static member variables should be prefixed with `s_` in order to
identify them. Public static member variables can omit the `s_` to conform more
with public interface styling. Static variables of any type are generally
discouraged, and should only be used with careful thought. Non-member static
variables should not be used unless they are constant see
[Global Constants](#global-constants).

### Thread Local Variables

Thread local variables should be prefixed with `tl_` in order to identify them.
Thread local variables of any type are generally discouraged, and should only be
used with careful thought. There should never thread local public members or
globals or any kind. If access to one is needed it should be wrapped in some way
to disallow direct access.

### Global Constants

Global constants should be used in place of Macros wherever possible. They
should be named as other variables, but prefixed with `k_` in order to identify
hem.

### Template Parameters

Template parameters should be in sentence case. One exception is if the usage is
very localized and clear, an abbreviated form can be used.

```cpp
template <typename MyType>
class fooClass {
  ...
};

template <typename T>
operator=(const T& _other) {
  ...
}
```

### Include Guards

Include guards follow the same structure as the namespace and class name for the
main class in the file. This should also be the file name. The closing of the
include guard should also contain a comment describing what has been closed.
Also, always use both include guards and `#pragma once` to ensure fastest
compilation.

```cpp
#pragma once
#ifdef __WN_CONTAINERS_DEQUE_H__
#define __WN_CONTAINERS_DEQUE_H__

...

#endif  // __WN_CONTAINERS_DEQUE_H__
```

### Macros

Macros should be avoided if possible, but sometimes it is required. Other than
include guards preprocessor directives should be ALL_CAPS separated by `_`
characters. In conditional preprocessor sections, at the end of the directive a
comment should be included describing the section that was closed.

```cpp
#define MY_FOO
#define MY_FOO(bar)
#ifdef FOO

...

#endif  // FOO
```

### Numeric Types

As a rule, the numeric types specified in `<cstdint>` should be used instead of
the more traditional `int` and `long int` types. This ensures clarity of width,
and more control over data types than otherwise.

### Discarding Return Values

Returns from function should always try to be incorporated into proper program
flow, but in the rare cases where ignoring the return explicitly is desired the
use of a C-style `void` cast should be used. This is the only place that a
C-style cast should be used in C++ code.

```cpp
int foo() {
  return 1;
}

(void)foo(); // this is ok
static_cast<void>(foo()) // this is discourage
```

### System Specific Code

When writing system-specific code, Win32 for example, any variables, functions
that interact with system-specific API should use the types expected by the API
documentation, rather than their equivalent types from this document. Also, when
able, prefix all system-specific code with `::` to help in avoiding name
collisions.

```cpp
::GetModuleHandle(NULL); // this is OK
::GetModuleHandle(nullptr); // this is discouraged
GetModuleHandle(NULL); // add global namespace ::
```

## Formatting

Our formatting is based off the Google C++ style guide with some modifications.
We use clang-format extensively, and it should handle all formatting. We support
`clang-format` version **10**. See our [.clang-format](../../.clang-format) for
further details.

<!-- external links -->
[Google C++ Style Guide]: https://google-styleguide.googlecode.com/svn/trunk/cppguide.html
