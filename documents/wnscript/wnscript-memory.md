# Object Types

An object can be defined in one of 4 ways. For an object of type `T` being
declared it may have the following declarations.

| Type       | Documenation                                               |
| ---------- | ---------------------------------------------------------- |
| `shared T` | [Shared Ownership](#ObjectTypes-SharedOwnership)           |
| `weak T`   | [Weak Ownership](#ObjectTypes-WeakOwnership)               |
| `T`        | [Unique Ownership](#ObjectTypes-UniqueOwnership)           |
| `T?`       | [Nullable Ownership](#ObjectTypes-UniqueNullableOwnership) |

> **Important** All of the following rules apply to the [Sequential
Script](WNScript.asciidoc#Modes-Runtime) runtime mode. Deviations will be
described in [Runtime Mode Effects](#RuntimeModeEffects)

## Shared Ownership

Given a `shared T`, you can always create a new `shared T` from it. Both `shared
T` values reference the same object, and the object will only be destroyed when
the last `shared T` disappears.

```cpp
...
shared T my_t = T(); // Creates a new T()
shared T my_other_t = my_t; // myOtherT points at the same object as my_t.
shared T my_third_t = my_t.Copy(); // Points to a new object that is a copy.

my_t.DoThing(); // Does some thing
my_other_t.DidSomethingHappen() // Returns true, since my_t and my_other_t are
                                // the same, anything that happens to one
                                // happens to the other.
my_third_t.DidSomethingHappen() // Return false, since my_third_t is a copy of
                                // my_t.
...
```

## Weak Ownership

Given a `weak T`, you can create futher `weak T` objects from it. You can also
create new `shared T` objects from it. In the same way as `shared T` all `weak
T` values will point to the same actual object. Unlike `shared T` values, the
object may be removed before all `weak T` objects dissapper. Specifically once
all `shared T` objects dissappear, all `weak T` objects will be invalidated.

```cpp
...
{
  shared T my_shared_t = T();
  weak T my_weak_t = my_shared_t; // The same object as my_shared_t.

  if (my_weak_t) {
    // This will get hit.
  }

  my_shared_t = None; // my_shared_t now references nothing.

  if (my_weak_t) {
    // This will not get hit, as my_shared_t has disappeared.
  }
}
...
```

As well as creating a new `shared T` value, it is possible for a `weak T` to
temporarily force an object to continue to exist using a pin().

```cpp
...
Int foo(weak T my_weak_t) {
  try_pin(my_weak_t) { // If my_weak_t was not None, it is guaranteed to
                       // stay that way until the closing }
    my_weak_t.DoThing(); // Safe
  } else {
    // my_weak_t was None
  }
}
...
```

## Unique Ownership

A unique ownership object can only have a single value pointing to it at any
given time. It can be passed as a function parameter but cannot appear on the
right hand side of an assignment operation. Unique ownership objects, including
function parameters must never be null and are always guarenteed not to be.

```cpp
...
Int foo(T my_unique_t) {
  T my_new_t = my_unique_t; // << NOT ALLOWED
  T my_new_t_2 = my_unique_t.Copy(); // Allowed

  foo(my_new_t_2); // Allowed
}
...
```

## Nullable Ownership

Nullable ownership is exactly the same as unique ownership except that it is
possible for the value to be None.

## Runtime Mode Effects

### Sequential Scripts

Events in Sequential Scripts can be run in parallel with the main execution.
These events are expected to not interact with data that is being used by the
main thread of execution, or any other events at the same time.

### Event Scripts

Event scripts run in a very similar mode to Sequential scripts. The biggest
caveats are as follows:

1. `weak T` objects will ONLY be destroyed at a sequence point.
1. [Functions](WNScript.asciidoc#CodeConstructs-Functions) that are members of
   classes may only access varaible that have the `@functional` tag. These
   variables represent the value of the varaible at the last sequence point.
1. [Events](WNScript.asciidoc#CodeConstructs-Events) that are members of
   classes may freely modify values in that class. Any values read or written
   are current.

> **Caution** describe @functional members.

### Compute Scripts

The only object type in a compute script is [Unique
Ownership](#ObjectTypes-UniqueOwnership). All other types are invalid.
