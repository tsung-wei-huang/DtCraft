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

#ifndef DTC_IPC_SHM_HPP_
#define DTC_IPC_SHM_HPP_

#include <dtc/ipc/device.hpp>

namespace dtc {

// Class: SharedMemory
class SharedMemory : public Device {

  public:
    
    using fifo_type = std::array<char, BUFSIZ>;
    
    SharedMemory(int);
    SharedMemory(SharedMemory&&) = delete;
    SharedMemory(const SharedMemory&) = delete;
    
    SharedMemory& operator = (SharedMemory&&) = delete;
    SharedMemory& operator = (const SharedMemory&) = delete;
  
    ~SharedMemory();
    
    std::streamsize read(void*, std::streamsize) override final;
    std::streamsize write(const void*, std::streamsize) override final;
  
  private:  
    
    //std::array<char, BUFSIZ> _fifo;
    std::unique_ptr<fifo_type, std::function<void(fifo_type*)>> _fifo {_allocate()};
    std::streamsize _w_ptr {1};
    std::streamsize _r_ptr {0};
    std::streamsize _bufcpy(void*, const void*, std::streamsize);

    bool _notify();
    
    static std::unique_ptr<fifo_type, std::function<void(fifo_type*)>> _allocate();
};

// Function: _bufcpy
inline std::streamsize SharedMemory::_bufcpy(void* to , const void* from , std::streamsize num){
  std::memcpy(to, from, num);
  return num;
}


// Function: make_shared_memory
std::shared_ptr<SharedMemory> make_shared_memory();


};  // End of namespace dtc. --------------------------------------------------------------



#endif




