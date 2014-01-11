#include "test_mexArray.h"

void mexFunction(	int nargout, mxArray *out[],
					int nargin, const mxArray *in[] )
{
	// Declare local variables
	unsigned nr, nc, ar, ac;
	int bsize[3];

	ndArray<const double,2> A;
	ndArray<float,3> B;

	// Expect a double matrix in input
	if ( nargin > 0 )
	{
		// Manage input matrix without copy
		A.assign( in[0] );

		// Set matrix dimensions
		nr = A.size(0);
		nc = A.size(1);

		// Print info about dimensions of input
		printf("\n-----\nDimensions of input: ");
		A.info(); 

		// Print the input matrix
		print_matrix(A);

		// If an output is passed, generate a matrix that's half the size 
		// in the largest dimension
		if ( nargout > 0 )
		{
			printf("Splitting input matrix in two parts, ");

			// Set b's dimensions
			bsize[2] = 2;
			if ( nr > nc )
			{
				printf("breaking rows.\n");
				bsize[0] = nr / 2; ar = bsize[0];
				bsize[1] = nc; ac = 0;
			}
			else
			{
				printf("breaking columns.\n");
				bsize[0] = nr; ar = 0;
				bsize[1] = nc / 2; ac = bsize[1];
			}

			// Create output
			out[0] = mxCreateNumericArray(3, bsize, mx_type<float>::id, mxREAL);

			// Handle without copy with B
			B.assign( out[0] );
			printf("Output dimensions are: ");
			B.info();

			// Handle the previous allocation without copy
			for ( int r = 0; r < bsize[0]; ++r )
			for ( int c = 0; c < bsize[1]; ++c )
			{
				B(r,c,0) = (float) A(r,c);
				B(r,c,1) = (float) A(r+ar,c+ac);
			}

			printf("-----\n\n");
		}
	}
}