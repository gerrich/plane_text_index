#ifndef SLICE_TOOLS_HPP
#define SLICE_TOOLS_HPP

#include "slice.hpp"

struct word_start_record_less_t {
  // compare parts of slices before space
  bool operator()(const slice_t &lhs, const slice_t &rhs) const {
    slice_t lpfx(lhs.ptr, either(lhs.find(0, '\t'), (const char*)(lhs.ptr + lhs.size)) - lhs.ptr);
    slice_t rpfx(rhs.ptr, either(rhs.find(0, '\t'), (const char*)(rhs.ptr + rhs.size)) - rhs.ptr);
//    std::cout << "less(" << lpfx << "," << rpfx << ")" << std::endl;
    return operator < (lpfx, rpfx);
  }
};

#endif // SLICE_TOOLS_HPP
