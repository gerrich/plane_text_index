#include <string>
#include "algo.hpp"

struct slice_less_t {
  bool operator ()(const slice_t &lhs, const slice_t &rhs) const {
    return lhs < rhs;
  }
};


int main() {
  std::string data;
  
  for (size_t i = 0; i < 1; ++i) data += "a\n";  
  for (size_t i = 1; i < 1000; ++i) data += "b\n";  
  for (size_t i = 0; i < 1; ++i) data += "c"; 
   
  slice_less_t less;

  {
//    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"b", 1), less);
//    std::cout << distance(&data[0], lb.ptr) << ":" << lb.size << "\t" << make_str(lb) << std::endl;

    slice_t ub = upper_bound_line(&data[0], data.size(), slice_t((char*)"b\n", 2), less);
    std::cout << distance(&data[0], ub.ptr) << ":" << ub.size << "\t" << make_str(ub) << std::endl;
  }

  return 0;
}
