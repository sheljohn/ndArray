MexArray
========

_Ongoing redaction, will be finished shortly._

MexArray is a single-header template C++11 library that defines a **generic n-dimensional array class** capable of handling both Matlab's `mxArray`s (mutable or not) and standard C++ allocations (both static and dynamic). It is designed to provide minimal (but useful) functionality and can be easily extended/built-on for any kind of application. It is released in the public domain and can therefore be used for open or closed source projects, commercial or not (read licence).

---

## Documentation

The library implements a template n-dimensional array class `ndArray`, focusing mainly on **four core features**:

- Handle memory properly, either assuming ownership (ie dealing with deallocation) or acting as a passive handle on a given allocation;
- Handle both shallow and deep copies efficiently and reliably;
- Provide efficient and natural multi-dimensional accessors to the contents of the array;
- Provide detailed access to the dimensions of the array.

In the following, we describe the interface provided by this class and give simple usage examples. Note that the file `test_mexArray` contains additional examples of usage.

### The `ndArray` template class

The `ndArray` template class requires **two** template parameters:

1. `typename T`: the value-type (eg. `double`, `char`, or any user-defined/STL class). This type can be `const`-qualified to prevent underlying elements from being mutated (useful with Matlab inputs for instance, cf examples).
2. `unsigned N`: the number of dimensions of the array, any number greater than 1.

Once the dimensionality of an array (`N`) is set, it cannot be modified, so you'll have to declare several arrays to deal with 2D and 3D matrices for example. The members of `ndArray<T,N>` are:
- `unsigned m_numel`: the number of elements stored in the array;
- `unsigned m_size[N]`: the dimensions of the array (similar to matlab `size()` function);
- `unsigned m_strides[N]`: the index-offsets in each dimension, typcially `[1 cumprod(size(1:N-1))]`;
- `std::shared_ptr<T>`: the actual data, stored as a plain 1D array of length `m_numel`.

Note that the indexing convention is similar to that of Matlab: the second element in `m_data` corresponds to the coordinates `(2,1,...)` (ie **row-first/column-wise**).

#### Handling memory

