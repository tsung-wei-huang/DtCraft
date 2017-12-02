/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_IPC_IOS_HPP_
#define DTC_IPC_IOS_HPP_

#include <dtc/ipc/streambuf.hpp>
#include <dtc/archive/binary.hpp>

namespace dtc {

//// Class: OutputStream
//class OutputStream : public OutputStreamBuffer {
//  
//  friend class BinaryOutputArchiver;
//
//  public:
//
//    template <typename... T>
//    OutputStream(T&&...);
//
//    template <typename... T>
//    std::streamsize operator ()(T&&...);
//};
//
//// Constructor.
//template <typename... T>
//OutputStream::OutputStream(T&&... args) : 
//  OutputStreamBuffer {std::forward<T>(args)...} {
//}
//
//// Inserter
//template <typename... T>
//std::streamsize OutputStream::operator ()(T&&... t) {
//  return BinaryOutputPackager(*this)(std::forward<T>(t)...);
//}
//
////-------------------------------------------------------------------------------------------------
//
//// Class: InputStream
//class InputStream : public InputStreamBuffer {
//  
//  friend class BinaryInputArchiver;
//
//  public:
//
//    template <typename... T>
//    InputStream(T&&...);
//
//    template <typename... T>
//    std::streamsize operator ()(T&&...);
//};
//
//// Constructor.
//template <typename... T>
//InputStream::InputStream(T&&... args) :
//  InputStreamBuffer {std::forward<T>(args)...} {
//}
//
//// Extractor
//template <typename... T>
//std::streamsize InputStream::operator ()(T&&... t) {
//  return BinaryInputPackager(*this)(std::forward<T>(t)...);
//}

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif
