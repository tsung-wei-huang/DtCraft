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

#ifndef DTC_KERNEL_STREAM_HPP_
#define DTC_KERNEL_STREAM_HPP_

#include <dtc/ipc/ipc.hpp>

namespace dtc {

// Forward declaration.
class Vertex;

// ------------------------------------------------------------------------------------------------

// Class: Stream
class Stream final {

  friend class Vertex;
  friend class Graph;
  friend class Executor;
  friend class StreamBuilder;

  using Writer = std::variant<std::weak_ptr<OutputStream>, std::shared_ptr<Device>>;
  using Reader = std::variant<std::weak_ptr<InputStream>, std::shared_ptr<Device>>;

  public:

    const key_type key {-1};
    
    Stream(key_type, Vertex*, Vertex*);

    bool is_intra_stream() const;
    bool is_inter_stream() const;
    bool is_inter_stream(std::ios_base::openmode) const;

    std::shared_ptr<OutputStream> ostream() const;
    std::shared_ptr<InputStream> istream() const;

    std::shared_ptr<Device> obridge() const;
    std::shared_ptr<Device> ibridge() const;
    std::shared_ptr<Device> extract_obridge();
    std::shared_ptr<Device> extract_ibridge();

    inline const std::string& tag() const;

  private:

    std::string _tag;

    Writer _writer;
    Reader _reader;

    Vertex* _tail {nullptr};
    Vertex* _head {nullptr};

    bool _critical {false};

    std::function<Event::Signal(Vertex&, OutputStream&)> _on_ostream;
    std::function<Event::Signal(Vertex&, InputStream&)> _on_istream;

    Event::Signal operator () (InputStream&) const;
    Event::Signal operator () (OutputStream&) const;

};

// Function: tag
inline const std::string& Stream::tag() const {
  return _tag;
}

// ------------------------------------------------------------------------------------------------

// Class: PlaceHolder
class PlaceHolder {
  
  friend class Graph;

  public:

    const std::optional<key_type> tail;
    const std::optional<key_type> head;

    PlaceHolder(std::optional<key_type>, std::optional<key_type>);

    inline const auto& keys() const;
    inline size_t num_keys() const;

  private:

    std::vector<key_type> _keys;
};

// Function: num_keys
inline size_t PlaceHolder::num_keys() const {
  return _keys.size();
}

// Function: keys
inline const auto& PlaceHolder::keys() const {
  return _keys;
}

};  // end of namespace dtc. ----------------------------------------------------------------------

#endif












