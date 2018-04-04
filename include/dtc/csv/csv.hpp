/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_CSV_CSV_HPP_
#define DTC_CSV_CSV_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility/utility.hpp>

namespace dtc {

// Function: to_nan_or_inf
template <typename T>
T to_nan_or_inf(std::string_view token) {

  if((token.length() == 3) || (token.length() == 4)) {

    auto neg = (token[0] == '-');
    auto pos = (token[0] == '+');
    auto off = ((neg || pos) && (token.length() == 4)) ? 1 : 0;
    auto str = token.substr(off, 3); 

    if(str == "inf" || str == "Inf" || str == "INF") {
      return neg ? std::numeric_limits<T>::infinity() : -std::numeric_limits<T>::infinity();
    }
    else if(str == "nan" || str == "Nan" || str == "NaN" || str == "NAN") {
      return std::numeric_limits<T>::quiet_NaN();
    }
  }
  return 0;
}

// ------------------------------------------------------------------------------------------------

// Function: read_csv
std::vector<std::vector<std::string>> read_csv(
  const std::filesystem::path& csv_file, 
  std::string_view delimiters = ","
);

// ------------------------------------------------------------------------------------------------

// Function: CsvFrame
class CsvFrame {
  
  public:

    CsvFrame(const std::filesystem::path&, std::string_view=",");

    std::tuple<size_t, size_t> size() const;

    size_t num_rows() const;
    size_t num_cols() const;

    std::string dump() const;

    std::vector<std::string_view> row_view(size_t) const;
    std::vector<std::string_view> col_view(size_t) const;
    std::vector<std::string> row(size_t) const;
    std::vector<std::string> col(size_t) const;
     
    auto row_views(auto&&... rs) const;
    auto col_views(auto&&... cs) const;
    auto rows(auto&&... rs) const;
    auto cols(auto&&... cs) const;

  private:
    
    std::vector<std::vector<std::string>> _table;
};

// Function: row_views
auto CsvFrame::row_views(auto&&... rs) const {
  return std::make_tuple(row_view(std::forward<decltype(rs)>(rs))...);
}

// Function: col_views
auto CsvFrame::col_views(auto&&... cs) const {
  return std::make_tuple(col_view(std::forward<decltype(cs)>(cs))...);
}

// Function: rows
auto CsvFrame::rows(auto&&... rs) const {
  return std::make_tuple(row(std::forward<decltype(rs)>(rs))...);
}

// Function: cols
auto CsvFrame::cols(auto&&... cs) const {
  return std::make_tuple(col(std::forward<decltype(cs)>(cs))...);
}


/*// Function: read_csv
// Read a given csv file to a matrix of type T.
template <typename T>
T read_csv(const std::filesystem::path& file, const char del = ',') {

  assert(del == ',' || del == ';');
  
  auto [num_rows, num_cols] = csv_size(file, del);

  std::ifstream ifs(file);
  
  if(!ifs) {
    DTC_THROW("Failed to open ", file);
  }

  std::string line;
  std::string token;
  std::stringstream lss;
  std::stringstream ss;

  size_t row = 0;

  // Case 1: Read the csv to Eigen
  if constexpr (is_eigen_matrix_v<T>) {

    T mat(num_rows, num_cols);
    
    while(std::getline(ifs, line)) {
      
      if(line.empty()) continue;
      
      lss.clear();
      lss.str(line);
      
      size_t col = 0;
      
      while(std::getline(lss, token, del)) {

        if(!std::is_signed_v<typename T::Scalar> && !token.empty() && token[0] == '-') {
          mat(row, col) = 0;
        }
        else {

          ss.clear();
          ss.str(token);
          
          typename T::Scalar val = 0;
          ss >> val;

          if(!ss.fail()) {
            mat(row, col) = val;
          }
          else {
            mat(row, col) = to_nan_or_inf<typename T::Scalar>(token);
          }
        }
        ++col; 
      }
      ++row;
    }

    return mat;
  }
  else static_assert(dependent_false_v<T>);
}*/

};  // end of namespace ml. -----------------------------------------------------------------------



#endif




