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

#define CATCH_CONFIG_MAIN 

#include <dtc/unittest/catch.hpp>
#include <dtc/dtc.hpp>

using namespace std::literals::chrono_literals;

// ---- Utility -----------------------------------------------------------------------------------
template <typename D>
auto make_device_pair() {

  std::shared_ptr<dtc::Device> rend, wend;

  if constexpr(std::is_same_v<D, dtc::Socket>) {
    std::tie(rend, wend) = dtc::make_domain_pair();
  }
  else if constexpr(std::is_same_v<D, dtc::SharedMemory>) {
    rend = wend = dtc::make_shared_memory();
  }
  else static_assert(dtc::dependent_false_v<D>);

  return std::make_tuple(std::move(rend), std::move(wend));
}

// ---- Device test -------------------------------------------------------------------------------

// Procedure: test_device_property
auto test_device_property(std::shared_ptr<dtc::Device> device) {
  REQUIRE(dtc::is_fd_valid(device->fd()));
  REQUIRE(dtc::is_fd_nonblocking(device->fd()));
  REQUIRE(dtc::is_fd_close_on_exec(device->fd()));
}

// Procedure: test_device_io
auto test_device_io(std::shared_ptr<dtc::Device> rend, std::shared_ptr<dtc::Device> wend) {
  
  auto wstr = dtc::random<std::string>('a', 'z', 2000000);
  auto rstr = dtc::random<std::string>('0', '9', 2000000);

  auto num_writes = wend->write(wstr.data(), wstr.size());
  REQUIRE(wend->write(wstr.data() + num_writes, wstr.size()) == -1);

  auto num_reads = rend->read(rstr.data(), rstr.size());
  REQUIRE(rend->read(rstr.data() + num_reads, rstr.size()) == -1);

  REQUIRE(num_reads <= num_writes);
  REQUIRE(wstr.substr(0, num_reads) == rstr.substr(0, num_reads));
}

// Test case: DeviceTest.Socket
TEST_CASE("DeviceTest.Socket") {

  std::string H {"127.0.0.1"}; // test host.

  // Bind a listener.
  auto L = dtc::make_socket_server("0");  
  REQUIRE((L && L->is_listener()));

  // Try accepting an incoming connection.
  REQUIRE_THROWS_AS(L->accept(), std::system_error);

  // Try connecting to the wrong port
  REQUIRE_THROWS_AS(dtc::make_socket_client(H, "21"), std::system_error);
  
  // Try connecting to the right port
  auto C = dtc::make_socket_client(H, L->this_host().second);
  REQUIRE((C && C->is_connected()));

  // Now accept the connection from the listener.
  auto A = L->accept();
  REQUIRE((A && A->is_connected()));
  
  // Cross validation.
  REQUIRE(A->this_host() == C->peer_host());
  REQUIRE(A->peer_host() == C->this_host());
  
  test_device_property(A);
}

// Test case: DeviceTest.ShareMemory
TEST_CASE("DeviceTest.SharedMemory") {
  auto shm = dtc::make_shared_memory();
  REQUIRE((shm != nullptr));
  test_device_property(shm);
}

// Test case: DeviceTest.IO.Socket
TEST_CASE("DeviceTest.IO.Socket") {
  std::apply(test_device_io, make_device_pair<dtc::Socket>());
}

// Test case: DeviceTest.IO.SharedMemory
TEST_CASE("DeviceTest.IO.SharedMemory") {
  std::apply(test_device_io, make_device_pair<dtc::SharedMemory>());
}

// ---- StreamBufferTest --------------------------------------------------------------------------

// Procedure: test_streambuf_string_view
auto test_streambuf_string_view() {
  
  const auto N = 65536;

  for(size_t n=0; n<=N; n = (n>1024) ? n<<1 : n+1) {

    auto s = dtc::random<std::string>('a', 'z', n);

    dtc::OutputStreamBuffer osbuf;
    osbuf.write(s.data(), s.size());
    REQUIRE(osbuf.string_view() == s);

    {
      dtc::InputStreamBuffer isbuf(osbuf);
      REQUIRE((isbuf.string_view() == s && osbuf.string_view() == s));
    }
    
    dtc::InputStreamBuffer isbuf(std::move(osbuf));
    REQUIRE(isbuf.string_view() == s);
    REQUIRE(osbuf.string_view() == "");
  }
}

