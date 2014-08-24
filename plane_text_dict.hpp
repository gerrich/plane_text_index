#ifndef PLANE_TEXT_DICT_HPP
#define PLANE_TEXT_DICT_HPP

#include <vector>

#include "mmap.hpp"
#include "slice-tools.hpp"

struct variant_t {
  slice_t word;
  double error_weight; //log(cnt/fix_sum_cnt)  
};
typedef std::vector<variant_t> variant_list_t;

// Load sorted file <word>\t<fix>\t<cnt>\t<agg_fix_cnt>
//
// for word get list of pairs <fix, log(cnt/agg_fix_cnt)
struct plane_text_dict_t {

  plane_text_dict_t(char *data, size_t sz)
    :data_(data)
    ,sz_(sz)
  {}

  size_t get_fix_list(const slice_t& word, variant_list_t& variants) const {
    word_start_record_less_t less;
    slice_t lb = lower_bound_line(data_, sz_, word, less);
    slice_t ub = upper_bound_line(data_, sz_, word, less);
   
    std::cout << distance(data_, lb.ptr) << "+(" << lb.size << ") -> " << distance(data_, ub.ptr) << "+(" << ub.size << ")" << std::endl;
    if (lb == ub) return 0;
//    std::cout << "[[[" << std::string(lb.ptr, distance(lb.ptr, ub.ptr)) << "]]]" << std::endl;

    size_t variants_count = 0;
    for (slice_t fix = lb; fix != ub; fix = next_slice(fix, data_ + sz_), ++variants_count) {
      variants.push_back(variant_t());
      std::cout << "VARIANT_STR: (" << make_str(fix) << ")" << std::endl;
      size_t cnt = 0;
      size_t fix_cnt = 0;
      if (4 != read_slice<void, slice_t, size_t, size_t>(fix, NULL, &variants.back().word, &cnt, &fix_cnt)) continue;
      if ( cnt == 0 || fix_cnt == 0) continue;
      variants.back().error_weight = log(double(cnt)) - log(double(fix_cnt));
    }
    return variants_count;
  } 

  char * data_;
  size_t sz_;
};

struct plane_text_dict_mmap_t : public plane_text_dict_t {
  plane_text_dict_mmap_t(const char* fname)
    :plane_text_dict_t(NULL, 0)
  {
    mmap.load(fname);
    data_ = (char*)mmap.data; 
    sz_ = mmap.size;
  }
  ~plane_text_dict_mmap_t() {
    mmap.free();
  }
  mmap_t mmap;
};

#endif // PLANE_TEXT_DICT_HPP
