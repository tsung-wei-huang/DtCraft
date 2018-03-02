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

#ifndef DTC_IPC_PIPE_HPP_
#define DTC_IPC_PIPE_HPP_

#include <dtc/device.hpp>

namespace dtc {

// Class: Pipe
class Pipe : public Device {
  
  public:
    
    template <typename... Ts>
    Pipe(Ts&&...);

    Pipe(Pipe&&) = delete;
    Pipe(const Pipe&) = delete;
    
    Pipe& operator = (Pipe&&) = delete;
    Pipe& operator = (const Pipe&) = delete;
  
    ~Pipe() = default;
};

// Constructor
template <typename... Ts>
Pipe::Pipe(Ts&&... ts) : Device {std::forward<Ts>(ts)...} {}

// Function: make_pipe
std::tuple<std::shared_ptr<Pipe>, std::shared_ptr<Pipe>> make_pipe();

// Function: make_sync_pipe
std::tuple<std::shared_ptr<Pipe>, std::shared_ptr<Pipe>> make_sync_pipe();

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif





