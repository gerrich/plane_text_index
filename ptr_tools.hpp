#ifndef PTR_TOOLS_HPP
#define PTR_TOOLS_HPP
#include <stddef.h>
void* advance(void *ptr, size_t num) { return (void*)((char*)ptr + num); }
char* advance(char *ptr, size_t num) { return ptr + num; }
const char* advance(const char *ptr, size_t num) { return ptr + num; }

size_t distance(void *begin, void *end) { return (char*)end - (char*)begin; }
size_t distance(char *begin, char *end) { return end - begin; }
size_t distance(const char *begin, const char *end) { return end - begin; }

char *either(char *lhs, char *rhs) { return lhs ? lhs : rhs; }
const char *either(const char *lhs, const char *rhs) { return lhs ? lhs : rhs; }
void *either(void *lhs, void *rhs) { return lhs ? lhs : rhs; }
#endif
