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

#ifndef DTC_ERROR_HPP_
#define DTC_ERROR_HPP_

#include <string>
#include <system_error>
#include <csignal>

namespace dtc {

/*// Class: TaskInfoCategory
// The error category that describes the task informaction. 
class TaskInfoCategory : public std::error_category {

  private:

    TaskInfoCategory() = default;

  public:

    enum Code : int {

      SUCCESS = 0,

      SIGNALED,


      // Signal error.

    };

    inline const char* name() const noexcept override final;
    inline static const std::error_category& get();
    
    std::string message(int) const override final;
};

// Function: name
inline const char* TaskInfoCategory::name() const noexcept {
  return "DtCraft TaskInfo";
}

// Function: get 
inline const std::error_category& TaskInfoCategory::get() {
  static TaskInfoCategory instance;
  return instance;
}

// Function: make_error_code
// Argument dependent lookup.
inline std::error_code make_error_code(TaskInfoCategory::Code e) {
  return std::error_code(static_cast<int>(e), TaskInfoCategory::get());
}

//-------------------------------------------------------------------------------------------------

// Class: Exception
// The exception class.
class Exception : public std::exception {

  public:

    Exception(const std::error_code&);
    
    const char* what() const noexcept override;
    const std::error_code& code() const noexcept;

  private:

    const std::error_code _errc;
};

//-------------------------------------------------------------------------------------------------

enum class Error : int {

  // Unknown error code.
  UNKNOWN = 1,

  // POSIX errno
  ADDRESS_FAMILY_NOT_SUPPORTED,
  ADDRESS_IN_USE,
  ADDRESS_NOT_AVAILABLE,
  ALREADY_CONNECTED,
  ARGUMENT_LIST_TOO_LONG,
  ARGUMENT_OUT_OF_DOMAIN,
  BAD_ADDRESS,
  BAD_FILE_DESCRIPTOR,
  BAD_MESSAGE,
  BROKEN_PIPE,
  CONNECTION_ABORTED,
  CONNECTION_ALREADY_IN_PROGRESS,
  CONNECTION_REFUSED,
  CONNECTION_RESET,
  CROSS_DEVICE_LINK,
  DESTINATION_ADDRESS_REQUIRED,
  DEVICE_OR_RESOURCE_BUSY,
  DIRECTORY_NOT_EMPTY,
  EXECUTABLE_FORMAT_ERROR,
  FILE_EXISTS,
  FILE_TOO_LARGE,
  FILENAME_TOO_LONG,
  FUNCTION_NOT_SUPPORTED,
  HOST_UNREACHABLE,
  IDENTIFIER_REMOVED,
  ILLEGAL_BYTE_SEQUENCE,
  INAPPROPRIATE_IO_CONTROL_OPERATION,
  INTERRUPTED,
  INVALID_ARGUMENT,
  INVALID_SEEK,
  IO_ERROR,
  IS_A_DIRECTORY,
  MESSAGE_SIZE_TOO_LONG,
  NETWORK_DOWN,
  NETWORK_RESET,
  NETWORK_UNREACHABLE,
  NO_BUFFER_SPACE,
  NO_CHILD_PROCESS,
  NO_LINK,
  NO_LOCK_AVAILABLE,
  NO_MESSAGE_AVAILABLE,
  NO_MESSAGE,
  NO_PROTOCOL_OPTION,
  NO_SPACE_ON_DEVICE,
  NO_STREAM_RESOURCES,
  NO_SUCH_DEVICE_OR_ADDRESS,
  NO_SUCH_DEVICE,
  NO_SUCH_FILE_OR_DIRECTORY,
  NO_SUCH_PROCESS,
  NOT_A_DIRECTORY,
  NOT_A_SOCKET,
  NOT_A_STREAM,
  NOT_CONNECTED,
  NOT_ENOUGH_MEMORY,
  NOT_SUPPORTED,
  OPERATION_CANCELED,
  OPERATION_IN_PROGRESS,
  OPERATION_NOT_PERMITTED,
  OWNER_DEAD,
  PERMISSION_DENIED,
  PROTOCOL_ERROR,
  PROTOCOL_NOT_SUPPORTED,
  READ_ONLY_FILE_SYSTEM,
  RESOURCE_DEADLOCK_WOULD_OCCUR,
  RESOURCE_UNAVAILABLE_TRY_AGAIN,
  RESULT_OUT_OF_RANGE,
  STATE_NOT_RECOVERABLE,
  STREAM_TIMEOUT,
  TEXT_FILE_BUSY,
  TIMED_OUT,
  TOO_MANY_FILES_OPEN_IN_SYSTEM,
  TOO_MANY_FILES_OPEN,
  TOO_MANY_LINKS,
  TOO_MANY_SYMBOLIC_LINK_LEVELS,
  VALUE_TOO_LARGE,
  WRONG_PROTOCOL_TYPE,

  // Signal.
  HANG_UP,
  TERMINAL_QUIT,
  ILLEGAL_INSTRUCTION,
  TRACE_TRAP,
  IOT_TRAP,
  BUS_ERROR,
  FLOATING_POINT_EXCEPTION,
  KILLED,
  INVALID_MEMORY_SEGMENT_ACCESS,
  ALARM_CLOCK,
  TERMINATED,
  STACK_FAULT,
  CHILD_STOPPED,
  RESUME,
  STOPPED,
  TERMINAL_STOP,
  CPU_LIMITED_EXCEEDED,
  FILE_SIZE_LIMITED_EXCEEDED
}; */

// Class: NetworkCategory
//class NetworkCategory : public std::error_category {
//
//  private:
//
//    NetworkCategory() = default;
//
//  public:
//
//    const char* name() const noexcept override final;
//    std::string message(int) const override final;
//    
//    static const std::error_category& get();
//};

/*// Make error code enum from system-specific errno
Error make_posix_error_code(int);
Error make_signal_error_code(int);
Error make_socket_error_code(int);

// Argument dependent lookup.
inline auto make_error_code(Error e) {
  return std::error_code(static_cast<int>(e), DtCraftCategory::get());
}

//inline auto make_error_condition(Error::Code e) {
//  return std::error_condition(static_cast<int>(e), Error::category()); 
//} */

inline std::error_code make_posix_error_code(int e) {
  return std::make_error_code(static_cast<std::errc>(e));
}

};  // End of namespace dtc. ----------------------------------------------------------------------

// Register for implicit conversion to dtc::Error::Code
namespace std {
 
  //template <>
  //struct is_error_code_enum<dtc::TaskInfoCategory::Code> : true_type {};

  //template <>
  //struct is_error_code_enum<dtc::Error> : true_type {};


};

#endif




