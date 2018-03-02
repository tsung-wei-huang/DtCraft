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

#ifndef DTC_IPC_DOMAIN_HPP_
#define DTC_IPC_DOMAIN_HPP_

#include <dtc/device.hpp>
#include <dtc/utility/utility.hpp>
#include <dtc/concurrent/fifo.hpp>

// TODO:
//
// 1. Improve the function attach with c++17 emplace which returns the reference.
//

namespace dtc {

// Class: Domain
//
// The wrapper for a domain socket device. A Unix domain socket or IPC socket (inter-process 
// communication socket) is a data communications endpoint for exchanging data between processes 
// executing on the same host operating system.
//
class Domain : public Device {

  union ControlMessage {
    struct cmsghdr header;
    char buf[CMSG_SPACE(sizeof(int))];
  };
  
  public:

    Domain(const int);
    Domain(Domain&&) = delete;
    Domain(const Domain&) = delete;

    Domain& operator = (Domain&&) = delete;
    Domain& operator = (const Domain&) = delete;

    ~Domain();

    int pop();

    void close();
    void attach(const int);

  private:

    //ConcurrentFIFO <ControlMessage, 1024> _rcmsgs;
    //ConcurrentFIFO <ControlMessage, 1024> _wcmsgs;

    //std::streamsize read(void*, std::streamsize, std::error_code&) override final;
    //std::streamsize write(const void*, std::streamsize, std::error_code&) override final;
};

/*
// Function: make_domain_pair
inline std::tuple<std::shared_ptr<Domain>, std::shared_ptr<Domain>> make_domain_pair() {
  int fd[2];
  if(::socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1) {
    return {nullptr, nullptr};
  }
  return std::make_tuple(std::make_shared<Domain>(fd[0]), std::make_shared<Domain>(fd[1]));
}

//-------------------------------------------------------------------------------------------------

// Class: DomainReaderBase
class DomainReaderBase : public ReadEventBase {
  
  protected:

    InputStreamBuffer <Domain> _isbuf;
    std::istream _istream;

  public:
    
    DomainReaderBase(const std::shared_ptr<Domain>&);

    ~DomainReaderBase() = default;

    virtual void operator () (std::istream&) = 0;

  private:

    void operator () () override final;
    
};

//-------------------------------------------------------------------------------------------------

// Class: DomainWriterBase
class DomainWriterBase : public WriteEventBase {

  protected:

    OutputStreamBuffer<Domain> _osbuf;
    std::ostream _ostream;
  
  public:
    
    DomainWriterBase(const std::shared_ptr<Domain>&);
    ~DomainWriterBase() = default;

    virtual void operator () (std::ostream&) = 0;

  private: 

    void operator () () override final;
};
*/

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif

