/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_LOGGER_HPP_
#define DTC_LOGGER_HPP_

#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>

namespace dtc {

// Enum: LogType
enum class LogType {
  DEBUG   = 0,
  INFO    = 1,
  WARNING = 2,
  ERROR   = 3,
  FATAL   = 4
};

// Class: Logger
template<typename L = std::mutex>
class Logger {

  public:
    
    static constexpr const char* ERROR_COLOR   {"\033[1;31m"};
    static constexpr const char* FATAL_COLOR   {"\033[1;31m"};
    static constexpr const char* DEBUG_COLOR   {"\033[1;36m"};
    static constexpr const char* WARNING_COLOR {"\033[1;33m"};
    static constexpr const char* RESET_COLOR   {"\033[0m"   };

    Logger(const std::string& fpath = "");
    ~Logger();

    template <typename...ArgsT>
    void debug(const char*, const int, ArgsT&&...);
    
    template <typename...ArgsT>
    void info(const char*, const int, ArgsT&&...);
    
    template <typename...ArgsT>
    void warning(const char*, const int, ArgsT&&...);
    
    template <typename...ArgsT>
    void error(const char*, const int, ArgsT&&...);
    
    template <typename...ArgsT>
    void fatal(const char*, const int, ArgsT&&...);

    void redir(const std::string&);
  
  private:

    mutable L _mutex;

    FILE* _handle {nullptr};
    
    constexpr const char* _strend(const char*) const;
    constexpr const char* _basename(const char*, const char*) const;
    constexpr const char* _basename(const char*) const;

    static pid_t _gettid();
    
