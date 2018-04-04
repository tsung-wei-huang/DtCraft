/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/csv/csv.hpp>

namespace dtc {

// Function: read_csv
std::vector<std::vector<std::string>> read_csv(const std::filesystem::path& path, std::string_view dels) {

  std::ifstream ifs(path);

  if(!ifs.good()) {
    DTC_THROW("Failed to open ", path);
  }

  std::vector<std::vector<std::string>> table;

  std::string line;
  std::string token;
  
  bool is_first_line {true};
  size_t num_cols {0};

  while(read_line(ifs, line)) {

    if(line.empty()) continue;

    std::vector<std::string> row;

    for(const auto& c : line) {
      if(std::find(dels.begin(), dels.end(), c) != dels.end()) {
        row.push_back(std::move(token));
      }
      else {
        token.push_back(c);
      }
    }
    row.push_back(std::move(token));

    if(is_first_line) {
      num_cols = row.size();
      is_first_line = false;
    }
    else if(num_cols != row.size()) {
      DTC_THROW("Mismatched column size in line: ", line);
    }

    table.push_back(std::move(row));
  }

  return table;
}

// ------------------------------------------------------------------------------------------------

// Constructor
CsvFrame::CsvFrame(const std::filesystem::path& path, std::string_view dels) : 
  _table {read_csv(path, dels)} {
}

// Function: size
std::tuple<size_t, size_t> CsvFrame::size() const {
  if(_table.empty()) return {0, 0};
  return {_table.size(), _table[0].size()};
}

// Function: num_cols
size_t CsvFrame::num_cols() const {
  return std::get<1>(size());
}

// Function: num_rows
size_t CsvFrame::num_rows() const {
  return std::get<0>(size());
}

// Function: row_view
std::vector<std::string_view> CsvFrame::row_view(size_t r) const {

  std::vector<std::string_view> view;
  
  if(r >= num_rows()) return view;

  for(const auto& item : _table[r]) {
    view.push_back(item);
  }

  return view;
}

// Function: row
std::vector<std::string> CsvFrame::row(size_t r) const {

  std::vector<std::string> strs;
  
  if(r >= num_rows()) return strs;

  for(const auto& item : _table[r]) {
    strs.push_back(item);
  }

  return strs;
}


// Function: col_view
std::vector<std::string_view> CsvFrame::col_view(size_t c) const {
  
  std::vector<std::string_view> view;

  if(c >= num_cols()) return view;

  auto _num_rows = num_rows();

  for(size_t i=0; i<_num_rows;++i) {
    view.push_back(_table[i][c]);
  }

  return view;
}

// Function: col
std::vector<std::string> CsvFrame::col(size_t c) const {
  
  std::vector<std::string> strs;

  if(c >= num_cols()) return strs;

  auto _num_rows = num_rows();

  for(size_t i=0; i<_num_rows;++i) {
    strs.push_back(_table[i][c]);
  }

  return strs;
}




};  // end of namespace dtc. ----------------------------------------------------------------------




