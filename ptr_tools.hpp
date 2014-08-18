#ifndef PTR_TOOLS_HPP
#define PTR_TOOLS_HPP
#include <stddef.h>
inline void* advance(void *ptr, size_t num) { return (void*)((char*)ptr + num); }
inline char* advance(char *ptr, size_t num) { return ptr + num; }
inline const char* advance(const char *ptr, size_t num) { return ptr + num; }

inline size_t distance(void *begin, void *end) { return (char*)end - (char*)begin; }
inline size_t distance(char *begin, char *end) { return end - begin; }
inline size_t distance(const char *begin, const char *end) { return end - begin; }

inline char *either(char *lhs, char *rhs) { return lhs ? lhs : rhs; }
inline const char *either(const char *lhs, const char *rhs) { return lhs ? lhs : rhs; }
inline void *either(void *lhs, void *rhs) { return lhs ? lhs : rhs; }
#endif
