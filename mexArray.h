#ifndef __MEX_ARRAY__
#define __MEX_ARRAY__

//================================================
// @file         mexArray.h
// @author       Jonathan Hadida, Oxford DTC
// @contact      Jonathan.hadida [at] dtc.ox.ac.uk
//================================================

#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

#include <exception>
#include <stdexcept>

#include <memory>
#include <algorithm>
#include <type_traits>
#include <initializer_list>

#include "mex.h"

// Comment this to gain a tiny bit of speed
#define MEX_ARRAY_SAFE_ACCESS

// Protect 1D access
#ifdef MEX_ARRAY_SAFE_ACCESS
#define MEX_ARRAY_PROTECT(k,n) (k % n)
#else
#define MEX_ARRAY_PROTECT(k,n) k
#endif



        /********************     **********     ********************/
        /********************     **********     ********************/



/**
 * Convert numeric types to mex types.
 */
template <typename T>
struct mx_type
{
	static const char *name;
	static const mxClassID id;
};

// ------------------------------------------------------------------------

template <> const char* mx_type<char>::name = "int8";
template <> const char* mx_type<unsigned char>::name = "uint8";
template <> const char* mx_type<short>::name = "int16";
template <> const char* mx_type<unsigned short>::name = "uint16";
template <> const char* mx_type<int>::name = "int32";
template <> const char* mx_type<unsigned int>::name = "uint32";
template <> const char* mx_type<float>::name = "single";
template <> const char* mx_type<double>::name = "double";

template <> const mxClassID mx_type<char>::id = mxINT8_CLASS;
template <> const mxClassID mx_type<unsigned char>::id = mxUINT8_CLASS;
template <> const mxClassID mx_type<short>::id = mxINT16_CLASS;
template <> const mxClassID mx_type<unsigned short>::id = mxUINT16_CLASS;
template <> const mxClassID mx_type<int>::id = mxINT32_CLASS;
template <> const mxClassID mx_type<unsigned int>::id = mxUINT32_CLASS;
template <> const mxClassID mx_type<float>::id = mxSINGLE_CLASS;
template <> const mxClassID mx_type<double>::id = mxDOUBLE_CLASS;

// ------------------------------------------------------------------------

template <> const char* mx_type<const char>::name = "int8";
template <> const char* mx_type<const unsigned char>::name = "uint8";
template <> const char* mx_type<const short>::name = "int16";
template <> const char* mx_type<const unsigned short>::name = "uint16";
template <> const char* mx_type<const int>::name = "int32";
template <> const char* mx_type<const unsigned int>::name = "uint32";
template <> const char* mx_type<const float>::name = "single";
template <> const char* mx_type<const double>::name = "double";

template <> const mxClassID mx_type<const char>::id = mxINT8_CLASS;
template <> const mxClassID mx_type<const unsigned char>::id = mxUINT8_CLASS;
template <> const mxClassID mx_type<const short>::id = mxINT16_CLASS;
template <> const mxClassID mx_type<const unsigned short>::id = mxUINT16_CLASS;
template <> const mxClassID mx_type<const int>::id = mxINT32_CLASS;
template <> const mxClassID mx_type<const unsigned int>::id = mxUINT32_CLASS;
template <> const mxClassID mx_type<const float>::id = mxSINGLE_CLASS;
template <> const mxClassID mx_type<const double>::id = mxDOUBLE_CLASS;



        /********************     **********     ********************/
        /********************     **********     ********************/



/**
 * Convert nd coordinates to 1d index.
 * Two main variants are provided:
 * - Taking an ARRAY as coordinates (size input by template)
 * - Taking a VA_LIST as a list of coordinate inputs (cf 
 * operator() below).
 *
 * A few template specializations are provided as well for 
 * low dimensions (increased speek). But any modern compiler should 
 * be trusted to optimize the for-loops for low N anyway (eg loop 
 * unrolling for low statically defined number of cycles).
 */
template <unsigned N>
unsigned sub2ind( const unsigned *subs, const unsigned *size, const unsigned *strides )
{
	register unsigned ind = 0;

	for (unsigned i = 0; i < N; ++i) 
		ind += MEX_ARRAY_PROTECT(subs[i],size[i]) * strides[i];

	return ind;
}

template <unsigned N>
unsigned sub2ind( va_list& vl, const unsigned *size, const unsigned *strides )
{
	register unsigned ind = 0;

	for (unsigned i = 1; i < N; ++i) 
		ind += MEX_ARRAY_PROTECT(va_arg(vl,unsigned),size[i]) * strides[i];

	va_end(vl); return ind;
}

template <> inline unsigned 
sub2ind<0>( const unsigned*, const unsigned*, const unsigned* )
	{ return 0; }
