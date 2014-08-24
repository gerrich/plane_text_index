#ifndef PLANE_NGRAM_SUBSTRINGS_HPP
#define PLANE_NGRAM_SUBSTRINGS_HPP

#include <string>
#include "mmap.hpp"
#include "algo.hpp"
#include "slice-tools.hpp"

struct ngram_substrings_calc_t {
  ngram_substrings_calc_t(const mmap_t &_ngram_map): ngram_map(_ngram_map) {}

  void get_count(const std::string & str, size_t &count) const {
  
  }
  const mmap_t &ngram_map;
};

struct ngram_substrings_provider_t : public ngram_substrings_calc_t {
  ngram_substrings_provider_t(const std::string & fname)
    :ngram_substrings_calc_t(ngram_map)
  {}

  ~ngram_substrings_provider_t() {
    ngram_map.free();
  }
  mmap_t ngram_map;
};

#endif

