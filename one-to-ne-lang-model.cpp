#include "bi-stat-loader.hpp"
#include "action-io.hpp"
#include "slice-tools.hpp"

#include <iostream>

// split by word boundary 
void split_string_simple(const std::string & str, std::vector<offset_slice_t> &result) {
  // do something
  return;
}

// split by word boundaries
void split_by_spaces(const std::string & str, std::vector<offset_slice_t> & slices) {
  for (size_t i = 0; i < str.size(); ++i) {
  }
}

template <typename it_t>
size_t sum_slice_len(it_t begin, it_t end) {
  size_t len = 0;
  for (it_t it = begin; it != end; ++it) {
    len += *it->size_;
  }
  return len;
}

struct one_to_one_lm_action_t {
  one_to_one_lm_action_t(
      const mmap_t &replacement_map_,
      const mmap_t &freq_map_)
    :replacement_map(replacement_map_)
    ,freq_map(freq_map_)
  {}

  // freq(w1 w2 w3) > 0 =>
  // freq(w1,w2) > 0, freq(w2,w3) >0 =>
  // freq(w1) > 0, fre1(w2) > 0, freq(w3) > 0 
  // fast search: w1 , ok ? (w1,w2  ok ? w1,w2,w3 : w2 ... ) : 
 

  template <typename action_t>
  void process_all_substrings(const std::string &str, const mmap_t &replacement_map, action_t &action) {
    static const size_t max_len = 3; // max length of ngram to find
    std::vector<offset_slice_t> slices;
    split_string_simple(str, slices);
    size_t left = 0;
    for (; left < slices.size(); ++left) {
      size_t right = left;
      for (; right < left + max_len and  right < slices.size(); ++right) {
        std::string word = std::string(str, slices[left].offset, sum_slice_len(&slices[left], &slices[right]));
        
        word_start_record_less_t less;
        slice_t lb = lower_bound_line((char*)replacement_map.data, replacement_map.size, word, less);
        slice_t ub = upper_bound_line((char*)replacement_map.data, replacement_map.size, word, less);
        
        if (lb != ub) {// not found ->next word
          slice_t fix; // TODO read fix & freq from slice
          size_t freq = 1;
          action(word, fix, freq);
        } else {
          break;
        }
      }
      ++left;
    }
  }

  void operator() (const std::string &str) const {
    std::string fix;
    // TODO actual spelling corrector 

    // split str -> find vector of slices
    // for each slice choose best replacement if any
    // compare variants using freq stats of ngram surrouding the slice

    // 1.0 
    std::vector<offset_slice_t> slices;
    split_by_spaces(str, slices);
    
    for (size_t i = 0; i < slices.size(); ++i) {
      const offset_slice_t & slice = slices[i];
      slice_t fix;
      size_t freq;
      //if (find_replacement(replacement_map, slice, fix, freq)) {
        //compare fix with original variant -> delta weight
      //}
    }

    std::cout << str << "\t" << fix << std::endl;    
  }

  const mmap_t &replacement_map;
  const mmap_t &freq_map;
};

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " <replacement-map> <freq-stat>" << std::endl;
    return 1;
  } 
  
  // load corrections
  // format:
  // $sort -rnk3 | sort --stable -k1,2
  // max 10 corrections for each string
  // <orig> <fix> <freq>
  // samples:  
  // word -> word
  // word word -> word
  // word -> word word
  mmap_t replacement_map;
  if (replacement_map.load(argv[1])) {
    std::cerr << "failed to map file" << argv[1] << std::endl;
    return 1;
  }
  
  // load substring occurence stat
  // format:
  // sort -k1,1 
  // <query> <freq>
  mmap_t freq_map;
  if (freq_map.load(argv[2])) {
    std::cerr << "failed to map file" << argv[2] << std::endl;
    return 1;
  } 
 
  one_to_one_lm_action_t action(replacement_map, freq_map);
  process_1str(std::cin, action); 

  if (replacement_map.free()) {
    std::cerr << "failed to unmap file" << argv[1] << std::endl;
    return 1;
  }
  if (freq_map.free()) {
    std::cerr << "failed to unmap file" << argv[2] << std::endl;
    return 1;
  }
  return 0;
}


