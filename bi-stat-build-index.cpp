#include "bi-stat-loader.hpp"
#include "../include/io/io.hpp"

#include <iostream>

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
  
  build_bi_stat_index(map);

  if (map.free()) {
    std::cerr << "failed to unmap file" << fname << std::endl;
    return 1;
  }
 

  return 0;
}
