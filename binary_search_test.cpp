#include <gtest/gtest.h>

#include <cstring>

#include <string>
#include <algorithm>

#include "slice.hpp"
#include "algo.hpp"
#include "bi-stat-loader.hpp"

inline bool less(const std::string& s1, const std::string& s2) {
  static std::less<std::string> _less;
  return _less.operator()(s1,s2);
}

struct slice_less_t {
  bool operator ()(const slice_t &lhs, const slice_t &rhs) const {
    return lhs < rhs;
  }
};

TEST(slice_cmp, qe_ne) {
  slice_t null_slice;
  slice_t empty_slice((char*)"", 0);
  EXPECT_TRUE(null_slice == slice_t(NULL, 0)); 
  EXPECT_TRUE(slice_t() == slice_t(NULL, 0)); 
  EXPECT_FALSE(empty_slice == slice_t(NULL, 0)); 
  EXPECT_TRUE(empty_slice == empty_slice);

  EXPECT_TRUE(slice_t((char*)"a", 1) == slice_t((char*)"a", 1));
  EXPECT_FALSE(slice_t((char*)"b", 1) == slice_t((char*)"a", 1));
  EXPECT_TRUE(slice_t((char*)"a", 1) != slice_t((char*)"b", 1));
  EXPECT_FALSE(slice_t((char*)"b", 1) != slice_t((char*)"b", 1));
}

TEST(slice_cmp, le_gt) {
  slice_t null_slice;
  slice_t empty_slice((char*)"", 0);

  EXPECT_TRUE(slice_t((char*)"", 0) < slice_t((char*)"a", 1));

  EXPECT_TRUE(slice_t((char*)"a", 1) <= slice_t((char*)"a", 1));
  EXPECT_TRUE(slice_t((char*)"a", 1) <= slice_t((char*)"aa", 2));
  EXPECT_TRUE(slice_t((char*)"a", 1) < slice_t((char*)"aa", 2));
  
  EXPECT_TRUE(slice_t((char*)"aa", 2) >= slice_t((char*)"aa", 2));
  EXPECT_TRUE(slice_t((char*)"aa", 2) >= slice_t((char*)"a", 1));
  EXPECT_TRUE(slice_t((char*)"aa", 2) > slice_t((char*)"a", 1));
}

TEST(slice_map, check_order) {
  std::string data("aa\nab\nab\nab\naba\naba\nabb\nbb\nbb");
  slice_less_t less;
  slice_t disorder_slice = check_order<slice_less_t>(&data[0], data.size(), less);
  if ( disorder_slice != slice_t()) {
    std::cout << "disorder_slice: " << make_str(disorder_slice) << std::endl;
  }
  EXPECT_TRUE(slice_t() == disorder_slice);

  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"a", 1), less);
    std::cout << distance(&data[0], lb.ptr) << ":" << lb.size << "\t" << make_str(lb) << std::endl;
    EXPECT_TRUE(lb == slice_t(&data[0], 3));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"aa", 2), less);
    EXPECT_TRUE(lb == slice_t(&data[0], 3));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"aaa", 3), less);
    EXPECT_TRUE(lb == slice_t(&data[3], 3));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"ab", 2), less);
    EXPECT_TRUE(lb == slice_t(&data[3], 3));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"ab_", 3), less);
    std::cout << distance(&data[0], lb.ptr) << ":" << lb.size << "\t" << make_str(lb) << std::endl;
    EXPECT_TRUE(lb == slice_t(&data[12], 4));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"aba", 3), less);
    std::cout << distance(&data[0], lb.ptr) << ":" << lb.size << "\t" << make_str(lb) << std::endl;
    EXPECT_TRUE(lb == slice_t(&data[12], 4));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"abb", 3), less);
    std::cout << distance(&data[0], lb.ptr) << ":" << lb.size << "\t" << make_str(lb) << std::endl;
    EXPECT_TRUE(lb == slice_t(&data[20], 4));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"b", 1), less);
    EXPECT_TRUE(lb == slice_t(&data[24], 3));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"bb", 2), less);
    EXPECT_TRUE(lb == slice_t(&data[24], 3));
  }
  {
    slice_t lb = lower_bound_line(&data[0], data.size(), slice_t((char*)"bbb", 3), less);
    EXPECT_TRUE(lb == slice_t());
  }
}
