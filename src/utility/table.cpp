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

#include <dtc/utility/table.hpp>

namespace dtc {

// Procedure: end_of_row
void TableBuilder::end_of_row() {
  _rows.push_back(_current);
  _current.assign(0, "");
}

// Procedure: setup
void TableBuilder::setup() {
  _set_widths();
  _set_alignments();
}

// Function: ruler
std::string TableBuilder::ruler() const {
  std::string result;
  result += corner;
	for(auto w : _widths) {
		result.append(w, horizontal); 
    result += corner;
  }
  return result;
}
  
// Function: to_string
std::string TableBuilder::to_string() {
  std::ostringstream os;
  setup();
  os << ruler() << "\n";
  for(const auto& row : rows()) {
    os << vertical;
    for(size_t i = 0; i < row.size(); ++i) {
      auto a = alignment(i) == TableBuilder::LEFT ? std::left : std::right;
      os << std::setw(width(i)) << a << row[i];
      os << vertical;
    }
    os << "\n";
    os << ruler() << "\n";
  }
  return os.str();
}

// Procedure: _set_widths
void TableBuilder::_set_widths() {
  _widths.assign(num_columns(), 0);
  for(const auto& row : _rows) {
    for(size_t i = 0; i < row.size(); ++i) {
      _widths[i] = _widths[i] > row[i].size() ? _widths[i] : row[i].size();
    }
  }
}

// Procedure: _set_alignments
void TableBuilder::_set_alignments() {
  for(size_t i = 0; i < num_columns(); ++i) {
    if(_alignments.find(i) == _alignments.end()) {
      _alignments[i] = Alignment::LEFT;
    }
  }
}

// OutputStream
std::ostream & operator<<(std::ostream & os, TableBuilder& table) {

  os << table.to_string();

  return os;
}


};  // End of namespace dtc. ----------------------------------------------------------------------
