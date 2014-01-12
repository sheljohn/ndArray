MexArray
========

MexArray is a single-header template C++11 library that defines a **generic n-dimensional array class** capable to handle both Matlab's `mxArray`s (mutable or not) and standard C++ allocations (both static and dynamic). It is designed to provide minimal (but useful) functionality and can be easily extended/built-on for any kind of application. It is released in the public domain and can therefore be used for open or closed source projects, commercial or not (read licence).

---

## Documentation

The library mainly implements a template n-dimensional array class `ndArray`. The implementation focuses mainly on four core features:

- Handle memory allocations properly, either assuming ownership (ie handling deallocation) or acting as a passive handle on a given allocation.
- Handle assignments, shallow and deep copies efficiently, intuitively and reliably (using shared pointers/reference counting).
- Provide efficient and natural multi-dimensional accessors to the contents of the array.
- Provide detailed access to the dimensions of the array.

In the following, we describe the interface provided by this class and provide simple usage examples. Note that the file `test_mexArray` provides additional examples of usage.

### The `ndArray` template class

The `ndArray` template class defines **two** template parameters:

1. `typename T`: the value-type (eg. `double`, `char`, ).

#### Handling memory

#### Copies and assignments

#### Accessing the data

### Other useful implementations

#### `mx_type`

#### `singleton`

---

## Examples

### Handling an existing allocation

### Creating a new array
