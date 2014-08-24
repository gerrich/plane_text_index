#include "bi-stat-loader.hpp"
#include "action-io.hpp"

#include <iostream>

struct bi_stat_score_calc_t {
  bi_stat_score_calc_t(const mmap_t &_stat_map, const mmap_t &_index_map)
    :stat_map(_stat_map)
    ,index_map(_index_map) {}

  double calc_score(const std::string &w1, const std::string &w2) const {
    slice_t stat1 = get_stat(stat_map, index_map, w1); 
    slice_t stat2 = get_stat(stat_map, index_map, w2);
  
    return calc_similarity_score_slow(stat1, stat2);
  }

  const mmap_t &stat_map;
  const mmap_t &index_map;
};

struct bi_stat_score_provider_t : public bi_stat_score_calc_t {
  bi_stat_score_provider_t(const std::string &fname) 
      :bi_stat_score_calc_t(stat_map, index_map) {
    stat_map.load(fname.c_str());
    std::string index_name = fname + ".index";
    index_map.load(index_name.c_str());
  }
  ~bi_stat_score_provider_t() {
    stat_map.free();
    index_map.free();  
  }
  mmap_t stat_map;
  mmap_t index_map;
};

