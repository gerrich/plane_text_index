#ifndef BI_STAT_LOADER_HPP
#define BI_STAT_LOADER_HPP 1

#include "slice.hpp"
#include "mmap.hpp"

// TODO change with propriate error logger
//void perror(const char*) {}

size_t calc_lines(void *data, size_t size);

void build_bi_stat_index(const mmap_t &map);

const slice_t get_stat(const mmap_t &map, const mmap_t &idx_map, const slice_t &word);

double calc_similarity_score_slow(const slice_t &stat1, const slice_t &stat2);

#endif
