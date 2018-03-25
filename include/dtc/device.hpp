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

#ifndef DTC_DEVICE_HPP_
#define DTC_DEVICE_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility/utility.hpp>
#include <dtc/static/logger.hpp>

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

    Device(int);
    
    virtual ~Device();

    std::streamsize read(void*, std::streamsize) const;
    std::streamsize purge(void*, std::streamsize) const;
    std::streamsize write(const void*, std::streamsize) const;
    std::streamsize flush(const void*, std::streamsize) const;
    
    inline int fd() const;
    
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    Device& operator = (const Device&) const = delete;
    Device& operator = (Device&&) = delete;

    Device& blocking(bool);
    Device& open_on_exec(bool);
};

// Function: fd
inline int Device::fd() const {
  return _fd;
}

// ------------------------------------------------------------------------------------------------
  
// Class: ScopedOpenOnExec
class ScopedOpenOnExec {

  public:

    ScopedOpenOnExec(std::shared_ptr<Device>);
    ~ScopedOpenOnExec();
    
    ScopedOpenOnExec() = delete;
    ScopedOpenOnExec(const ScopedOpenOnExec&) = delete;
    ScopedOpenOnExec(ScopedOpenOnExec&& rhs);

    ScopedOpenOnExec& operator = (ScopedOpenOnExec&&) = delete;
    ScopedOpenOnExec& operator = (const ScopedOpenOnExec&) = delete;

  private:

    std::shared_ptr<Device> _device;
};

// ------------------------------------------------------------------------------------------------

// Class: ScopedDeviceRestorer
class ScopedDeviceRestorer {

  public:

    ScopedDeviceRestorer(std::shared_ptr<Device>);
    ~ScopedDeviceRestorer();
    
    ScopedDeviceRestorer() = delete;
    ScopedDeviceRestorer(const ScopedDeviceRestorer&) = delete;
    ScopedDeviceRestorer(ScopedDeviceRestorer&& rhs);

    ScopedDeviceRestorer& operator = (ScopedDeviceRestorer&&) = delete;
    ScopedDeviceRestorer& operator = (const ScopedDeviceRestorer&) = delete;

  private:

    std::shared_ptr<Device> _device;
};

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif





