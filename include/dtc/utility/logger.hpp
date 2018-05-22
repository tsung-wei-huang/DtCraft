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

namespace dtc {

// Enum: LoggingSeverity
enum class LoggingSeverity {
  DEBUG   = 0,
  INFO    = 1,
  WARNING = 2,
  ERROR   = 3,
  FATAL   = 4
};

//-------------------------------------------------------------------------------------------------

// Class: LogPolicyInterface
template <typename Policy>
class LogPolicyInterface {

  protected:

    std::ostream* _os {&std::cerr};

    bool _color {true};

  public:
    
    Policy* policy();
    const Policy* policy() const;

    void open(const std::string& name);
    void close();
    void write(const std::string& msg);
    void color(bool);

    bool color() const;
};

// Function: policy
template <typename Policy>
Policy* LogPolicyInterface<Policy>::policy() {
  return static_cast<Policy*>(this);
}

// Function: policy
template <typename Policy>
const Policy* LogPolicyInterface<Policy>::policy() const {
  return static_cast<const Policy*>(this);
}
    
// Procedure: open
template <typename Policy>
void LogPolicyInterface<Policy>::open(const std::string& name) {
  return policy()->_open(name);
}

// Procedure: close
template <typename Policy>
void LogPolicyInterface<Policy>::close() {
  return policy()->_close();
}

// Procedure: write
template <typename Policy>
void LogPolicyInterface<Policy>::write(const std::string& msg) {
  return policy()->_write(msg);
}

// Function: color
template <typename Policy>
bool LogPolicyInterface<Policy>::color() const {
  return _color;
}

//-------------------------------------------------------------------------------------------------

// Class: FileLogPolicy
class FileLogPolicy : public LogPolicyInterface <FileLogPolicy> {

  friend class LogPolicyInterface<FileLogPolicy>;

  private:

    std::ofstream _ofs;

  public:

    inline FileLogPolicy();
    inline ~FileLogPolicy();

  private:

    inline void _open(const std::string&);
    inline void _close();
    inline void _write(const std::string&);
};

// Constructor
inline FileLogPolicy::FileLogPolicy() {
}

// Destructor
inline FileLogPolicy::~FileLogPolicy() {
}
  
// Procedure: write
inline void FileLogPolicy::_write(const std::string& msg) {
  *(_os) << msg;
  _os->flush();
}

// Procedure: open
inline void FileLogPolicy::_open(const std::string& name) {

  close();
  _ofs.open(name, std::ios_base::out);

  // Redirect the log to the file stream.
  if(_ofs.is_open()) {
    _os = &_ofs;
  }
}

// Procedure: close
inline void FileLogPolicy::_close() {
  if(_ofs.is_open()) {
    _ofs.close();
    _os = &std::cerr;
  }
}
  
//-------------------------------------------------------------------------------------------------

// Class: Logger
template<typename LogPolicyT>
class Logger {

  private:

    static constexpr const char* _set_green   {"\033[1;32m"};
    static constexpr const char* _set_yellow  {"\033[1;33m"};
    static constexpr const char* _set_cyan    {"\033[1;36m"};
    static constexpr const char* _set_magenta {"\033[1;35m"};
    static constexpr const char* _reset_color {"\033[0m"   };
    
    LogPolicyT _policy;

    //std::atomic_flag _lock = ATOMIC_FLAG_INIT;

  public:
    
    static constexpr const char* ERROR_COLOR   {"\033[1;31m"};
    static constexpr const char* FATAL_COLOR   {"\033[1;31m"};
    static constexpr const char* DEBUG_COLOR   {"\033[1;36m"};
    static constexpr const char* WARNING_COLOR {"\033[1;33m"};
    static constexpr const char* RESET_COLOR   {"\033[0m"   };

    Logger(const std::string& fpath = "");
    ~Logger();

    template <LoggingSeverity severity, typename...ArgsT>
    void write(const char*, const int, ArgsT&&...);
    
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
    
    template <LoggingSeverity severity>
    void _append_header(std::ostringstream&, const char*, const int);
    
    template <LoggingSeverity s>
    void _append_severity(std::ostringstream&);
    
    constexpr const char* _strend(const char*) const;
    constexpr const char* _basename(const char*, const char*) const;
    constexpr const char* _basename(const char*) const;

