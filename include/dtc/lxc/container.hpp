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

#ifndef DTC_LXC_CONTAINER_HPP_
#define DTC_LXC_CONTAINER_HPP_

#include <dtc/lxc/cgroup.hpp>
#include <dtc/ipc/ipc.hpp>
#include <dtc/protobuf/protobuf.hpp>

namespace dtc {

// Class: Container
class Container {

  struct ChildArgument {
    const pb::Topology& topology;
    std::shared_ptr<Socket> sync[2];
  };

  public:
    
    Container(const std::filesystem::path&);
    Container(const Container&) = delete;
    Container(Container&&);
    
    ~Container();

    Container& operator = (const Container&) = delete;
    Container& operator = (Container&&);

    bool alive() const;

    inline pid_t pid() const;
    inline int status() const;
    inline const ControlGroup& cgroup() const;
    
    void spawn(const pb::Topology&);
    void kill();
    void wait();

  private:
    
    pid_t _pid {-1};

    int _status {-1};

    std::unique_ptr<char[]> _stack;

    ControlGroup _cgroup;

    static int _entrypoint(void*);
};

// Function: pid
inline pid_t Container::pid() const {
  return _pid;
}

// Function: status
inline int Container::status() const {
  return _status;
}

// Function: cgroup
inline const ControlGroup& Container::cgroup() const {
  return _cgroup;
}

}; // End of namespace dtc. -----------------------------------------------------------------------


#endif

