#ifndef SLICE_READER_T_HPP
#define SLICE_READER_T_HPP 1

#include "slice.hpp"
#include "ptr_tools.hpp"

struct slice_reader_t {
  slice_reader_t(const slice_t &_data)
    : data(_data) { 
    init();
  }
  slice_reader_t(const char *str, size_t size)
    : data(const_cast<char*>(str), size) {
    init();
  }

  void init() {
    it = memchr((void*)data.ptr, '\n', data.size);
    if (!it) {
      it = advance(data.ptr, data.size);
    } else {
      it = advance(it, 1);
    }
    line.assign(data.ptr, distance(data.ptr, it));
  }

  slice_t& get() { 
    return line;
  }

  void next() {
    void *new_it = memchr(it, '\n', data.size - distance(data.ptr, it));
    if (!new_it) {
      new_it = advance(data.ptr, data.size);
    } else {
      new_it = advance(new_it, 1);
    }
    line.assign((char*)it, distance(it, new_it));
    it = new_it;
  }
  bool empty() {return it == advance(data.ptr, data.size);}
  
  slice_t line;
  const slice_t data;
  void *it;
};

#endif
