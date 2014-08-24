#include "gtest/gtest.h"

#include "slice.hpp"
#include "slice-tools.hpp"
#include "ptr_tools.hpp"
#include <string>

TEST(slice_test, simple) {

  EXPECT_TRUE(slice_t() == slice_t());
  
  std::string empty_str;
  EXPECT_FALSE(slice_t(empty_str) == slice_t());
  EXPECT_FALSE(slice_t() == slice_t(empty_str));

}

TEST(ptr_tools, simple) {
  char str[] = "0123456789";

  EXPECT_EQ(0, distance(&str[0], &str[0]));
  EXPECT_EQ(1, distance(&str[0], &str[1]));
  EXPECT_EQ(2, distance(&str[0], &str[2]));
  EXPECT_EQ(3, distance(&str[0], &str[3]));
  EXPECT_EQ(4, distance(&str[0], &str[4]));
  EXPECT_EQ(5, distance(&str[0], &str[5]));
  EXPECT_EQ(6, distance(&str[0], &str[6]));
  EXPECT_EQ(7, distance(&str[0], &str[7]));
  EXPECT_EQ(8, distance(&str[0], &str[8]));
  EXPECT_EQ(9, distance(&str[0], &str[9]));

  EXPECT_EQ(&str[1], advance(&str[0],1));
  EXPECT_EQ(&str[2], advance(&str[0],2));
  EXPECT_EQ(&str[4], advance(&str[0],4));
  EXPECT_EQ(&str[8], advance(&str[0],8));
}

TEST(slice_test, find_char) {

  std::string str("hello world");
  slice_t slice(str);
  EXPECT_EQ(0, distance(&str[0], slice.find(0, 'h')));
  EXPECT_EQ(1, distance(&str[0], slice.find(0, 'e')));
  EXPECT_EQ(2, distance(&str[0], slice.find(0, 'l')));
  EXPECT_EQ(4, distance(&str[0], slice.find(0, 'o')));
  EXPECT_EQ(5, distance(&str[0], slice.find(0, ' ')));
  EXPECT_EQ(6, distance(&str[0], slice.find(0, 'w')));
  EXPECT_EQ(8, distance(&str[0], slice.find(0, 'r')));
  EXPECT_EQ(10, distance(&str[0], slice.find(0, 'd')));

  EXPECT_EQ(NULL, slice.find(0, 'X'));
  


  EXPECT_EQ(NULL, find_nth_char(slice, 0, 'h', 2));
  EXPECT_EQ(0, distance(&str[0], find_nth_char(slice, 0, 'h', 1)));
  
  EXPECT_EQ(2, distance(&str[0], find_nth_char(slice, 0, 'l', 1)));
  EXPECT_EQ(3, distance(&str[0], find_nth_char(slice, 0, 'l', 2)));
  EXPECT_EQ(9, distance(&str[0], find_nth_char(slice, 0, 'l', 3)));
  EXPECT_EQ(NULL, find_nth_char(slice, 0, 'l', 4));

}
