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

template <typename _size_type>
struct base_slice_t {
  typedef _size_type size_type;

  base_slice_t() :ptr(NULL), size(0) {}
  base_slice_t(char *_ptr, _size_type _size) :ptr(_ptr), size(_size) {}
  base_slice_t(const std::string &s) :ptr(const_cast<char*>(s.c_str())), size(s.size()) {}

  bool operator == (const base_slice_t &rhs) const {
    return size == rhs.size &&
      (ptr != NULL && rhs.ptr != NULL) ?
        (0 == memcmp((void*)ptr, (void*)rhs.ptr, size)) :
        ((ptr == NULL) && (rhs.ptr == NULL));
  }
  bool operator != (const base_slice_t &rhs) const { return !operator==(rhs);}
  void assign(char *_ptr, _size_type _size) {ptr = _ptr; size = _size;}
  bool empty() const { return size == 0; }
  
  const char& operator[](_size_type index) const { return ptr[index]; }
  char& operator[](_size_type index) { return ptr[index]; }
  
  const char* begin() const { return ptr; } 
  char* begin() { return ptr; } 
  const char* end() const { return ptr + size; } 
  char* end() { return ptr + size; }

  char *find(_size_type offset, char ch) {
    return (char*)memchr((void*)(&ptr[offset]), ch, size - offset); 
  }
  const char *find(_size_type offset, char ch) const {
    return (char*)memchr((void*)(&ptr[offset]), ch, size - offset); 
  }

  char *ptr;
  _size_type size;
};

template <typename _size_type>
std::ostream &operator << (std::ostream &os, const base_slice_t<_size_type> &slice) {
  if (slice.size > 0) os.write(slice.ptr, slice.size);
  return os;
}

template <typename _size_type>
std::string make_str(const base_slice_t<_size_type> &slice) {
  return std::string(slice.ptr, slice.size);
}

template <typename _size_type>
int slicecmp(const base_slice_t<_size_type> &lhs, const base_slice_t<_size_type> &rhs) {
  if (lhs.size < rhs.size) {
    int cmp_res = memcmp((void*)lhs.ptr, (void*)rhs.ptr, lhs.size);
    return cmp_res == 0 ? -int(rhs.size) : cmp_res;
  } else {
    int cmp_res = memcmp((void*)lhs.ptr, (void*)rhs.ptr, rhs.size);
    if (cmp_res == 0) return (lhs.size == rhs.size) ? 0 : int(lhs.size);
    return cmp_res;
  }
}

template <typename _size_type>
bool operator < (const base_slice_t<_size_type> &lhs, const base_slice_t<_size_type> &rhs) { return slicecmp(lhs, rhs) < 0; }
template <typename _size_type>
bool operator <= (const base_slice_t<_size_type> &lhs, const base_slice_t<_size_type> &rhs) { return slicecmp(lhs, rhs) <= 0; }
template <typename _size_type>
bool operator > (const base_slice_t<_size_type> &lhs, const base_slice_t<_size_type> &rhs) { return slicecmp(lhs, rhs) > 0; }
template <typename _size_type>
bool operator >= (const base_slice_t<_size_type> &lhs, const base_slice_t<_size_type> &rhs) { return slicecmp(lhs, rhs) >= 0; }

typedef base_slice_t<size_t> slice_t;

#endif
