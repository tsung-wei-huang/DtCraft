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

//namespace dtc {

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

  public:
    
    inline Policy* policy();
    inline const Policy* policy() const;

    //inline bool is_console() const;
    
    void open(const std::string& name);
    void close();
    void write(const std::string& msg);

};

// Function: policy
template <typename Policy>
inline Policy* LogPolicyInterface<Policy>::policy() {
  return static_cast<Policy*>(this);
}

// Function: policy
template <typename Policy>
inline const Policy* LogPolicyInterface<Policy>::policy() const {
  return static_cast<const Policy*>(this);
}

// Function: is_console
//template <typename Policy>
//inline bool LogPolicyInterface<Policy>::is_console() const {
//  return _os == &std::cerr || _os == &std::cout;
//}
    
// Procedure: open
template <typename Policy>
inline void LogPolicyInterface<Policy>::open(const std::string& name) {
  return policy()->_open(name);
}

// Procedure: close
template <typename Policy>
inline void LogPolicyInterface<Policy>::close() {
  return policy()->_close();
}

// Procedure: write
template <typename Policy>
inline void LogPolicyInterface<Policy>::write(const std::string& msg) {
  return policy()->_write(msg);
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
  
    //static constexpr const char* _set_red     {"\033[1;31m"};
    //static constexpr const char* _set_green   {"\033[1;32m"};
    //static constexpr const char* _set_yellow  {"\033[1;33m"};
    //static constexpr const char* _set_cyan    {"\033[1;36m"};
    //static constexpr const char* _set_magenta {"\033[1;35m"};
    //static constexpr const char* _reset_color {"\033[0m"   };
    
    LogPolicyT _policy;

    //std::atomic_flag _lock = ATOMIC_FLAG_INIT;

  public:

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

    template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::DEBUG>* = nullptr>
    void _append_severity(std::ostringstream&);
    
    template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::INFO>* = nullptr>
    void _append_severity(std::ostringstream&);
    
    template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::WARNING>* = nullptr>
    void _append_severity(std::ostringstream&);
    
    template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::ERROR>* = nullptr>
    void _append_severity(std::ostringstream&);
    
    template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::FATAL>* = nullptr>
    void _append_severity(std::ostringstream&);
    
    template <typename... ArgsT>
    void _append_message(std::ostringstream&, ArgsT&&...);
    
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
template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::DEBUG>*>
void Logger<LogPolicyT>::_append_severity(std::ostringstream& oss) {
  //if(_policy.is_console()) {
  //  oss << _set_cyan;
  //}
  oss << "D ";
}

template <typename LogPolicyT>    
template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::INFO>*>
void Logger<LogPolicyT>::_append_severity(std::ostringstream& oss) {
  oss << "I ";
}

template <typename LogPolicyT>    
template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::WARNING>*>
void Logger<LogPolicyT>::_append_severity(std::ostringstream& oss) {
  //if(_policy.is_console()) {
  //  oss << _set_yellow;
  //}
  oss << "W ";
}

template <typename LogPolicyT>    
template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::ERROR>*>
void Logger<LogPolicyT>::_append_severity(std::ostringstream& oss) {
  //if(_policy.is_console()) {
  //  oss << _set_red;
  //}
  oss << "X ";
}

template <typename LogPolicyT>    
template <LoggingSeverity s, std::enable_if_t<s==LoggingSeverity::FATAL>*>
void Logger<LogPolicyT>::_append_severity(std::ostringstream& oss) {
  //if(_policy.is_console()) {
  //  oss << _set_red;
  //}
  oss << "! ";
}

template <typename LogPolicyT>
template <typename... ArgsT>
void Logger<LogPolicyT>::_append_message(std::ostringstream& oss, ArgsT&&... args) {
  (oss << ... << args) ;
}

// Procedure: write
template< typename LogPolicyT >
template< LoggingSeverity severity , typename...ArgsT >
void Logger<LogPolicyT>::write(const char* fpath, const int line, ArgsT&&... args) {

  std::ostringstream oss;
  
  // Clear the ostringstream and clear any possible failing bits.
  oss.str("");
  oss.clear();

  // Append the header into the ostringstream.
  _append_header<severity>(oss, fpath, line);

  // Append the given message to the ostringstream.
  _append_message(oss, std::forward<ArgsT>(args)...);

  // Append the reset color.
  //if(_policy.is_console()) oss << _reset_color;
  
  // Write to the device (under lock).
  //while(_lock.test_and_set(std::memory_order_acquire));
  _policy.write(oss.str());
  //_lock.clear(std::memory_order_release);
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

// Global declaration and macro usage.
static Logger<FileLogPolicy> logger;

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

#define LOGD_IF(...) if(LOG_GET_FIRST(__VA_ARGS__)) {           \
                       LOGD(LOG_REMOVE_FIRST(__VA_ARGS__));     \
                     }

#define LOGI_IF(...) if(LOG_GET_FIRST(__VA_ARGS__)) {           \
                       LOGI(LOG_REMOVE_FIRST(__VA_ARGS__));     \
                     }

#define LOGW_IF(...) if(LOG_GET_FIRST(__VA_ARGS__)) {           \
                       LOGW(LOG_REMOVE_FIRST(__VA_ARGS__));     \
                     }

#define LOGE_IF(...) if(LOG_GET_FIRST(__VA_ARGS__)) {           \
                       LOGE(LOG_REMOVE_FIRST(__VA_ARGS__));     \
                     }

#define LOGF_IF(...) if(LOG_GET_FIRST(__VA_ARGS__)) {           \
                       LOGF(LOG_REMOVE_FIRST(__VA_ARGS__));     \
                     }

//};  // End of namespace dtc. --------------------------------------------------------------


#endif









