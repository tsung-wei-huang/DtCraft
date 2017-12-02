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

#ifndef DTC_DEVICE_HPP_
#define DTC_DEVICE_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility.hpp>

namespace dtc {

// Class: Device
class Device {
  
  friend class DeviceWriter;
  friend class DeviceReader;
  friend class OutputStreamBuffer;
  friend class InputStreamBuffer;

  protected:

    int _fd {-1};
  
  public:

    Device() = default;
    Device(int);
    
    virtual ~Device();

    virtual std::streamsize write(const void*, std::streamsize) = 0;
    virtual std::streamsize read(void*, std::streamsize) = 0;
    
    inline int fd() const;
    
    Device(const Device&) = delete;
    Device(Device&&);

    Device& operator = (const Device&) const = delete;
    Device& operator = (Device&&);

    void blocking(bool);
    void open_on_exec(bool);
};

// Function: fd
inline int Device::fd() const {
  return _fd;
}

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif





