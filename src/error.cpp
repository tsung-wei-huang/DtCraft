/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong   *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/error.hpp>

namespace dtc {

// Function: get
//const std::error_category& NetworkCategory::get() {
//  static NetworkCategory category;
//  return category;
//}

//-------------------------------------------------------------------------------------------------

/*std::string TaskInfoCategory::message(int code) const {
  switch(code) {
    
    case SUCCESS:
      return "success";
    break;

    default:
      return "undefined";
    break;
  };
}

//-------------------------------------------------------------------------------------------------

const char* DtCraftCategory::name() const noexcept {
  return "DtCraft";
}

std::string DtCraftCategory::message(int ev) const {

  switch(static_cast<Error>(ev)) {

    // POSIX-specific field.
    case Error::ADDRESS_FAMILY_NOT_SUPPORTED:
      return "address family not supported";
    break;

    case Error::ADDRESS_IN_USE:
      return "address already in use";
    break;

    case Error::ADDRESS_NOT_AVAILABLE:
      return "address not available";
    break;

    case Error::ALREADY_CONNECTED:
      return "socket is connected";
    break;

    case Error::ARGUMENT_LIST_TOO_LONG:
      return "argument list too long";
    break;

    case Error::ARGUMENT_OUT_OF_DOMAIN:
      return "mathematics argument out of domain of function";
    break;

    case Error::BAD_ADDRESS:
      return "bad address";
    break;

    case Error::BAD_FILE_DESCRIPTOR:
      return "bad file descriptor";
    break;

    case Error::BAD_MESSAGE:
      return "bad message";
    break;

    case Error::BROKEN_PIPE:
      return "broken pipe";
    break;
    
    case Error::CONNECTION_ABORTED:
      return "connection aborted by network";
    break;
    
    case Error::CONNECTION_ALREADY_IN_PROGRESS:
      return "connection already in progress";
    break;

    case Error::CONNECTION_REFUSED:
      return "connection refused";
    break;
    
    case Error::CONNECTION_RESET:
      return "connection reset";
    break;
    
    case Error::CROSS_DEVICE_LINK:
      return "cross device link";
    break;

    case Error::DESTINATION_ADDRESS_REQUIRED:
      return "destination address required";
    break;

    case Error::DEVICE_OR_RESOURCE_BUSY:
      return "device or resource busy";
    break;

    case Error::DIRECTORY_NOT_EMPTY:
      return "directory not empty";
    break;

    case Error::EXECUTABLE_FORMAT_ERROR:
      return "executable format error";
    break;

    case Error::FILE_EXISTS:
      return "file exists";
    break;

    case Error::FILE_TOO_LARGE:
      return "file too large";
    break;
    
    case Error::FILENAME_TOO_LONG:
      return "filename too long";
    break;
    
    case Error::FUNCTION_NOT_SUPPORTED:
      return "function not implemented";
    break;

    case Error::HOST_UNREACHABLE:
      return "host is unreachable";
    break;
    
    case Error::IDENTIFIER_REMOVED:
      return "identifier removed";
    break;

    case Error::ILLEGAL_BYTE_SEQUENCE:
      return "invalid or incomplete multibyte or wide character";
    break;

    case Error::INAPPROPRIATE_IO_CONTROL_OPERATION:
      return "inappropriate I/O control operation";
    break;

    case Error::INTERRUPTED:
      return "interrupted";
    break;
    
    case Error::INVALID_ARGUMENT:
      return "invalid argument";
    break;
    
    case Error::INVALID_SEEK:
      return "invalid seek";
    break;

    case Error::IO_ERROR:
      return "input/output error";
    break;
    
    case Error::IS_A_DIRECTORY:
      return "is a directory";
    break;
    
    case Error::MESSAGE_SIZE_TOO_LONG:
      return "message size too long";
    break;
    
    case Error::NETWORK_DOWN:
      return "network is down";
    break;

    case Error::NETWORK_RESET:
      return "connection aborted by network";
    break;
    
    case Error::NETWORK_UNREACHABLE:
      return "network unreachable";
    break;
      
    case Error::NO_BUFFER_SPACE:
      return "no buffer space available";
    break;
      
    case Error::NO_CHILD_PROCESS:
      return "no child process";
    break;

    case Error::NO_LINK:
      return "link has been served";
    break; 
      
    case Error::NO_LOCK_AVAILABLE:
      return "no locks available";
    break;

    case Error::NO_MESSAGE_AVAILABLE:
      return "no message is available on the STREAM head read queue";
    break;

    case Error::NO_MESSAGE:
      return "no message of the desired type";
    break;
    
    case Error::NO_PROTOCOL_OPTION:
      return "protocol not available";
    break;
    
    case Error::NO_SPACE_ON_DEVICE:
      return "no space left on device";
    break;
    
    case Error::NO_STREAM_RESOURCES:
      return "no stream resources";
    break;
      
    case Error::NO_SUCH_DEVICE_OR_ADDRESS:
      return "no such device or address";
    break;
      
    case Error::NO_SUCH_DEVICE:
      return "no such device";
    break;
    
    case Error::NO_SUCH_FILE_OR_DIRECTORY:
      return "no such file or directory";
    break;

    case Error::NO_SUCH_PROCESS:
      return "no such process";
    break;

    case Error::NOT_A_DIRECTORY:
      return "not a directory";
    break;

    case Error::NOT_A_SOCKET:
      return "not a socket";
    break;
    
    case Error::NOT_A_STREAM:
      return "not a stream";
    break;

    case Error::NOT_CONNECTED:
      return "not connected";
    break;

    case Error::NOT_ENOUGH_MEMORY:
      return "not enough memory (malloc, calloc, new, etc.)";
    break;

    case Error::NOT_SUPPORTED:
      return "operation not supported";
    break;

    case Error::OPERATION_CANCELED:
      return "operation canceled";
    break;

    case Error::OPERATION_IN_PROGRESS:
      return "operation in progress";
    break;
    
    case Error::OPERATION_NOT_PERMITTED:
      return "operation not permitted";
    break;
    
    case Error::OWNER_DEAD:
      return "owner is dead";
    break;

    case Error::PERMISSION_DENIED:
      return "permission denied";
    break;

    case Error::PROTOCOL_ERROR:
      return "protocol error";
    break;
      
    case Error::PROTOCOL_NOT_SUPPORTED:
      return "protocol not supported";
    break;

    case Error::READ_ONLY_FILE_SYSTEM:
      return "read only file system";
    break;
    
    case Error::RESOURCE_DEADLOCK_WOULD_OCCUR:
      return "resource deadlock would occur";
    break;

    case Error::RESOURCE_UNAVAILABLE_TRY_AGAIN:
      return "resource temporarily unavailabe (try again later)";
    break;
    
    case Error::RESULT_OUT_OF_RANGE:
      return "result out of range";
    break;
    
    case Error::STATE_NOT_RECOVERABLE:
      return "state not recoverable";
    break;

    case Error::STREAM_TIMEOUT:
      return "timer expired";
    break;
    
    case Error::TEXT_FILE_BUSY:
      return "text file busy";
    break;
    
    case Error::TIMED_OUT:
      return "timed out";
    break;
    
    case Error::TOO_MANY_FILES_OPEN_IN_SYSTEM:
      return "too many open files in system (/proc/sys/fs/file-max)";
    break;
    
    case Error::TOO_MANY_FILES_OPEN:
      return "too many files open (resource limit exceeded RLIMIT_NOFILE)";
    break;
    
    case Error::TOO_MANY_LINKS:
      return "too many links";
    break;
    
    case Error::TOO_MANY_SYMBOLIC_LINK_LEVELS:
      return "too many levels of symbolic links";
    break;
    
    case Error::VALUE_TOO_LARGE:
      return "value too large to be stored in data type";
    break;
    
    case Error::WRONG_PROTOCOL_TYPE:
      return "wrong protocol type";
    break;

    // Signal 
    case Error::HANG_UP:
      return "hang up";
    break;

    case Error::TERMINAL_QUIT:
      return "terminal interrupt";
    break;

    case Error::ILLEGAL_INSTRUCTION:
      return "illegal instruction";
    break;

    case Error::TRACE_TRAP:
      return "trace trap";
    break;

    case Error::IOT_TRAP:
      return "iot trap";
    break;

    case Error::BUS_ERROR:
      return "bus error";
    break;

    case Error::FLOATING_POINT_EXCEPTION:
      return "floating point exception";
    break;

    case Error::KILLED:
      return "killed";
    break;

    case Error::INVALID_MEMORY_SEGMENT_ACCESS:
      return "segmentation fault";
    break;

    case Error::ALARM_CLOCK:
      return "alarm clock";
    break;

    case Error::TERMINATED:
      return "terminated";
    break;

    case Error::STACK_FAULT:
      return "stack fault";
    break;

    case Error::CHILD_STOPPED:
      return "child process has stopped or existed, changed"; 
    break;

    case Error::RESUME:
      return "resumed";
    break;

    case Error::STOPPED:
      return "stop executing (can't be caught or ignored)";
    break;

    case Error::TERMINAL_STOP:
      return "terminal stopped";
    break;

    case Error::CPU_LIMITED_EXCEEDED:
      return "cpu limit exceeded";
    break;

    case Error::FILE_SIZE_LIMITED_EXCEEDED:
      return "file size limit exceeded";
    break;
    
    default:
      return "unknown error";
    break;
  };
}

// TODO:
Error make_socket_error_code(int no) {

  return Error::UNKNOWN;
}

Error make_signal_error_code(int sig) {
  
  switch(sig) {

    case SIGHUP:
      return Error::HANG_UP;
    break;

    case SIGINT:
      return Error::INTERRUPTED;
    break;

    case SIGQUIT:
      return Error::TERMINAL_QUIT;
    break;

    case SIGILL:
      return Error::ILLEGAL_INSTRUCTION;
    break;

    case SIGTRAP:
      return Error::TRACE_TRAP;
    break;

    case SIGIOT:
      return Error::IOT_TRAP;
    break;

    case SIGBUS:
      return Error::BUS_ERROR;
    break;

    case SIGFPE:
      return Error::FLOATING_POINT_EXCEPTION;
    break;

    case SIGKILL:
      return Error::KILLED;
    break;

    case SIGSEGV:
      return Error::INVALID_MEMORY_SEGMENT_ACCESS;
    break;
    
    case SIGPIPE:
      return Error::BROKEN_PIPE;
    break;

    case SIGALRM:
      return Error::ALARM_CLOCK;
    break;

    case SIGTERM:
      return Error::TERMINATED;
    break;

    case SIGSTKFLT:
      return Error::STACK_FAULT;
    break;

    case SIGCHLD:
      return Error::CHILD_STOPPED;
    break;

    case SIGCONT:
      return Error::RESUME;
    break;

    case SIGSTOP:
      return Error::STOPPED;
    break;

    case SIGTSTP:
      return Error::TERMINAL_STOP;
    break;

    case SIGXCPU:
      return Error::CPU_LIMITED_EXCEEDED;
    break;

    case SIGXFSZ:
      return Error::FILE_SIZE_LIMITED_EXCEEDED;
    break;

    default:
      return Error::UNKNOWN;
    break;
  };

}

Error make_posix_error_code(int no) {

  switch(no) {

    case EAFNOSUPPORT:
      return Error::ADDRESS_FAMILY_NOT_SUPPORTED;
    break;

    case EADDRINUSE:
      return Error::ADDRESS_IN_USE;
    break;

    case EADDRNOTAVAIL:
      return Error::ADDRESS_NOT_AVAILABLE;
    break;

    case EISCONN:
      return Error::ALREADY_CONNECTED;
    break;

    case E2BIG:
      return Error::ARGUMENT_LIST_TOO_LONG;
    break;

    case EDOM:
      return Error::ARGUMENT_OUT_OF_DOMAIN;
    break;

    case EFAULT:
      return Error::BAD_ADDRESS;
    break;

    case EBADF:
      return Error::BAD_FILE_DESCRIPTOR;
    break;
    
    case EBADMSG:
      return Error::BAD_MESSAGE;
    break;

    case EPIPE:
      return Error::BROKEN_PIPE;
    break;

    case ECONNABORTED:
      return Error::CONNECTION_ABORTED;
    break;
    
    case EALREADY:
      return Error::CONNECTION_ALREADY_IN_PROGRESS;
    break;

    case ECONNREFUSED:
      return Error::CONNECTION_REFUSED;
    break;

    case ECONNRESET:
      return Error::CONNECTION_RESET;
    break;

    case EXDEV:
      return Error::CROSS_DEVICE_LINK;
    break;

    case EDESTADDRREQ:
      return Error::DESTINATION_ADDRESS_REQUIRED;
    break;

    case EBUSY:
      return Error::DEVICE_OR_RESOURCE_BUSY;
    break;

    case ENOTEMPTY:
      return Error::DIRECTORY_NOT_EMPTY;
    break;

    case ENOEXEC:
      return Error::EXECUTABLE_FORMAT_ERROR;
    break;

    case EEXIST:
      return Error::FILE_EXISTS;
    break;

    case EFBIG:
      return Error::FILE_TOO_LARGE;
    break;

    case ENAMETOOLONG:
      return Error::FILENAME_TOO_LONG;
    break;

    case ENOSYS:
      return Error::FUNCTION_NOT_SUPPORTED;
    break;

    case EHOSTUNREACH:
      return Error::HOST_UNREACHABLE;
    break;

    case EIDRM:
      return Error::IDENTIFIER_REMOVED;
    break;

    case EILSEQ:
      return Error::ILLEGAL_BYTE_SEQUENCE;
    break;

    case ENOTTY:
      return Error::INAPPROPRIATE_IO_CONTROL_OPERATION;
    break;

    case EINTR:
      return Error::INTERRUPTED;
    break;

    case EINVAL:
      return Error::INVALID_ARGUMENT;
    break;

    case ESPIPE:
      return Error::INVALID_SEEK;
    break;
    
    case EIO:
      return Error::IO_ERROR;
    break;

    case EISDIR:
      return Error::IS_A_DIRECTORY;
    break;

    case EMSGSIZE:
      return Error::MESSAGE_SIZE_TOO_LONG;
    break;

    case ENETDOWN:
      return Error::NETWORK_DOWN;
    break;

    case ENETRESET:
      return Error::NETWORK_RESET;
    break;

    case ENETUNREACH:
      return Error::NETWORK_UNREACHABLE;
    break;

    case ENOBUFS:
      return Error::NO_BUFFER_SPACE;
    break;

    case ECHILD:
      return Error::NO_CHILD_PROCESS;
    break;

    case ENOLINK:
      return Error::NO_LINK;
    break;
    
    case ENOLCK:
      return Error::NO_LOCK_AVAILABLE;
    break;

    case ENODATA:
      return Error::NO_MESSAGE_AVAILABLE;
    break;

    case ENOMSG:
      return Error::NO_MESSAGE;
    break;

    case ENOPROTOOPT:
      return Error::NO_PROTOCOL_OPTION;
    break;

    case ENOSPC:
      return Error::NO_SPACE_ON_DEVICE;
    break;

    case ENOSR:
      return Error::NO_STREAM_RESOURCES;
    break;

    case ENXIO:
      return Error::NO_SUCH_DEVICE_OR_ADDRESS;
    break;

    case ENODEV:
      return Error::NO_SUCH_DEVICE;
    break;

    case ENOENT:
      return Error::NO_SUCH_FILE_OR_DIRECTORY;
    break;

    case ESRCH:
      return Error::NO_SUCH_PROCESS;
    break;

    case ENOTDIR:
      return Error::NOT_A_DIRECTORY;
    break;

    case ENOTSOCK:
      return Error::NOT_A_SOCKET;
    break;

    case ENOSTR:
      return Error::NOT_A_STREAM;
    break;

    case ENOTCONN:
      return Error::NOT_CONNECTED;
    break;

    case ENOMEM:
      return Error::NOT_ENOUGH_MEMORY;
    break;

    case ENOTSUP:
    #if EOPNOTSUPP != ENOTSUP
    case EOPNOTSUPP:
    #endif
      return Error::NOT_SUPPORTED;
    break;

    case ECANCELED:
      return Error::OPERATION_CANCELED;
    break;

    case EINPROGRESS:
      return Error::OPERATION_IN_PROGRESS;
    break;

    case EPERM:
      return Error::OPERATION_NOT_PERMITTED;
    break;

    case EOWNERDEAD:
      return Error::OWNER_DEAD;
    break;

    case EACCES:
      return Error::PERMISSION_DENIED;
    break;

    case EPROTO:
      return Error::PROTOCOL_ERROR;
    break;

    case EPROTONOSUPPORT:
      return Error::PROTOCOL_NOT_SUPPORTED;
    break;

    case EROFS:
      return Error::READ_ONLY_FILE_SYSTEM;
    break;

    case EDEADLK:
      return Error::RESOURCE_DEADLOCK_WOULD_OCCUR;
    break;

    case EAGAIN:
    #if EWOULDBLOCK!=EAGAIN
    case EWOULDBLOCK:
    #endif
      return Error::RESOURCE_UNAVAILABLE_TRY_AGAIN;
    break;

    case ERANGE:
      return Error::RESULT_OUT_OF_RANGE;
    break;

    case ENOTRECOVERABLE:
      return Error::STATE_NOT_RECOVERABLE;
    break;

    case ETIME:
      return Error::STREAM_TIMEOUT;
    break;

    case ETXTBSY:
      return Error::TEXT_FILE_BUSY;
    break;

    case ETIMEDOUT:
      return Error::TIMED_OUT;
    break;

    case ENFILE:
      return Error::TOO_MANY_FILES_OPEN_IN_SYSTEM;
    break;

    case EMFILE:
      return Error::TOO_MANY_FILES_OPEN;
    break;

    case EMLINK:
      return Error::TOO_MANY_LINKS;
    break;

    case ELOOP:
      return Error::TOO_MANY_SYMBOLIC_LINK_LEVELS;
    break;

    case EOVERFLOW:
      return Error::VALUE_TOO_LARGE;
    break;

    case EPROTOTYPE:
      return Error::WRONG_PROTOCOL_TYPE;
    break;

    default:
      return Error::UNKNOWN;
    break;

  };
}

const std::error_category& DtCraftCategory::get() {
  static DtCraftCategory instance;
  return instance;
}  */

//-------------------------------------------------------------------------------------------------

};  // End of namespace dtc. ----------------------------------------------------------------------