    static pid_t _gettid();
};

// Constructor.
template <typename LogPolicyT>
Logger<LogPolicyT>::Logger(const std::string& fpath) {
  _policy.open(fpath);
}

// Destructor.
template <typename LogPolicyT>
Logger<LogPolicyT>::~Logger() {
  _policy.close();
}

// Function: _gettid
// Adoped from google's open-source library glog.
template <typename LogPolicyT>
pid_t Logger<LogPolicyT>::_gettid() {
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
template <typename LogPolicyT>
constexpr const char* Logger<LogPolicyT>::_strend(const char* str) const {
  return *str ? _strend(str + 1) : str;
}

// Function: _basename
// Compile-time finding of a file name.
template <typename LogPolicyT>
constexpr const char* Logger<LogPolicyT>::_basename(const char* beg, const char* end) const {
  return (end >= beg && *end != '/') ? _basename(beg, end - 1) : (end + 1);
}

// Function: _basename
// Compile-time finding of a file name.
template <typename LogPolicyT>
constexpr const char* Logger<LogPolicyT>::_basename(const char* fpath) const {
  return _basename(fpath, _strend(fpath));
}

// Procedure: redir
template <typename LogPolicyT>
void Logger<LogPolicyT>::redir(const std::string& fpath) {
  _policy.open(fpath); 
}

// Function:_get_header
template <typename LogPolicyT>
template <LoggingSeverity severity>
void Logger<LogPolicyT>::_append_header(std::ostringstream& oss, const char* fpath, const int line) {
  
  // Append the severity prefix.
  _append_severity<severity>(oss);

  // Append the process id
  //oss << std::setfill(' ') << std::setw(5)
  //    << static_cast<unsigned short>(::getpid()) << std::setfill('0') << '|';

  // Append the thread id.
  oss << std::setw(5) << static_cast<unsigned short>(_gettid()) << ' ';
  
  // Insert timeinfo
  auto rt = std::time(nullptr);
  auto lt = *std::localtime(&rt);
  oss << std::put_time(&lt, "%Y-%m-%d %T ");
  oss << _basename(fpath) << ":" << line << "] ";
}

template <typename LogPolicyT>
template <LoggingSeverity severity>
void Logger<LogPolicyT>::_append_severity(std::ostringstream& oss) {

  if constexpr(severity == LoggingSeverity::DEBUG) {
    oss << DEBUG_COLOR << "D ";
  }
  else if constexpr(severity == LoggingSeverity::WARNING) {
    oss << WARNING_COLOR << "W ";
  }
  else if constexpr(severity == LoggingSeverity::ERROR) {
    oss << ERROR_COLOR << "E "; 
  }
  else if constexpr(severity == LoggingSeverity::FATAL) {
    oss << FATAL_COLOR << "F ";
  }
  else {
    oss << "I ";
  }
}

// Procedure: write
template<typename LogPolicyT>
template<LoggingSeverity severity, typename...ArgsT>
void Logger<LogPolicyT>::write(const char* fpath, const int line, ArgsT&&... args) {

  std::ostringstream oss;
  
  // Append the header into the ostringstream.
  _append_header<severity>(oss, fpath, line);

  // Append the given message to the ostringstream.
  (oss << ... << args);
  
  if constexpr(severity != LoggingSeverity::INFO) {
    oss << RESET_COLOR;
  }
  
  // Write to the device.
  _policy.write(oss.str());
}

// Procedure: debug
template <typename LogPolicyT>
template <typename... ArgsT>
void Logger<LogPolicyT>::debug(const char* fpath, const int line, ArgsT&&... args) {
  write<LoggingSeverity::DEBUG>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: info
template <typename LogPolicyT>
template <typename... ArgsT>
void Logger<LogPolicyT>::info(const char* fpath, const int line, ArgsT&&... args) {
  write<LoggingSeverity::INFO>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: warning
template <typename LogPolicyT>
template <typename... ArgsT>
void Logger<LogPolicyT>::warning(const char* fpath, const int line, ArgsT&&... args) {
  write<LoggingSeverity::WARNING>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: error
template <typename LogPolicyT>
template <typename... ArgsT>
void Logger<LogPolicyT>::error(const char* fpath, const int line, ArgsT&&... args) {
  write<LoggingSeverity::ERROR>(fpath, line, std::forward<ArgsT>(args)...);
}

// Procedure: fatal
// Log a fatal message and generate the backtrace symbols.
template <typename LogPolicyT>
template <typename... ArgsT>
void Logger<LogPolicyT>::fatal(const char* fpath, const int line, ArgsT&&... args) {
  write<LoggingSeverity::FATAL>(fpath, line, std::forward<ArgsT>(args)...);
  std::abort();
}

//-------------------------------------------------------------------------------------------------

// Atomic cout
template <typename... Ts>
auto& cout(Ts&&... args) {
  std::ostringstream oss;
  (oss << ... << args); 
  return (std::cout << oss.str());
}

// Atomic cerr
template <typename... Ts>
auto& cerr(Ts&&... args) {
  std::ostringstream oss;
  (oss << ... << args);
  return (std::cerr << oss.str());
}


};  // End of namespace dtc. --------------------------------------------------------------


#endif