The [STL implementation](http://www.cplusplus.com/reference/memory/shared_ptr/) of shared pointers allows to specify a deleter. This deleter is used when the last referring instance gets destroyed in order to release the memory allocation. We provide a dummy class `no_delete<T>` that acts as a "fake-deleter"; the allocation will not be modified upon destruction of the last shared pointer, which allows to handle static allocations or externally managed memory (like Matlab's) for example.

There are **four possible ways** to assign the contents of a `ndArray`:

1. `assign( const mxArray* )`: using the method `assign` with an `mxArray` (either a Matlab input or output). The memory allocation corresponding to a Matlab matrix **will not  be managed** by the shared pointer. Note that if the `mxArray` has the wrong dimensions or wrong type, an exception will be raised.
2. `assign( pointer, const unsigned*, bool )`: using the method `assign` with a pointer (`T*`), a size (unsigned array) and a boolean flag that specifies whether or not the corresponding memory should be managed. If true, the default deleter `std::default_delete` will be passed to the shared pointer (this releases memory allocated using `new` **only**, do NOT use `malloc` variants). If false, `no_delete` will be passed instead, which will prevent from deallocating static or externally managed memory.
3. `operator= ( const ndArray<T,N>& )`: the assignment operator copies the contents of another instance of same type and same dimensions. Only a reference to the data is copied, not the data itself (**shallow copy**). The copy constructor uses this implementation.
4. `copy( const ndArray<U,N>& )`: this performs a **deep copy** of an array of same dimensions and possibly different type. A new memory allocation is requested (and therefore managed) to store the copies. Note that deep copies are not possible if the value-type (`T`) is const-qualified.

#### Accessing the data

Data access is possible in **four ways** as well:

1. `operator[] (unsigned)`: access the matrix as a 1D array, with column-wise convention;
2. `operator() ( const unsigned* )`: access the matrix with N-dimensional coordinates stored in an array (eg `{1,4,2,3,3}` for a 5D array);
3. `operator() ( std::initializer_list<unsigned> )`: idem, but with an initializer list;
4. `operator() ( unsigned, ... )`: access with N coordinate inputs (eg `M(1,4,2,3,3)`).

Note that these methods are unsafe, and might segfault if:

- The underlying array has not been assigned;
- The coordinates in methods 2 or 3 are not of length `N` (the third method will throw an exception in safe mode);
- Not enough inputs are given in method 4;
- Indexes are out of bounds in unsafe mode.

The flag `MEX_ARRAY_SAFE_ACCESS` can be commented in the sources to increase speed slightly, although this should probably not be the bottleneck of your program.

#### Accessing dimensions

There are several methods to access the dimensions of the array:

- `unsigned ndims()`: get the number of dimensions;
- `unsigned numel()`: get the number of elements in the array;
- `const unsigned* size()`: access the size array;
- `unsigned size( unsigned k )`: access the k-th dimension;
- similar methods to `size` for `strides` (index-offsets for each dimension).

### Other useful implementations

The library comes with two other implementations that might be useful to you, even though they have nothing to do with N-dimensional arrays.

#### `mx_type`

`mx_type` is a template class that associates numeric C types with the corresponding Matlab name and class ID. A typical usage is as follows: `mx_type<float>::name` will give you "single" and `mx_type<float>::id` will give you `mxSINGLE_CLASS`. This is used for example to make sure that the type of an input `mxArray` corresponds to the value-type of the `ndArray` (cf examples).

#### `singleton`

A very simple singleton construct is provided, simply use `singleton<T>::instance` at any time in your code.

---

## Examples

We provide two main examples of usage, in addition to the test file. As the library is quite small, and the code entirely commented, these examples are meant to give you a feel at what your program should look like if you decide to use it in your application, nothing more.

### Handling an existing allocation

```c++
#include "mexArray.h"
#define MASSERT( cdt, msg ) { if(!cdt) {mexErrMsgTxt(msg);return;} }

void mexFunction(	int nargout, mxArray *out[],
					int nargin, const mxArray *in[] )
{

	//////////////////////////////////////////////
	// Get a double matrix of size 3x4 in input //
	//////////////////////////////////////////////

	// Instanciate a const double matrix
	// NOTE the const type used with Matlab inputs
	ndArray<const double,2> A;

	// Get Matlab input
	MASSERT( nargin > 0, "One input expected." );
	A.assign( in[0] );

	// Make sure the dimensions are correct
	MASSERT( A.size(0) == 3 && A.size(1) == 4, "Input required to be a 3x4 matrix." );

	// ------------------------------------------------------------------------
	
	/////////////////////////////////////////////////
	// Handle a 5x20x3 matrix statically allocated //
	/////////////////////////////////////////////////
	
	// Create static allocation
	unsigned static_alloc[ 5*20*3 ];

	// Create ndArray handler
	const unsigned size[3] = { 5, 20, 3 };
	ndArray<unsigned,3> B( static_alloc, size, false );

	// Test an assignment
	B( 3,11,0 ) = 42;
}
```

### Creating a new array

```c++
#include "mexArray.h"

void mexFunction(	int nargout, mxArray *out[],
					int nargin, const mxArray *in[] )
{
	///////////////////////////////////////////////////
	// Create a 4D matrix using a dynamic allocation //
	///////////////////////////////////////////////////

	// Allocate new 4D matrix
	const unsigned size[4] = {3,4,5,6};
	const unsigned numel   = 3*4*5*6;
	ndArray<short,4> A( new short[numel], size, true );

	// Do something with it
	for ( unsigned i1 = 0; i1 < size[0]; ++i1 )
	for ( unsigned i2 = 0; i2 < size[1]; ++i2 )
	for ( unsigned i3 = 0; i3 < size[2]; ++i3 )	
	for ( unsigned i4 = 0; i4 < size[3]; ++i4 )
	{
		A( {i1,i2,i3,i4} ) = static_cast<short>( (i1 + 2*i2 + 3*i3) % (i4 + 1) );
	}

	// ------------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////
	// Create a Matlab output and copy the previous array to it //
	//////////////////////////////////////////////////////////////
	
	/*
	
	IMPORTANT NOTE: this is just an example to illustrate the fact that deep-copies on 
	arrays of SAME SIZE do not require a new allocation. We could perform the exact same 
	treatment than the previous for-loops directly on the following Matlab output...
		... and it would be much more efficient to do so!!

	*/
	
	// Crate new Matlab output
	out[0] = mxCreateNumericArray( 4, (const int*) size, mx_type<short>::id, mxREAL );

	// Assign output to new ndArray
	ndArray<short,4> B( out[0] );

	// Deep copy of A without reallocation because they have the same dimensions
	B.copy( A );
}
```
