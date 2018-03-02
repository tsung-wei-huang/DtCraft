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

#ifndef DTC_UTILITY_IOS_HPP_
#define DTC_UTILITY_IOS_HPP_

#include <iostream>
#include <fstream>

namespace dtc {

// Function: read_line
template <typename C, typename T, typename A>
std::basic_istream<C, T>& read_line(std::basic_istream<C, T>& is, std::basic_string<C, T, A>& line) {

  line.clear();

  typename std::basic_istream<C, T>::sentry se(is, true);        
  std::streambuf* sb = is.rdbuf();

  for(;;) {

    switch (int c = sb->sbumpc(); c) {

      // case 1: newline
      case '\n':
        return is;
      break;

      // case 2: carriage return
      case '\r':
        if(sb->sgetc() == '\n'){
          sb->sbumpc();
        }
        return is;
      break;
      
      // case 3: eof
      case std::streambuf::traits_type::eof():
        // Also handle the case when the last line has no line ending
        if(line.empty()) {
          is.setstate(std::ios::eofbit | std::ios::failbit);
        }
        return is;
      break;

      default:
        line.push_back(static_cast<char>(c));
      break;
    }
  } 

  return is;
}


};  // end of namespace . -------------------------------------------------------------------------

#endif
