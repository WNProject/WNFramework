# WNScript

All of WNScript is very much in flux and subject to change

## Modes

### Runtime

There are fundamentally 3 different ways in which WNScript can operate.

| Mode              | Description                                                                                                                                                                                 |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Event Script      | In this mode, multiple scripts are expected to be running in parallel, and interact with each-other through an Event system. This is the method expected when embedding WNScript in a game. |
| Sequential Script | This script runs as if it were executing on a single thread in isolation. Conceptually similar to a single-threaded C or Java application.                                                  |
| Compute Script    | This script is used to perform highly parallelizable data computations. It is constrained to be able to run on a variety of platforms such as multiple CPUs or GPUs.                        |

### Execution Environment

This section is only useful when integrating WNScript into projects. It has very
little bearing on how code should be written. See [Execution
Environment](WNScriptExecution.asciidoc) for more details.

## Design

### Philosophy

There are 3 main goals in the creation of WNScript.

1. Speed of processing the language

   This means that the language, although grammatically similar to C++ or
   Java, has removed many of the constructs that complicate the parsing and
   processing of those languages.

1. Limited number of language constructs to learn

   This language should be usable for many different aspects of
   development, whether or not it is Gameplay, Shaders, Asset Conversion or
   Physics Computation. This goal allows a programmer to learn the language
   once, and be productive in all areas.

1. The language constructs should be extensible

   Custom, domain-specific types and constructs should be available for
   use. For example, custom vector/quaternion/matrix types could be added
   and, from the language standpoint be treated as equal to the builtin
   types, including

   - Constant Propagation and Folding
   - Dead Code Eliminiation
   - Highly customized vector operations

## Naming

### Types

Valid types must be of the form `(A-Z)(a-z|A-Z|0-9|_)\*` meaning that they can
contain any upper or lower case roman letters, any digit and the underscore
character. Furthermore they **must** start with an upper case roman letter.

### Values

Valid values must be of the form `(a-z|\_)(a-z|A-Z|0-9|_)\*` meaning that they
can contain any upper or lower case roman letters, any digit and the underscore
character. Furthermore they **must** start with either a lower case roman letter
or an underscore character.

## Code Constructs

### Types

WNScript is a statically and explicitly typed language. No implicit casting
between types is done, and all operations must be given the exact types expected
for the operation.

> **Note** explicit casting will be added soon. Implicit widening casting may be
done in the future.

#### Builtins

| Type  | C++ Equivalent |
| ----- | -------------- |
| Void  | void           |
| Int   | int32\_t       |
| Float | float          |
| Bool  | bool           |
| Char  | uint8\_t       |

### Functions

Functions in WNScript follow the same logical layout as a C/C++ function.
Function names are considered values and must follow the naming conventions
defined in [Naming:Values](#Naming-Values).

A `return` statement must be added at the end of any function. The value of the
`return` statement must match the return type of the function. One exception to
this is, if the function has a return type of `Void` the `return` statement may
be omitted. A `return` statement may appear anywhere in a function, but at least
one must be reached during the execution of the function.

```cpp
Void foo() {
  // No return required, since this is a Void return.
}

Int bar() {
  return 4;
}

Int baz(Int x, Float y) {
  if (y > 3.0) {
    return x;
  } else {
    return 0;
  }
  // Although permissible, No return needed here as either `return x;` or
  // `return 0;` will be hit first.
}
```

### Events

An Event in WNScript is semantically very similar to a function, except that is
not allowed to return a value. It is a function that implicitly has a Void
return type. These may be run immediately or may be deferred until later.

```cpp
foo(Signal s) {
  // This is an event, it may or may not be run when it is called, but
  // will eventually be run.
  s.Decrement();
}

Int bar() {
  Signal s(1)
  foo(s);

  s.WaitUntilValue(0);
}
```

### Signals

A `Signal` is used to communicate state out of an event. A `Signal` has an
initial value and can be incremented or decremented at will. Any function or
event may call Signal.WaitUntilValue(Int). This function or event will wait
until the signal has been set to the given value. Any signal passed to anothe
function or event must exist for the duration of that function or event. Put
another way, any signal that is derived from another signal must dissappear
before the original.

### Control Flow

#### Conditional Statements

Standard `if` statements can be used in WNScript. Any number of `else if`
clauses can optionally be added, and an optional `else` clause can be placed
last. The argument to *if* and *else if* statements must be of `Bool` type.

```cpp
if (a) {
  // Do something.
} else if (b == 3) {
  // Do something else.
} else {
  // Do the last thing.
}
```

## Memory

Given the [Mode](#Modes) that WNScript is running in, a different memory model
may be used for the execution of WNScript. See [Memory](WNScriptMemory.md) for
more details

> **Note** describe sequence points, @tags, additional features as they are
added.
