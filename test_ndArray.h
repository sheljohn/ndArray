#ifndef __TEST_ND_ARRAY__
#define __TEST_ND_ARRAY__

//================================================
// @file         test_ndArray.h
// @author       Jonathan Hadida, Oxford DTC
// @contact      Jonathan.hadida [at] dtc.ox.ac.uk
//================================================

#include "ndArray.h"
#include <initializer_list>

using namespace nd;

/**
 * Print the contents of a 2-dimensional array.
 */
template <typename T>
void print_matrix( const ndArray<T,2>& M )
{
	const unsigned nr = M.size(0);
	const unsigned nc = M.size(1);
	printf( "Printing %u-by-%u matrix:\n", nr, nc );

	for ( unsigned r = 0; r < nr; ++r )
	{
		for ( unsigned c = 0; c < nc; ++c )
			std::cout << "\t" << M(r,c);
		printf("\n");
	}	printf("\n");
}

#endif
