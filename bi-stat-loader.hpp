#ifndef BI_STAT_LOADER_HPP
#define BI_STAT_LOADER_HPP 1

#include "ptr_tools.hpp"
#include "slice.hpp"
#include "slice-reader.hpp"

#include <string>
#include <map>
#include <set>

#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <iostream>
#include <ostream>

struct mmap_t {
  mmap_t()
    :fd(-1)
    ,data(NULL) {}
 
  // return 0 on success
  int load(const char *fname) {
    fd = open(fname, O_RDONLY);
    if (fd == -1) {
      perror("Error opening file for reading");
      return -1;    
    }

    struct stat st;
    fstat(fd, &st);
    size = st.st_size;

    data = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
      close(fd);
      perror("Error mmapping the file");
      return -1;
    }
    
    return 0;  
  }
  
  // returns 0 on success
  int free() {
    if (munmap(data, size) == -1) {
      perror("Error un-mmapping the file");
      return -1;
    }
    close(fd);
    return 0;
  }
  int fd;
  void *data;
  size_t size;
};

template <typename action_t>
void process_lines(void *data, size_t size, action_t &action) {
  size_t size_left = size;
  void *it = data;
  for (; size_left > 0;) {
    void *pos = memchr(it, '\n', size_left);
    if (!pos) {
      action.process(it, size_left);
      break;
    }
    pos = advance(pos, 1);
    action.process(it, (char*)pos - (char*)it); 
    size_left -= (char*)pos - (char*)it;
    it = pos;
  }
  action.commit();
}
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
  ordered_bi_reader_t(slice_map_t &_slice_map)
    :chunk_begin(NULL)
    ,chunk_size(0)
    ,slice_map(_slice_map) {}

  void commit_chunk() {
    if (!chunk_begin) return;
    slice_t chunk(chunk_begin, chunk_size);
    slice_map.insert(std::make_pair(make_str(last_word), chunk));
//    std::cout << "insert(" << last_word << "," << chunk <<")" << std::endl; 
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
  char *chunk_begin;
  size_t chunk_size; 
  slice_map_t &slice_map;
};

struct bi_stat_t {
  int init(const mmap_t &_map) {
    map_ptr = &_map;
    ordered_bi_reader_t action(slice_map);
    process_lines(map_ptr->data, map_ptr->size, action);
    return 0;
  }

  const slice_t& get_stat(const std::string &word) const {
    slice_map_t::const_iterator it = slice_map.find(word);
    if (it == slice_map.end()) return empty_slice;
    return it->second;
  }

  slice_map_t slice_map;
  slice_t empty_slice;
  const mmap_t *map_ptr;
};

template <typename data_t>
int read_num(const char *data, size_t size, data_t &res) {
  res = 0;
  size_t char_read = 0;
  const data_t base = 10;
  for(size_t i = 0; i < size; ++i) {
    if (data[i] >= '0' and data[i] <= '9') break;
  }
  
  for(size_t i = 0; i < size; ++i) {
    if (data[i] < '0' or data[i] > '9') break;
    res = res * base + data[i] - '0';
    ++char_read;
  }
  return char_read > 0 ? 0 : -1;
}
int read_bi_stat_line(const slice_t &slice, slice_t &w1, slice_t &w2, size_t &count) {
  const char* it1 = slice.find(0, ' ');
  if (!it1) return -1;
  w2.assign(slice.ptr, it1 + 1 - slice.ptr);
  const char* it2 = slice.find(it1 + 1 - slice.ptr, '\t');
  if (!it2) return -1;
  w2.assign(const_cast<char*>(it1 + 1), it2 - it1);
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
  for (slice_reader_t r1(stat1); !r1.empty(); r1.next()) {
    slice_t line = r1.get();
    slice_t w1, w2;
    size_t count;
    if (read_bi_stat_line(line, w1, w2, count)) {
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
