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

#ifndef DTC_BLOCK_FILE_HPP_
#define DTC_BLOCK_FILE_HPP_

#include <dtc/device.hpp>

namespace dtc {

// Class: BlockFile
class BlockFile : public Device {
  
  friend class OutputStreamBuffer;
  friend class InputStreamBuffer;

  public:
    
    template <typename... Ts>
    BlockFile(Ts&&...);

    ~BlockFile() = default;

    std::streamsize read(void*, std::streamsize) const override final;
    std::streamsize write(const void*, std::streamsize) const override final;
};

template <typename... Ts>
BlockFile::BlockFile(Ts&&... ts) : Device {std::forward<Ts>(ts)...} {}

// ------------------------------------------------------------------------------------------------

// Function: make_block_file
std::shared_ptr<BlockFile> make_block_file(
  const std::filesystem::path& = "", 
  std::ios_base::openmode = std::ios_base::in | std::ios_base::out
);

};  // End of namespace dtc. ----------------------------------------------------------------------


#endif





