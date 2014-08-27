#ifndef VECTOR_TOOLS_HPP
#define VECTOR_TOOLS_HPP
// copied from http://stackoverflow.com/questions/838384/reorder-vector-using-a-vector-of-indices

template< typename order_iterator, typename value_iterator >
void reorder( order_iterator order_begin, order_iterator order_end, value_iterator v ) {   
  typedef typename iterator_traits< order_iterator >::difference_type diff_t;

  diff_t remaining = order_end - 1 - order_begin;
  for ( index_t s = index_t(), d; remaining > 0; ++ s ) {
    for ( d = order_begin[s]; d > s; d = order_begin[d] ) ;
    if ( d == s ) {
      -- remaining;
      value_t temp = v[s];
      while ( d = order_begin[d], d != s ) {
        swap( temp, v[d] );
        -- remaining;
      }
      v[s] = temp;
    }
  }
}

#endif 
