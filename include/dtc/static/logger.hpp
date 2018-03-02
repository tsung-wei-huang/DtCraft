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

#ifndef DTC_STATIC_LOGGER_HPP_
#define DTC_STATIC_LOGGER_HPP_

#include <dtc/policy.hpp>
#include <dtc/utility/logger.hpp>

namespace dtc {

// Global declaration and macro usage.
inline static Logger<FileLogPolicy> logger(env::log_file());

#define LOG_REMOVE_FIRST_HELPER(N, ...) __VA_ARGS__
#define LOG_GET_FIRST_HELPER(N, ...) N
#define LOG_GET_FIRST(...) LOG_GET_FIRST_HELPER(__VA_ARGS__)
#define LOG_REMOVE_FIRST(...) LOG_REMOVE_FIRST_HELPER(__VA_ARGS__)

#define LOGTO(...) logger.redir (__VA_ARGS__)

#define LOGD(...) logger.debug  (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define LOGI(...) logger.info   (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define LOGW(...) logger.warning(__FILE__, __LINE__, __VA_ARGS__, '\n')
#define LOGE(...) logger.error  (__FILE__, __LINE__, __VA_ARGS__, '\n')
#define LOGF(...) logger.fatal  (__FILE__, __LINE__, __VA_ARGS__, '\n')

};  // end of namespace dtc. ----------------------------------------------------------------------


#endif


