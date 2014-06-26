#ifndef PTR_TOOLS_HPP
#define PTR_TOOLS_HPP
#include <stddef.h>
void * advance(void *ptr, size_t num) {
  return (void*)((char*)ptr + num);
}

size_t distance(void *begin, void *end) {
  return (char*)end - (char*)begin;
}
#endif
