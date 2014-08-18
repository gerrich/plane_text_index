#ifndef SLICE_TOOLS_HPP
#define SLICE_TOOLS_HPP

#include <cstring>
#include <typeinfo>

#include "slice.hpp"
#include "ptr_tools.hpp"

inline const char * find_nth_char(const slice_t & slice, size_t offset, char ch, size_t n) {
  const char * pos = NULL;
  for(size_t i = 0; i < n; ++i) {
    const char *pos = slice.find(offset, ch);
    if (pos == NULL) return NULL;
    offset = distance(slice.ptr, pos) + 1;
  }
  return pos;
}

struct word_start_record_less_t {
  word_start_record_less_t(size_t _key_width = 1) :key_width(_key_width) {}
  // compare parts of slices before space
  bool operator()(const slice_t &lhs, const slice_t &rhs) const {
    slice_t lpfx(lhs.ptr, either(find_nth_char(lhs, 0, '\t', key_width), (const char*)(lhs.ptr + lhs.size)) - lhs.ptr);
    slice_t rpfx(rhs.ptr, either(find_nth_char(rhs, 0, '\t', key_width), (const char*)(rhs.ptr + rhs.size)) - rhs.ptr);
//    std::cout << "less(" << lpfx << "," << rpfx << ")" << std::endl;
    return operator < (lpfx, rpfx);
  }
  size_t key_width;
};

inline slice_t next_slice(slice_t slice, char* end) {
  char *it = advance(slice.ptr, slice.size + 1);
  if (it == end) return slice_t();
  char *next_it = (char*)memchr((void*)it, '\n', distance(it, end));
  if (!next_it) return slice_t(it, distance(it, end));
  return slice_t(it, distance(it, next_it));
}

template <class t>
struct slice_data_reader_t{ }; 

template <>
struct slice_data_reader_t<void> {
  bool get(const slice_t &src, void * ) const {
    return true;
  }
};

template <>
struct slice_data_reader_t<slice_t> {
  bool get(const slice_t &src, slice_t * dst) const {
    *dst = src;
    return true;
  }
};

template <>
struct slice_data_reader_t<std::string> {
  bool get(const slice_t &src, std::string * dst) const {
    *dst = make_str(src);
    return true;
  }
};

template <>
struct slice_data_reader_t<size_t> {
  bool get(const slice_t &src, size_t *dst) const {
    *dst = 0;
    for (size_t i = 0; i < src.size; ++i) {
      if (src.ptr[i] >= '0' && src.ptr[i] <= '9') break;
    }
    for (size_t i = 0; i < src.size; ++i) {
      if (src.ptr[i] < '0' || src.ptr[i] > '9') break;
      *dst *= 10;
      *dst += src.ptr[i] - '0';
    }
    return true;
  }
};

template <typename t1, typename t2, typename t3, typename t4>
struct read_slice_t{
  size_t _do(char *it, char *end, t1 *v1, t2 *v2, t3 *v3, t4 *v4) const {
    char *tab_pos = (char*)memchr((void*)it, '\t', distance(it, end));
    slice_t field(it, distance(it, tab_pos ? tab_pos : end));
    // parse field
    slice_data_reader_t<t1> data_reader;
    if((v1==NULL && typeid(void) != typeid(t1)) || !data_reader.get(field, v1)) return 0;
    if (tab_pos == NULL) return 1;
    return 1 + read_slice_t<t2,t3,t4,void>()._do(advance(tab_pos, 1), end, v2, v3, v4, NULL);
  }
};

template <typename t1, typename t2, typename t3, typename t4>
size_t read_slice(slice_t src, t1 *v1, t2 *v2, t3 *v3, t4 *v4) {
  return read_slice_t<t1,t2,t3,t4>()._do(src.ptr, src.ptr + src.size, v1,v2,v3,v4);
}
#endif // SLICE_TOOLS_HPP