// Procedure: test_streambuf_flush
template <typename D>
auto test_streambuf_flush() {

  for(size_t n=0; n<=65536; n = (n>1024) ? n<<1 : n+1) {

    auto [rend, wend] = make_device_pair<D>();

    dtc::OutputStreamBuffer osbuf;
    
    REQUIRE(osbuf.flush() == 0); 

    for(size_t i=0; i<n; ++i) {
      auto t = dtc::random<int>();
      osbuf.write(&t, sizeof(t));
    }
    
    osbuf.device(wend);
    auto ret = osbuf.flush();
    REQUIRE(((ret == -1 && osbuf.out_avail() > 0) || (osbuf.out_avail() == 0)));
  }
}

// Procedure: test_streambuf_copy
auto test_streambuf_copy() {
  
  constexpr auto N = std::streamsize{65536};

  for(std::streamsize n=0; n<=N; n = (n>1024) ? n<<1 : n+1) {

    const auto S = dtc::random<std::string>('0', '9', n);
    
    dtc::OutputStreamBuffer osbuf;

    REQUIRE(osbuf.write(S.data(), n) == n);

    auto s = dtc::random<std::string>('a', 'z', n);
    osbuf.copy(s.data(), n);
    REQUIRE(s == S);

    dtc::InputStreamBuffer isbuf(osbuf);
    
    REQUIRE(osbuf.out_avail() == n);
    REQUIRE(isbuf.in_avail() == n);

    s = dtc::random<std::string>('a', 'z', n);
    isbuf.copy(s.data(), n);
    REQUIRE(s == S);
  }
}

// Procedure: test_streambuf_move
auto test_streambuf_move() {
  
  constexpr auto N = std::streamsize{65536};

  for(std::streamsize n=0; n<=N; n = (n>1024) ? n<<1 : n+1) {

    const auto S = dtc::random<std::string>('0', '9', n);
    
    dtc::OutputStreamBuffer osbuf;
    REQUIRE(osbuf.write(S.data(), n) == n);

    dtc::InputStreamBuffer isbuf(std::move(osbuf));
    REQUIRE(osbuf.out_avail() == 0);

    auto s = dtc::random<std::string>('a', 'z', n); 
    isbuf.copy(s.data(), n);
    REQUIRE(s == S); 
  }
}

// Procedure: test_streambuf_sync
template <typename D>
auto test_streambuf_sync() {

  auto [rend, wend] = make_device_pair<D>();

  const auto W = std::thread::hardware_concurrency();
  const auto N = 65536;

  for(size_t n=0; n<=N; n = (n>1024) ? n<<1 : n+1) {

    dtc::InputStreamBuffer isbuf(rend);
    dtc::OutputStreamBuffer osbuf(wend);
    
    REQUIRE((isbuf.sync() == -1 && osbuf.sync() == 0));

    std::atomic<int64_t> ores{0};
    std::atomic<int64_t> ires{0};
    
    // Concurrent write
    REQUIRE(osbuf.out_avail() == 0);
    std::vector<std::thread> oworkers;
    for(size_t w=0; w<W; ++w) {
      oworkers.emplace_back(
        [&]() {
          for(size_t i=0; i<n; ++i) {
            auto t = dtc::random<char>();
            osbuf.write(&t, sizeof(t));
            ores += t;
          }
        }
      );
    }
    for(auto& w : oworkers) w.join();

    // Check the byte count.
    auto num_bytes = osbuf.out_avail();
    REQUIRE(num_bytes == static_cast<std::streamsize>(n*W*sizeof(char)));

    // Synchronize the osbuf/isbuf buffer concurrently.
    std::thread t1([&](){ while(osbuf.out_avail() > 0) osbuf.sync(); });
    std::thread t2([&](){ while(isbuf.in_avail() < num_bytes) isbuf.sync(); });
    t1.join();
    t2.join();

    // Concurrent read
    std::vector<std::thread> iworkers;
    for(size_t w=0; w<W; ++w) {
      iworkers.emplace_back(
        [&]() {
          for(size_t i=0; i<n; ++i) {
            char t;
            isbuf.read(&t, sizeof(char));
            ires += t;
          }
        }
      );
    }
    for(auto& w : iworkers) w.join();

    REQUIRE(isbuf.in_avail() == 0);
    REQUIRE(ores == ires);

    isbuf.device(nullptr);
    osbuf.device(nullptr);
    REQUIRE((isbuf.sync() == -1 && osbuf.sync() == -1));
  }
}