    template <LogType severity, typename...ArgsT>
    void _write(const char*, const int, ArgsT&&...);
};

// Constructor.
template <typename L>
Logger<L>::Logger(const std::string& fpath) {
  redir(fpath);
}

// Destructor.
template <typename L>
Logger<L>::~Logger() {
  if(_handle) {
    std::fclose(_handle);
  }
}

// Function: _gettid
// Adoped from google's open-source library glog.
template <typename L>
pid_t Logger<L>::_gettid() {
  // On Linux and MacOSX, we try to use gettid().
#if defined OS_LINUX || defined OS_MACOSX
#ifndef __NR_gettid
#ifdef OS_MACOSX
#define __NR_gettid SYS_gettid
#elif ! defined __i386__
#error "Must define __NR_gettid for non-x86 platforms"
#else
#define __NR_gettid 224
#endif
#endif
  static bool lacks_gettid = false;
  if (!lacks_gettid) {
    pid_t tid = syscall(__NR_gettid);
    if (tid != -1) {
      return tid;
    }   
    // Technically, this variable has to be volatile, but there is a small
    // performance penalty in accessing volatile variables and there should
    // not be any serious adverse effect if a thread does not immediately see
    // the value change to "true".
    lacks_gettid = true;
  }
#endif  // OS_LINUX || OS_MACOSX

  // If gettid() could not be used, we use one of the following.
#if defined OS_LINUX
  return getpid();  // Linux:  getpid returns thread ID when gettid is absent
#elif defined OS_WINDOWS || defined OS_CYGWIN
  return GetCurrentThreadId();
#else
  // If none of the techniques above worked, we use pthread_self().
  return (pid_t)(uintptr_t)pthread_self();
#endif
}
    
// Function: _strend
// Compile-time finding of the end of a string.
template <typename L>
constexpr const char* Logger<L>::_strend(const char* str) const {
  return *str ? _strend(str + 1) : str;
}

// Function: _basename
// Compile-time finding of a file name.
template <typename L>
constexpr const char* Logger<L>::_basename(const char* beg, const char* end) const {
  return (end >= beg && *end != '/') ? _basename(beg, end - 1) : (end + 1);
}

// Function: _basename
// Compile-time finding of a file name.
template <typename L>
constexpr const char* Logger<L>::_basename(const char* fpath) const {
  return _basename(fpath, _strend(fpath));
}

// Procedure: redir
template <typename L>
void Logger<L>::redir(const std::string& fpath) {

  if(fpath.empty()) {
    return;
  }

  int fd = ::open(fpath.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

  if (fd == -1) {
    return;
  }

  // Mark the file close-on-exec.
  ::fcntl(fd, F_SETFD, FD_CLOEXEC);

  if(_handle = ::fdopen(fd, "wa"); _handle == nullptr) {
    ::close(fd);
    ::unlink(fpath.c_str());
  }
}

// Procedure: write
template<typename L>
template<LogType severity, typename...ArgsT>
void Logger<L>::_write(const char* fpath, const int line, ArgsT&&... args) {

  std::ostringstream oss;
  
  // ------------------------
  // Message header
  // ------------------------
  if constexpr(severity == LogType::DEBUG) {
    oss << DEBUG_COLOR << "D ";
  }
  else if constexpr(severity == LogType::WARNING) {
    oss << WARNING_COLOR << "W ";
  }
  else if constexpr(severity == LogType::ERROR) {
    oss << ERROR_COLOR << "E "; 
  }
  else if constexpr(severity == LogType::FATAL) {
    oss << FATAL_COLOR << "F ";
  }
  else {
    oss << "I ";
  }
  
  // Append the thread id.
  oss << std::setw(5) << static_cast<unsigned short>(_gettid()) << ' ';
  
  // Insert timeinfo
  auto rt = std::time(nullptr);
  auto lt = *std::localtime(&rt);
  oss << std::put_time(&lt, "%Y-%m-%d %T ");
  oss << _basename(fpath) << ":" << line << "] ";

  // ------------------------
  // Message body
  // ------------------------
  (oss << ... << args);
  
  // ---- Message tail ----
  if constexpr(severity != LogType::INFO) {
    oss << RESET_COLOR;
  }

  const std::string& message = oss.str();
  const auto target = _handle ? _handle : stderr;

  // ------------------------
  // Write to the device.
  // ------------------------
  std::scoped_lock lock(_mutex);
  std::fwrite(message.data(), 1, message.length(), target);
  std::fflush(target);

  if constexpr(severity == LogType::FATAL) {
    std::abort();
  }
}

// Procedure: debug
template <typename L>
template <typename... ArgsT>
void Logger<L>::debug(const char* fpath, const int line, ArgsT&&... args) {
  _write<LogType::DEBUG>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: info
template <typename L>
template <typename... ArgsT>
void Logger<L>::info(const char* fpath, const int line, ArgsT&&... args) {
  _write<LogType::INFO>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: warning
template <typename L>
template <typename... ArgsT>
void Logger<L>::warning(const char* fpath, const int line, ArgsT&&... args) {
  _write<LogType::WARNING>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: error
template <typename L>
template <typename... ArgsT>
void Logger<L>::error(const char* fpath, const int line, ArgsT&&... args) {
  _write<LogType::ERROR>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: fatal
// Log a fatal message and generate the backtrace symbols.
template <typename L>
template <typename... ArgsT>
void Logger<L>::fatal(const char* fpath, const int line, ArgsT&&... args) {
  _write<LogType::FATAL>(fpath, line, std::forward<ArgsT>(args)...);
}

//-------------------------------------------------------------------------------------------------

// Atomic cout
template <typename... Ts>
void cout(Ts&&... args) {
  std::ostringstream oss;
  (oss << ... << args); 
  const std::string& message = oss.str();
  std::fwrite(message.data(), 1, message.length(), stdout);
}

// Atomic cerr
template <typename... Ts>
void cerr(Ts&&... args) {
  std::ostringstream oss;
  const std::string& message = oss.str();
  std::fwrite(message.data(), 1, message.length(), stderr);
}

};  // End of namespace dtc. --------------------------------------------------------------


#endif









