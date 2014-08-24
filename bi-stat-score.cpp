#include "bi-stat-loader.hpp"
#include "action-io.hpp"
#include "bi-stat-score-provider.hpp"
#include <iostream>

struct bi_stat_action_t {
  bi_stat_action_t(const bi_stat_score_calc_t &_score_calc)
    :score_calc(_score_calc)
  {}
  void operator() (const std::string &w1, const std::string &w2) const {
    double score = score_calc.calc_score(w1, w2);
    std::cout << w1 << "\t" << w2 << "\t" << score << std::endl;
  }
  const bi_stat_score_calc_t &score_calc;
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

  bi_stat_score_calc_t score_calc(stat_map, index_map);
  bi_stat_action_t action(score_calc);
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