// Test case: StreamBufferTest.StringView
TEST_CASE("StreamBufferTest.StringView") {
  test_streambuf_string_view();
}

// Test case: StreamBufferTest.Copy
TEST_CASE("StreamBufferTest.Copy") {
  test_streambuf_copy();
}

// Test case: StreamBufferTest.Move
TEST_CASE("StreamBufferTest.Move") {
  test_streambuf_move();
}

// Test case: StreamBufferTest.Flush.Socket
TEST_CASE("StreamBufferTest.Flush.Socket") {
  test_streambuf_flush<dtc::Socket>();
}

// Test case: StreamBufferTest.Flush.SharedMemory
TEST_CASE("StreamBufferTest.Flush.SharedMemory") {
  test_streambuf_flush<dtc::SharedMemory>();
}

// Test case: StreamBufferTest.Sync.Socket
TEST_CASE("StreamBufferTest.Sync.Socket") {
  test_streambuf_sync<dtc::Socket>();
}

// Test case: StreamBufferTest.Sync.SharedMemory
TEST_CASE("StreamBufferTest.Sync.SharedMemory") {
  test_streambuf_sync<dtc::SharedMemory>();
}

// ---- Stream test -------------------------------------------------------------------------------

// Procedure: test_stream_criticality
// The procedure tests the criticality of an ostream event and an istream event.
// Only one thread can enter to the context of the istream's or the ostream's callback.
template <typename D>
auto test_stream_criticality() {
  
  dtc::Reactor R;

  auto [rend, wend] = make_device_pair<D>();

  std::atomic<int> oexternal {0};
  std::atomic<int> iexternal {0};

  constexpr auto N = 1024;
  constexpr auto P = 1024;

  // Create an ostream
  auto ostream = R.insert<dtc::OutputStream>(
    wend,
    [ointernal=0, &oexternal] (dtc::OutputStream& ostream) mutable {
      ostream.osbuf.sync();
      REQUIRE(++ointernal == ++oexternal);
    }
  ).get();

  for(int p=0; p<P; ++p) { 
    R.insert<dtc::PeriodicEvent>(
      0ms,
      true,
      [&ostream, i=0, &R, N] (dtc::Event& e) mutable {
        if(i++ < N) (*ostream)(dtc::random<char>());
        else R.remove(e.shared_from_this());
      }
    );
  }

  R.insert<dtc::InputStream>(
    rend,
    [iinternal=0, &iexternal, i=0, &R] (dtc::InputStream& istream) mutable {
      istream.isbuf.sync();
      REQUIRE(++iinternal == ++iexternal);
      char c;
      while(istream(c) != -1) {
        ++i;
      }
      if(i == N*P) R.shutdown();
    }
  );

  R.dispatch(); 
}

// Procedure: test_stream_io
// The procedure test the read/write operations of an iostream pair.
template <typename D>
auto test_stream_io() {

  dtc::Reactor R;

  for(int e=0; e<32; ++e) {

    auto [rend, wend] = make_device_pair<D>();
    auto n = dtc::random<size_t>(0, 2000000);
    auto data = dtc::random<std::string>('0', '9', n);

    // Create an ostream
    auto ostream = R.insert<dtc::OutputStream>(
      wend,
      [&R] (auto& ostream) {
        ostream.osbuf.sync();
        if(ostream.osbuf.out_avail() == 0) {
          R.remove(ostream.shared_from_this());
        }
      }
    ).get();

    (*ostream)(data);

    R.insert<dtc::InputStream>(
      rend,
      [&R, data] (auto& istream) {
        istream.isbuf.sync();
        if(std::string recv; istream(recv) != -1) {
          REQUIRE(recv == data);
          R.remove(istream.shared_from_this());
        }
      }
    );
  }
  
  R.dispatch(); 
}

// Test case: StreamTest.IO.Socket
TEST_CASE("StreamTest.IO.Socket") {
  test_stream_io<dtc::Socket>();
}

// Test case: StreamTest.IO.SharedMemory
TEST_CASE("StreamTest.IO.SharedMemory") {
  test_stream_io<dtc::SharedMemory>();
}

// Test case: StreamTest.Criticality.Socket
TEST_CASE("StreamTest.Criticality.Socket") {
  test_stream_criticality<dtc::Socket>();
}

// Test case: StreamTest.Criticality.SharedMemory
TEST_CASE("StreamTest.Criticality.SharedMemory") {
  test_stream_criticality<dtc::SharedMemory>();
}





