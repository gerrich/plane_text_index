#ifndef MY_IO_HPP
#define MY_IO_HPP
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
#endif
