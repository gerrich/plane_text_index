#ifndef SLICE_T_HPP
#define SLICE_T_HPP 1

#include <cstring>
#include <string>
#include <ostream>

struct offset_slice_t{
  offset_slice_t() :offset(0), size(0) {}

  size_t offset;
  size_t size;
};

struct slice_t {
  slice_t() :ptr(NULL), size(0) {}
  slice_t(char *_ptr, size_t _size) :ptr(_ptr), size(_size) {}
  
  bool operator == (const slice_t &rhs) const { return size == rhs.size && 0 == memcmp((void*)ptr, (void*)rhs.ptr, size); }
  bool operator != (const slice_t &rhs) const { return !operator==(rhs);}
  void assign(char *_ptr, size_t _size) {ptr = _ptr; size = _size;}
  bool empty() const { return size == 0; }
  
  const char& operator[](size_t index) const { return ptr[index]; }
  char& operator[](size_t index) { return ptr[index]; }
  
  const char* begin() const { return ptr; } 
  char* begin() { return ptr; } 
  const char* end() const { return ptr + size; } 
  char* end() { return ptr + size; }

  char *find(size_t offset, char ch) {
    return (char*)memchr((void*)(&ptr[offset]), ch, size - offset); 
  }
  const char *find(size_t offset, char ch) const {
    return (char*)memchr((void*)(&ptr[offset]), ch, size - offset); 
  }

  char *ptr;
  size_t size;
};

std::ostream &operator << (std::ostream &os, const slice_t &slice) {
  if (slice.size > 0) os.write(slice.ptr, slice.size);
  return os;
}

std::string make_str(const slice_t &slice) {
  return std::string(slice.ptr, slice.size);
}

int slicecmp(const slice_t &lhs, const slice_t &rhs) {
  if (lhs.size < rhs.size) {
    int cmp_res = memcmp((void*)lhs.ptr, (void*)rhs.ptr, lhs.size);
    return cmp_res == 0 ? lhs.size : cmp_res;
  } else {
    int cmp_res = memcmp((void*)lhs.ptr, (void*)rhs.ptr, rhs.size);
    if (cmp_res == 0) return (lhs.size == rhs.size) ? 0 : -rhs.size;
    return cmp_res;
  }
}

bool operator < (const slice_t &lhs, const slice_t &rhs) { return slicecmp(lhs, rhs) < 0; }
bool operator <= (const slice_t &lhs, const slice_t &rhs) { return slicecmp(lhs, rhs) <= 0; }
bool operator > (const slice_t &lhs, const slice_t &rhs) { return slicecmp(lhs, rhs) > 0; }
bool operator >= (const slice_t &lhs, const slice_t &rhs) { return slicecmp(lhs, rhs) >= 0; }

#endif
