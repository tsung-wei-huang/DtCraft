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

// Function: csv_size
// Find the row/column sizes of a given csv table.
std::tuple<size_t, size_t> csv_size(const std::filesystem::path& file, const char del) {

  assert(del == ',' || del == ';');

  using namespace std::literals;

  std::ifstream ifs(file);
  
  if(!ifs) {
    DTC_THROW("Failed to open ", file);
  }
  
  size_t num_rows {0};
  size_t num_cols {0};
  
  std::string line;
  std::string token;
  std::stringstream lss;

  while(std::getline(ifs, line)) {
    
    // Skip the empty line
    if(line.size() == 0) {
      continue;
    }

    lss.clear();
    lss.str(line);
    
    size_t num_cols_this_line {0};

    while(lss.good()) {
      std::getline(lss, token, del);
      ++num_cols_this_line; 
    }

    if(num_cols_this_line > num_cols) {
      num_cols = num_cols_this_line;
    }
    
    ++num_rows;
  }

  return std::make_tuple(num_rows, num_cols);
}


};  // end of namespace dtc. ----------------------------------------------------------------------
