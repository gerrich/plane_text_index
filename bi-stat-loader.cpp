#include "bi-stat-loader.hpp"
#include "../include/io/io.hpp"

#include <iostream>

struct bi_stat_action_t {
  bi_stat_action_t(const bi_stat_t &_bi_stat) :bi_stat(_bi_stat) {}
  void operator() (const std::string &w1, const std::string &w2) const {
    const slice_t &stat1 = bi_stat.get_stat(w1); 
    const slice_t &stat2 = bi_stat.get_stat(w2);

    double score = calc_similarity_score_slow(stat1, stat2);
    std::cout << "score: " << score << std::endl;
  }
  const bi_stat_t &bi_stat;
};

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " <filename>" << std::endl;
    return 1;
  } 

  const char *fname = argv[1];

  mmap_t map;
  if (map.load(fname)) {
    std::cerr << "failed to map file" << fname << std::endl;
    return 1;
  }
  
  //size_t lcount = calc_lines(map.data, map.size);
  //std::cout << "lcount: " << lcount << std::endl;

  bi_stat_t bi_stat;
  bi_stat.init(map);
  
  bi_stat_action_t action(bi_stat);
  process2fields(std::cin, std::cout, std::cerr, action); 

  if (map.free()) {
    std::cerr << "failed to unmap file" << fname << std::endl;
    return 1;
  }
 

  return 0;
}
