/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_UTILITY_TABLE_HPP_
#define DTC_UTILITY_TABLE_HPP_

#include <iostream>
#include <map>
#include <iomanip>
#include <vector>
#include <string>
#include <unordered_map>

namespace dtc {

// Class: TableBuilder
class TableBuilder {

  public:

  enum Alignment { 
    LEFT, 
    RIGHT 
  };
 
  using row_t = std::vector<std::string>;

    const char horizontal {'-'};
    const char vertical {'|'};
    const char corner {'+'};

    TableBuilder() = default; 
    TableBuilder(const TableBuilder&) = default;
    TableBuilder(TableBuilder&&) = default;

    TableBuilder& operator = (const TableBuilder&) = default;
    TableBuilder& operator = (TableBuilder&&) = default;

    template <typename... T>
    TableBuilder& add(T&&...);

		inline TableBuilder& alignment(size_t, Alignment);

		inline Alignment alignment(size_t) const;
    inline const std::vector<row_t>& rows() const;

    inline size_t width(size_t) const;
    inline size_t num_columns() const;

    void end_of_row();
    void setup();

    std::string ruler() const;
    std::string to_string();

  private:

    row_t _current;

    std::vector<row_t> _rows;
    std::vector<size_t> _widths;
    std::unordered_map<size_t, Alignment> _alignments;

    void _set_widths();
    void _set_alignments();
};
		
inline TableBuilder& TableBuilder::alignment(size_t i, Alignment a) {
	_alignments[i] = a;
  return *this;
}

inline TableBuilder::Alignment TableBuilder::alignment(size_t i) const {
	return _alignments.at(i);
}

template <typename... T>
TableBuilder& TableBuilder::add(T&&... args) {
  (_current.emplace_back(std::forward<T>(args)), ...);
  return *this;
}

inline const std::vector<typename TableBuilder::row_t>& TableBuilder::rows() const {
  return _rows;
}

inline size_t TableBuilder::width(size_t i) const { 
  return _widths[i]; 
}

inline size_t TableBuilder::num_columns() const {
  return _rows[0].size();
}

// Operator: <<
std::ostream & operator<<(std::ostream &, TableBuilder&);


};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
