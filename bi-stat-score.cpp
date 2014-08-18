#include "bi-stat-loader.hpp"
#include "action-io.hpp"

#include <iostream>

struct bi_stat_action_t {
  bi_stat_action_t(const mmap_t &_stat_map, const mmap_t &_index_map)
    :stat_map(_stat_map)
    ,index_map(_index_map) {}

  void operator() (const std::string &w1, const std::string &w2) const {
    slice_t stat1 = get_stat(stat_map, index_map, w1); 
    slice_t stat2 = get_stat(stat_map, index_map, w2);
  
    //std::cout << "stat1: " << stat1 << std::endl;
    //std::cout << "stat2: " << stat2 << std::endl;

    double score = calc_similarity_score_slow(stat1, stat2);
    std::cout << w1 << "\t" << w2 << "\t" << score << std::endl;
  }
  const mmap_t &stat_map;
  const mmap_t &index_map;
};

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " <bi-stat> <bi-index>" << std::endl;
    return 1;
  } 

  mmap_t stat_map;
  if (stat_map.load(argv[1])) {
    std::cerr << "failed to map file" << argv[1] << std::endl;
    return 1;
  }
  
  mmap_t index_map;
  if (index_map.load(argv[2])) {
    std::cerr << "failed to map file" << argv[2] << std::endl;
    return 1;
  }
  //size_t lcount = calc_lines(map.data, map.size);
  //std::cout << "lcount: " << lcount << std::endl;

  bi_stat_action_t action(stat_map, index_map);
  process_2str(std::cin, action); 

  if (stat_map.free()) {
    std::cerr << "failed to unmap file" << argv[1] << std::endl;
    return 1;
  }
  if (index_map.free()) {
    std::cerr << "failed to unmap file" << argv[2] << std::endl;
    return 1;
  }
 

  return 0;
}
