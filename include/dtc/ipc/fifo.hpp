/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_IPC_FIFO_HPP_
#define DTC_IPC_FIFO_HPP_

#include <dtc/ipc/device.hpp>

namespace dtc {
/*
// Class: FIFO
//
// A FIFO special file (a named pipe) is similar to a pipe, except that it is accessed as part of 
// the filesystem. It can be opened by multiple processes for reading or writing. When processes 
// are exchanging data via the FIFO, the kernel passes all data internally without writing it to 
// the filesystem. Thus, the FIFO special file has no contents on the filesystem; the filesystem 
// entry merely serves as a reference point so that processes can access the pipe using a name in 
// the filesystem.
//
// The kernel maintains exactly one pipe object for each FIFO special file that is opened by at 
// least one process. The FIFO must be opened on both ends (reading and writing) before data can 
// be passed. Normally, opening the FIFO blocks until the other end is opened also.
//
// A process can open a FIFO in non-blocking mode. In this case, opening for read only will succeed
// even if none has opened on the write side yet; opening for write only will fail with ENXIO 
// (no such device or address) unless the other end has already been opened.
//
// Under Linux, opening a FIFO for read and write will succeed both in blocking and non-blocking 
// mode. POSIX leaves this behaviour undefined. This can be used to open a FIFO for writing while 
// there are no readers available. A process that uses both ends of the connection in order to 
// communicate with itself should be very careful to avoid deadlocks.  
//
class FIFO : public DeviceIX <FIFO> {

  friend class DeviceIX <FIFO>;

  private:

    int _fd {-1};

  public:

    FIFO();
    ~FIFO();

    // ACCESSOR
    DTC_ACCESSOR(fd)

    // Procedure: is_open
    //
    // Query the status of the underlying fifo descriptor.
    //
    // @return: true if the underlying fifo descriptor has been successfully opened; 
    //          false otherwise.
    //
    bool is_open() const;
    
    // Procedure: open
    //
    // Open the fifo given the open mode.
    //
    // param1: file path to the fifio
    // param2: open mode (should be in or out only)
    //
    void open(const std::string&, const std::ios_base::openmode);

    // Procedure: close
    //
    // Close the underlying fifo. The procedure only close the file descriptor. It doesn't
    // unlink the fifo name.
    //
    void close();

  private:

    // Function: _read
    //
    // Perform iterative read and extract the data from the underlying device to the streambuffer.
    // The implementation will automatically set the iostate of the underlying device.
    //
    //   - eof : disconnected
    //   - fail: unexpected error (with -1 returned)
    //   - good: otherwise
    //
    // @param1: buffer
    // @param2: size
    // @return: number of bytes read or -1 indicating error.
    //
    std::streamsize _read(void*, const size_t);

    // Function: _write
    //
    // Perform iterative write and flush the data from the streambuffer to the underlying device.
    // The implementation will automatically set the iostate of the underlying device.
    //
    //   - fail: unexpected error (with -1 returned)
    //   - good: otherwise
    //
    std::streamsize _write(const void*, const size_t);

};

//-------------------------------------------------------------------------------------------------

// Class: FIFOConnectorBase
class FIFOConnectorBase : public AsyncEventBase {

  public:
    
    // Constructor
    //
    // Construct an asynchronous FIFO connector.
    //
    // @param1: name of the FIFO
    // @param2: open mode (std::ios_base::in or std::ios_base::out)
    //
    FIFOConnectorBase(const std::string&, const std::ios_base::openmode);
    
    // Operator: ()
    //
    // User-defined operator that will be invoked asynchronously when either the FIFO is opened 
    // or the timeout is reached. The input argument is set to nullptr if the connector fails 
    // to open the FIFO.
    //
    virtual void operator () (std::shared_ptr<FIFO>&&) = 0;

  private:

    std::future<std::shared_ptr<FIFO>> _future;

    // Operator: ()
    //
    // Called by the underlying reactor when the timeout reached, in which the atomic boolean
    // variable is set to true to terminate the connector. 
    //
    void operator () () override final;

};

//-------------------------------------------------------------------------------------------------

// Class: FIFOReaderBase
class FIFOReaderBase : public ReadEventBase {
  
  protected:

    InputStreamBuffer <FIFO> _isbuf;
    std::istream _istream;

  public:
    
    // Constructor
    //
    // Construct a fifo reader through a fifo. Notice that the fifo reader participates 
    // the ownership.
    //
    FIFOReaderBase(const std::shared_ptr<FIFO>&);

    // Destructor
    //
    // Destroy the object.
    //
    ~FIFOReaderBase() = default;

    // Operator
    //
    // User-defined callback for read event. The fifo reader autonomously invokes the operator
    // and pass the input stream to caller.
    //
    // @param: input stream
    //
    virtual void operator () (std::istream&) = 0;

  private:

    // Operator
    //
    // Called by the underlying reactor in non-blocking fashion. The operator automatically 
    // synchronize the istream with the underlying fifo.
    //
    void operator () () override final;
    
};

//-------------------------------------------------------------------------------------------------

// Class: FIFOWriterBase
class FIFOWriterBase : public WriteEventBase {

  protected:

    OutputStreamBuffer<FIFO> _osbuf;
    std::ostream _ostream;
  
  public:
    
    // Constructor
    //
    // Construct a fifo writer through a fifo. Notice that the fifo writer participates the 
    // ownership.
    //
    FIFOWriterBase(const std::shared_ptr<FIFO>&);

    // Destructor
    //
    // Destroy the object.
    //
    ~FIFOWriterBase() = default;

    // Operator
    //
    // User-defined callback for write event. The fifo reader autonomously invokes the operator
    // and pass the output stream to caller.
    //
    // @param: input stream
    //
    virtual void operator () (std::ostream&) = 0;

  private: 

    // Operator
    //
    // Called by the underlying reactor in non-blocking fashion. The operator automatically 
    // synchronizes the ostream with the underlying fifo.
    //
    void operator () () override final;
};

*/
};  // End of namespace dtc. --------------------------------------------------------------




#endif



