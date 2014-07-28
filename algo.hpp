#ifndef JAIL_ALGO_HPP
#define JAIL_ALGO_HPP 1

#include "slice.hpp"
#include "ptr_tools.hpp"
#include <cstring>
#include <iostream>

// return empty_slice on valid order
// firs inorder occurence overwise
template <typename less_t> 
slice_t check_order(char* data, size_t size, less_t &less) {
  char *it = data;
  char* end = advance(data, size);
  size_t offset = 0;
  static char empty_str[] = ""; 
  slice_t prev_word(empty_str, 0);
  for (;it != end ;) {
    char* new_it = (char*) memchr(it, '\n', distance(it, end));
    new_it = new_it ? advance(new_it,1) : end;   
    
    slice_t current_word(it, distance(it, new_it) - (new_it == end ? 0 : 1));
    if (less(current_word, prev_word)) {
      return current_word;
    }
    prev_word = current_word;
    it = new_it;
  }
  return slice_t();
}

void *memrchr(void* data, char ch, size_t size) {
  if(!size) return NULL;
  const char *it = (const char*)data + size - 1;
  const char *rend = (const char*)data - 1;
  for (; it != rend; --it) {
    if (ch == *it) return (void*)it;
  }
  return NULL;
}
// look for lb in sorted file
template <typename less_t> 
slice_t lower_bound_line_naive(char *data, size_t size, const slice_t &word, less_t &less)  {
  char* it = data;
  char* end = advance(data, size);
  for(;it != end ;) {
    char* new_it = (char*) memchr(it, '\n', distance(it, end));
    new_it = new_it ? advance(new_it,1) : end;
    
    slice_t current_word(it, distance(it, new_it));
    if (!less(current_word, word)) {
      return current_word;
    }
    it = new_it;
  }
  return slice_t();
}
template <typename less_t> 
slice_t lower_bound_line(char *data, size_t size, const slice_t &word, less_t &less)  {
  char *left_it = data;
  char *left_end = (char*)memchr((void*)data, '\n', size);
  left_end = (left_end == NULL) ? (char*)advance(data, size): advance(left_end, 1);
 
  if (size > 0 and data[size-1] == '\n') --size;
  char *right_it = (char*)memrchr((void*)data, '\n', size);
  right_it = (right_it == NULL) ? left_it : advance(right_it, 1);
  char *right_end = (char*)advance(data, size);

  while (left_it != right_it) {
    size_t dist = distance(left_it, right_it);
    char *middle_it = (char*)memrchr(left_it, '\n', dist / 2);
    
    if (!middle_it) {
      // no line end
      middle_it = left_it;
    } else {
      middle_it = advance(middle_it, 1);
    }
    char *middle_end = (char*)memchr(advance(left_it, dist / 2), '\n', dist - dist / 2);
    middle_end = middle_end == NULL ? right_it : advance(middle_end, 1);
    
    slice_t middle_word(middle_it, distance(middle_it, middle_end));
   
    /* 
    std::cout 
      << distance(left_it, middle_it) << " - "
      << (void*)middle_it << " " << (void*)middle_end << " " << distance(middle_it, middle_end)
      << " - " << distance(middle_end, right_end)
      <<  "["<< slice_t(left_it, distance(left_it, left_end))
      << "][" << middle_word
      << "][" << slice_t(right_it, distance(right_it, right_end))
      << "] - [" << word << "]" << std::endl;
    */
    if (less(word, middle_word)) {
      if (middle_it == right_it) return lower_bound_line_naive(left_it, distance(left_it, right_end), word, less);
      right_it = middle_it;
      right_end = middle_end;
    } else {
      if (middle_it == left_it) return lower_bound_line_naive(left_it, distance(left_it, right_end), word, less);
      left_it = middle_it;
      left_end = middle_end;
    }
  }
  return slice_t(left_it, distance(left_it, left_end));
}

#endif
