#ifndef ACTION_IO_HPP
#define ACTION_IO_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#include "ptr_tools.hpp"

template <typename action_t>
void process_lines(
    std::istream &is,
    action_t &action
) {
  while(is.good()) {
    std::string line;
    std::getline(is, line);
    
    if (line.empty() and !is.good()) break;
    action(line);
  }
}

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

template <typename action_t>
struct line_1str_adapter_t {
  line_1str_adapter_t(action_t &_action)
    :action(_action) {}

  void operator()(const std::string &line) {
    action(line);
  }

  action_t &action;
};

template <typename action_t>
void process_1str(
    std::istream &is,
    action_t &action
) {
  line_1str_adapter_t<action_t> line_action(action);
  process_lines(is, line_action); 
}

template <typename action_t>
struct line_2str_adapter_t {
  line_2str_adapter_t(action_t &_action)
    :action(_action) {}

  void operator()(const std::string &line) {
    if (line.empty()) {
      action(std::string(), std::string());
      return;
    }
    size_t tab_pos = line.find('\t');
    if (tab_pos == std::string::npos) {
      action(line, std::string());
      return;
    }
    std::string f1(line,0, tab_pos);

    size_t tab_pos_2 = line.find('\t', tab_pos + 1);
    std::string f2;
    if (tab_pos_2 == std::string::npos) {
      f2.assign(line, tab_pos + 1, std::string::npos);
    } else {
      f2.assign(line, tab_pos + 1, tab_pos_2 - tab_pos + 1);  
    }
    action(f1, f2);
  }

  action_t &action;
};

// run action_t on pair of fields delimited with '\t'
// on empty line run action on ("","")
// if no '\t' - run on (line, "")
template <typename action_t>
void process_2str(
    std::istream &is,
    action_t &action
) {
  line_2str_adapter_t<action_t> line_action(action);
  process_lines(is, line_action); 
}


template <typename action_t>
struct line_2str_sz_adapter_t {
  line_2str_sz_adapter_t(action_t &_action)
    :action(_action) {}

  void operator()(const std::string &line) {
    if (line.empty()) {
//      action(std::string(), std::string(), 1);
      return;
    }
    size_t tab_pos = line.find('\t');
    if (tab_pos == std::string::npos) {
//      action(line, std::string(), 1);
      return;
    }
    std::string f1(line,0, tab_pos);

    size_t tab_pos_2 = line.find('\t', tab_pos + 1);
    std::string f2;
    if (tab_pos_2 == std::string::npos) {
//      f2.assign(line, tab_pos + 1, std::string::npos);
//      action(f1, f2, 1);
      return;
    } else {
      f2.assign(line, tab_pos + 1, tab_pos_2 - tab_pos - 1);  
    }
    
    std::string tail(line, tab_pos_2 + 1);
    size_t count = 0;
    if (1 != sscanf(tail.c_str(), "%zu", &count)) return;
    action(f1, f2, count);
  }

  action_t &action;
};

template <typename action_t>
void process_2str_sz(
    std::istream &is,
    action_t &action
) {
  line_2str_sz_adapter_t<action_t> line_action(action);
  process_lines(is, line_action); 
}

template <typename action_t>
struct line_1str_sz_adapter_t {
  line_1str_sz_adapter_t(action_t &_action)
    :action(_action) {}

  void operator()(const std::string &line) {
    if (line.empty()) {
//      action(std::string(), std::string(), 1);
      return;
    }
    size_t tab_pos = line.find('\t');
    if (tab_pos == std::string::npos) {
//      action(line, std::string(), 1);
      return;
    }
    std::string f1(line,0, tab_pos);
    
    std::string tail(line, tab_pos + 1);
    size_t count = 0;
    if (1 != sscanf(tail.c_str(), "%zu", &count)) return;
    action(f1, count);
  }

  action_t &action;
};

template <typename action_t>
void process_1str_sz(
    std::istream &is,
    action_t &action
) {
  line_1str_sz_adapter_t<action_t> line_action(action);
  process_lines(is, line_action); 
}
#endif
