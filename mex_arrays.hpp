


        /********************     **********     ********************/
        /********************     **********     ********************/



/**
 * 
 */
template <typename T, unsigned N>
ndArray<T,N>::ndArray( const self& other )
{
	if ( other.m_data != m_data )
	{
		// Clear current instance first
		clear();

		// Copy data from other
		m_data  = other.m_data;
		m_numel = other.m_numel;
		std::copy_n( other.m_size, N, m_size );
		std::copy_n( other.m_strides, N, m_strides );
	}
}

// ------------------------------------------------------------------------

/**
 * typename std::enable_if< 
	self::is_mutable && std::is_assignable<T,U>::value, void >::type
 */
template <typename T, unsigned N>
template <typename U>
void ndArray<T,N>::copy( const ndArray<U,N>& other )
{
	if ( self::is_mutable )
	{
		// Create new allocation
		assign( new T[ other.m_numel ], other.m_size, true );

		// Copy data
		auto dst = begin(); auto src = other.begin();
		for ( ;src != other.end(); ++src, ++dst ) *dst = (T) *src;
	}
}

// ------------------------------------------------------------------------

/**
 * 
 */
template <typename T, unsigned N>
void ndArray<T,N>::clear()
{
	m_data.reset();
}

// ------------------------------------------------------------------------

/**
 * 
 */
template <typename T, unsigned N>
void ndArray<T,N>::reset()
{
	clear();
	m_numel = 0;
	std::fill_n( m_size, N, 0 );
	std::fill_n( m_strides, N, 0 );
}

// ------------------------------------------------------------------------

/**
 * 
 */
template <typename T, unsigned N>
void ndArray<T,N>::assign_shared( pointer ptr, bool manage )
{
	if (manage)
		m_data.reset( ptr );
	else
		m_data.reset( ptr, no_delete<T>() );
}

// ------------------------------------------------------------------------

/**
 * 
 */
template <typename T, unsigned N>
void ndArray<T,N>::assign( const mxArray *A )
{
	// Check dimensions and type
	if ( ((unsigned) mxGetNumberOfDimensions(A)) != N )
		throw new std::domain_error("Bad dimensions.");
	if ( mxGetClassID(A) != mx_type<T>::id )
		throw new std::invalid_argument("Type mismatch.");	

	// Get input dimensions
	const unsigned *size = (const unsigned*) mxGetDimensions(A);

	// Call assign variant
	assign( (pointer) mxGetData(A), size, false );
}

// ------------------------------------------------------------------------


/**
 * 
 */
template <typename T, unsigned N>
void ndArray<T,N>::assign( pointer ptr, const unsigned *size, bool manage )
{
	if ( ptr != data() )
	{
		// Clear contents
		clear();

		// Compute internal dimensions
		m_numel = 1; 
		for ( unsigned i = 0; i < N; ++i )
		{
			m_size[i] = size[i];
			m_numel  *= size[i];
			m_strides[ (i+1) % N ] = m_numel;
		}	m_strides[0] = 1;

		// Set data
		assign_shared( ptr, manage );
	}
}

// ------------------------------------------------------------------------

/**
 * 
 */
template <typename T, unsigned N>
void ndArray<T,N>::info() const
{
	if ( m_data )
	{
		printf("%u-dimensional array of size (%u", N, m_size[0]);
		for ( unsigned d = 1; d < N; ++d )
			printf(", %u", m_size[d]);
		printf(") = %u elements.\n", m_numel);
	}
	else
		printf("Empty %u-dimensional array.\n", N);
}



        /********************     **********     ********************/
        /********************     **********     ********************/


