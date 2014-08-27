#ifndef PLANE_NGRAM_SUBSTRINGS_HPP
#define PLANE_NGRAM_SUBSTRINGS_HPP

#include <string>
#include "mmap.hpp"
#include "algo.hpp"
#include "slice-tools.hpp"

struct ngram_substrings_calc_t {
  ngram_substrings_calc_t(const mmap_t &_ngram_map): ngram_map(_ngram_map) {}

  void get_count(const std::string & str, size_t &count) const {
    count = 0;
    slice_t str_slice(str);
    word_start_record_less_t less;
    slice_t lb = lower_bound_line((char*)ngram_map.data, ngram_map.size, str_slice, less);
    if (lb == slice_t()) return;
    slice_t word;
    size_t cnt;
    if (2 != read_slice<slice_t, size_t, void, void>(lb, &word, &cnt, NULL, NULL )) return;
    if (word != str_slice) return;
    count = cnt;
  }
  const mmap_t &ngram_map;
};

struct ngram_substrings_provider_t : public ngram_substrings_calc_t {
  ngram_substrings_provider_t(const std::string & fname)
    :ngram_substrings_calc_t(ngram_map)
  {
    if (0 != ngram_map.load(fname.c_str())) {
      throw std::runtime_error("failed to mmap file: " + fname);
    }
  }

  ~ngram_substrings_provider_t() {
    ngram_map.free();
  }
  mmap_t ngram_map;
};

#endif

