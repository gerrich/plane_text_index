#ifndef BI_STAT_LOADER_HPP
#define BI_STAT_LOADER_HPP 1

#include "ptr_tools.hpp"
#include "slice.hpp"
#include "slice-reader.hpp"
#include "slice-tools.hpp"
#include "algo.hpp"
#include "my-io.hpp"
#include "action-io.hpp"
#include "mmap.hpp"

#include <string>
#include <map>
#include <set>

#include <cstring>
#include <sys/stat.h>

#include <iostream>
#include <ostream>

#include <cstdio>
// TODO change with propriate error logger
//void perror(const char*) {}


struct line_counter_t {
  line_counter_t(): count(0) {}
  void process(void *data, size_t size) {++count;}
  void commit() {}
  size_t count;
};
size_t calc_lines(void *data, size_t size) {
  line_counter_t action;
  process_lines(data, size, action);
  return action.count;
}

typedef std::map<std::string, slice_t> slice_map_t;

struct ordered_bi_reader_t {
  ordered_bi_reader_t(const char *_data_begin)
    :chunk_begin(NULL)
    ,chunk_size(0)
    ,data_begin(_data_begin) {}

  void commit_chunk() {
    if (!chunk_begin) return;
//    slice_t chunk(chunk_begin, chunk_size);
//    slice_map.insert(std::make_pair(make_str(last_word), chunk));
    std::cout << last_word << "\t" << distance(data_begin, chunk_begin) << "\t" << chunk_size << std::endl; 
  }
  void process(void *data, size_t size) {
    void *space_ptr = memchr(data, ' ', size);
    if (!space_ptr) return;
    slice_t word((char*)data, distance(data, space_ptr));
    if (word != last_word) {
      commit_chunk();
      chunk_begin = (char*)data;
      chunk_size = 0;
      
      last_word = word;
    }
    chunk_size += size;
  }
  void commit() {commit_chunk();}

  slice_t last_word;
  const char *chunk_begin;
  size_t chunk_size; 
  const char *data_begin;
};

void build_bi_stat_index(const mmap_t &map) {
  ordered_bi_reader_t action((const char*)map.data);
  process_lines(map.data, map.size, action);
}

const slice_t get_stat(const mmap_t &map, const mmap_t &idx_map, const slice_t &word) {
  static slice_t empty_slice;
  word_start_record_less_t less;
  slice_t lb = lower_bound_line((char*)idx_map.data, idx_map.size, word, less); 
  if (less(word, lb)) {
    return empty_slice;
  }
  char *it = (char*)memchr((void*)lb.ptr, '\t', lb.size);
  if (!it) return empty_slice;
  it = it + 1;
  size_t offset;
  if (read_num(it, lb.size - distance(lb.ptr, it), offset)) return empty_slice;
  
  char *it2 = (char*)memchr((void*)it, '\t', lb.size - distance(lb.ptr, it));
  if (!it2) return empty_slice;
  it2 = it2 + 1;
  size_t len;
  if (read_num(it2, lb.size - distance(lb.ptr, it2), len)) return empty_slice;

  return slice_t((char*)map.data + offset, len);
}


int read_bi_stat_line(const slice_t &slice, slice_t &w1, slice_t &w2, size_t &count) {
  const char* it1 = slice.find(0, ' ');
  if (!it1) return -1;
  w1.assign(slice.ptr, distance(slice.ptr, it1));
  const char* it2 = slice.find(distance(slice.ptr, it1) + 1, '\t');
  if (!it2) return -1;
  w2.assign(const_cast<char*>(it1 + 1), distance(it1, it2) - 1);
  if (read_num(it2 + 1, slice.size - (it2 + 1 - slice.ptr), count)) {
    return -1;
  }
  return 0;
} 

inline double _sqr(double a) {
  return a*a;
}
double calc_similarity_score_slow(const slice_t &stat1, const slice_t &stat2) {
  typedef std::map<slice_t, size_t> freq_map_t;
  freq_map_t freq_map1;
  freq_map_t freq_map2;
  typedef std::set<slice_t> key_set_t;
  key_set_t key_set;

  double sum1 = 0.0f;
  double sum2 = 0.0f;
  slice_t END_WORD(const_cast<char*>("$"), 1);
  for (slice_reader_t r1(stat1); !r1.empty(); r1.next()) {
    slice_t line = r1.get();
    slice_t w1, w2;
    size_t count;
    if (read_bi_stat_line(line, w1, w2, count)) {
      continue;
    }
    if (w2 == END_WORD) {
      //std::cout << "ignore " << w2 << std::endl;
      continue;
    }
    freq_map1.insert(std::make_pair(w2,count));
    key_set.insert(w2);
    sum1 += count;
  }
  for(slice_reader_t r2(stat2); !r2.empty(); r2.next()) {
    slice_t line = r2.get();
    
    slice_t w1, w2;
    size_t count;
    if (read_bi_stat_line(line, w1, w2, count)) {
      continue;
    }
    if (w2 == END_WORD) {
      //std::cout << "ignore " << w2 << std::endl;
      continue;
    }
    freq_map2.insert(std::make_pair(w2,count));
    key_set.insert(w2);
    sum2 += count;
  }
   
  double error = 0.0f;
  double base = 0.0f; 
  key_set_t::const_iterator key_it = key_set.begin();
  key_set_t::const_iterator key_end = key_set.end();
  for (;key_it != key_end; ++key_it) {
    freq_map_t::const_iterator it1 = freq_map1.find(*key_it);
    freq_map_t::const_iterator it2 = freq_map2.find(*key_it);
    
    if (it1 == freq_map1.end()) {
      error += _sqr(it2->second * sum1);
      base += _sqr(it2->second * sum1);
    } else if (it2 == freq_map2.end()) {
      error += _sqr(it1->second * sum2);
      base += _sqr(it1->second * sum2);
    } else {
      error += _sqr(it1->second * sum2 - it2->second * sum1);
      base += _sqr(it1->second * sum2) + _sqr(it2->second * sum1);
    }
  }
  if (base == 0.0f) return 666.0; //completely different
  return error / base;
}

#endif
