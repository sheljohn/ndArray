MexArray
========

_Ongoing redaction, will be finished shortly._

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

The `ndArray` template class requires **two** template parameters:

1. `typename T`: the value-type (eg. `double`, `char`, or any user-defined/STL class). This type can be `const`-qualified to prevent underlying elements from being mutated (useful with Matlab inputs for instance, cf examples).
2. `unsigned N`: the number of dimensions of the array, any number greater than 1.

Once the dimension of an array is set, it cannot be modified, so you'll have to declare several arrays to handle 2D and 3D matrices for example. The members of `ndArray<T,N>` are:
- `unsigned m_numel`: the number of elements stored in the array;
- `unsigned m_size[N]`: the dimensions of the array (similar to matlab `size()` function);
- `unsigned m_strides[N]`: the index-offsets in each dimension, typcially `[1 cumprod(size(1:N-1))]`;
- `std::shared_ptr<T>`: the actual data, stored as a plain 1D array of length `m_numel`.

Note that the indexing convention is similar to that of Matlab: the second element in `m_data` corresponds to the coordinates `(2,1,...)` (ie **row-first/column-wise**).

#### Handling memory

The [STL implementation](http://www.cplusplus.com/reference/memory/shared_ptr/) of shared pointers allows to specify a deleter. This deleter is used when the last referring instance gets destroyed in order to release the memory allocation. We provide a dummy class `no_delete<T>` that acts as a "fake-deleter"; the allocation will not be modified upon destruction of the last shared pointer, which allows to handle static allocations or externally managed memory (like Matlab's) for example.

There are **four possible ways** to assign the contents of a `ndArray`:

1. `assign( const mxArray* )`: using the method `assign` with an `mxArray` (either Matlab input or output). By default, the memory allocation corresponding to the Matlab matrix is **not managed** by the shared pointer. Note that if the `mxArray` has the wrong dimensions or wrong type, an exception will be raised.
2. `assign( pointer, const unsigned*, bool )`: using the method `assign` with a pointer (`T*`), a size (unsigned array) and a boolean flag that specifies whether or not the corresponding memory should be managed. If true, the default deleter `std::default_delete` will be passed to the shared pointer (this releases memory allocated using `new` **only**). If false, `no_delete` will be passed instead, which will prevent from deallocating static or externally managed memory.
3. `operator= ( const ndArray<T,N>& )`: the assignment operator copies the contents of another instance of same type and same dimensions. Only a reference to the data is copied, not the data itself (**shallow copy**). The copy constructor uses this implementation.
4. `copy( const ndArray<U,N>& )`: this performs a **deep copy** of an array of same dimensions and possibly different type. A new memory allocation is requested (and therefore managed) to store the copies.

#### Accessing the data

Data access is possible in **four ways** as well:

1. `operator[] (unsigned)`: access the matrix as a 1D array, with column-wise convention;
2. `operator() ( const unsigned* )`: access the matrix with N-dimensional coordinates stored in an array (eg `{1,4,2,3,3}` for a 5D array);
3. `operator() ( std::initializer_list<unsigned> )`: idem, but with an initializer list;
4. `operator() ( unsigned, ... )`: access with N coordinate inputs (eg `M(1,4,2,3,3)`).

Note that these methods are unsafe, and might throw segfaults if:

- The underlying array has not been assigned;
- The coordinates in methods 2 or 3 are not of length `N` (the third method will throw an exception in safe mode);
- Not enough inputs are given in method 4;
- Indexes are out of bounds in unsafe mode.

The flag `MEX_ARRAY_SAFE_ACCESS` can be commented in the sources to increase speed slightly, although this will probably not be the bottleneck of your program.

### Other useful implementations



#### `mx_type`

#### `singleton`

---

## Examples

### Handling an existing allocation

### Creating a new array