template <> inline unsigned 
sub2ind<1>( const unsigned *subs, const unsigned*, const unsigned* )
	{ return *subs; }
template <> inline unsigned 
sub2ind<2>( const unsigned *subs, const unsigned *size, const unsigned* )
	{ return (subs[0] + subs[1]*size[0]); }

// ------------------------------------------------------------------------

/**
 * Simple singleton.
 */
template <typename T> struct singleton { static T instance; };
template <typename T> T singleton<T>::instance = T();

// ------------------------------------------------------------------------

/**
 * (obsolete)
 * Create fake array to protect against empty shared pointer.
 */
template <typename T>
struct fake_array
{
	inline T& operator[] (unsigned n) const { return singleton<T>::instance; }
};

// ------------------------------------------------------------------------

/**
 * Dummy deleter functor.
 * This litterally does nothing to the input pointer; it can be used 
 * safely with shared pointers for either statically allocated memory 
 * (eg fixed-size arrays) or externally managed memory (eg Matlab in/out).
 */
template <typename T>
struct no_delete
{ inline void operator() ( T* ptr ) const {} };



        /********************     **********     ********************/
        /********************     **********     ********************/



/**
 * n-dimensional array.
 * NOTE: T can be CONST (underlying elements non-assignable: 
 * suitable for Matlab inputs for instance), or NON-CONST
 * (underlying elements assignable, suitable for Matlab 
 * outputs or dynamically allocated memory for instance).
 */
template <typename T, unsigned N>
class ndArray
{
public:

	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;

	typedef typename std::add_const<T>::type const_value;
	typedef const_value* const_pointer;
	typedef const_value& const_reference;

	typedef std::shared_ptr<value_type> shared;
	typedef ndArray<T,N> self;



	// Constructors
	ndArray() { reset(); }
	ndArray( const mxArray *A ) { assign(A); }
	ndArray( pointer ptr, const unsigned *size, bool manage ) { assign(ptr,size,manage); }


	// Copy constructor
	ndArray( const self& other ) { operator=(other); }
	self& operator= ( const self& other );

	// Check pointer validity
	inline bool empty() const { return !((bool) m_data); }
	inline operator bool() const { return m_data; }

	// Print array dimensions
	void info() const;



	// ------------------------------------------------------------------------
	


	// Clear contents
	void clear();
	void reset();


	// Assign either an mxArray or a pointer
	void assign( const mxArray *A );
	void assign( pointer ptr, const unsigned *size, bool manage );
	
	
	// Swap contents with another array
	void swap( self& other );


	// Copy from another array
	template <typename U>
	void copy( const ndArray<U,N>& other );



	// ------------------------------------------------------------------------



	// 1D access
	inline reference operator[] ( unsigned n ) const 
		{ return data()[ MEX_ARRAY_PROTECT(n,m_numel) ]; }

	// ND access
	reference operator() ( const unsigned *subs ) const
		{ return data()[ sub2ind<N>(subs, m_size, m_strides) ]; }

	reference operator() ( std::initializer_list<unsigned> subs ) const
		{
#ifdef MEX_ARRAY_SAFE_ACCESS
			if ( subs.size() != N ) 
				throw std::length_error("Invalid coordinates length.");
#endif
			return data()[ sub2ind<N>(subs.begin(), m_size, m_strides) ];
		}

	// Coordinates access
	reference operator() ( unsigned i, ... ) const
		{ 
			va_list vl; va_start(vl,i); 
			return data()[ i+sub2ind<N>(vl, m_size, m_strides) ]; 
		}


	// Access data directly
	inline const_pointer cdata() const { return m_data.get(); }
	inline pointer data() const { return m_data.get(); }

	// Iterators
	inline const_pointer cbegin() const { return data(); }
	inline const_pointer cend() const { return data() + m_numel; }

	inline pointer begin() const { return data(); }
	inline pointer end() const { return data() + m_numel; }



	// ------------------------------------------------------------------------



	// Dimensions
	inline const unsigned* size() const { return m_size; }
	inline unsigned size( unsigned n ) const { return m_size[ n % N ]; }
	inline const unsigned* strides() const { return m_strides; }
	inline unsigned stride( unsigned n ) const { return m_strides[ n % N ]; }
	inline unsigned numel() const { return m_numel; }
	inline unsigned ndims() const { return N; }



protected:

	void assign_shared( pointer ptr, bool manage );

	unsigned m_numel;
	unsigned m_size[N];
	unsigned m_strides[N];

	shared m_data;
};



        /********************     **********     ********************/
        /********************     **********     ********************/



// Include implementation
#include "mexArray.hpp"

#endif
