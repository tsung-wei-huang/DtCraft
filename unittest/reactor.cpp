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

TEST_CASE("ReactorTest.Notify") {

  using namespace std::chrono_literals;

  dtc::Reactor R;
  
  auto _notifier = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);

  REQUIRE(_notifier != -1);

  R.insert<dtc::ReadEvent>(
    _notifier,
    [&] (dtc::Event& e) {
      R.remove(e.shared_from_this());
    }
  );

  std::thread(
    [&] () {
      std::this_thread::sleep_for(100ms);
      R.insert<dtc::TimeoutEvent>(
        0ms,
        [&] (dtc::Event& e) {
          std::this_thread::sleep_for(100ms);
          uint64_t c = 1;
          R.promise([&] () { 
            REQUIRE(::write(_notifier, &c, sizeof(c)) == sizeof(c)); 
          });
        }
      );
    }
  ).detach();
  
  R.dispatch();

  REQUIRE(::close(_notifier) != -1);
}

// ---- Reactor test-------------------------------------------------------------------------------

// Unittest: ReactorTest.Reactor
TEST_CASE("ReactorTest.Reactor") {

  dtc::Reactor reactor;

  REQUIRE(reactor.is_owner());

  auto ret = reactor.promise([](){return true;}).get();
  REQUIRE(ret);

  REQUIRE(reactor.num_events() == 0);
}

//-------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.TimeoutEvent
TEST_CASE("ReactorTest.TimeoutEvent") {

  const auto N = 16;
  
  // Test the priority queue
  for(auto n=1; n<=N; n++) {
    std::mutex mutex;
    std::vector<int> ranks;
    auto beg = std::chrono::steady_clock::now();
    {
      dtc::Reactor R;
      for(auto i=0; i<n; ++i) {
        R.insert<dtc::TimeoutEvent>(
          std::chrono::milliseconds(i*100), 
          [&, i] (auto&&) {
            std::lock_guard L(mutex);
            ranks.push_back(i);
          }
        );
      }
      REQUIRE(R.num_events() == n);
      R.dispatch(); 
      REQUIRE(R.num_events() == 0);
    }
    auto end = std::chrono::steady_clock::now();
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(end-beg).count() <= n*100);
    CHECK(std::is_sorted(ranks.begin(), ranks.end()));
  }

} 

//-------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.Periodic
TEST_CASE("ReactorTest.Periodic") {
  
  const auto N = 16;

  for(auto n=1; n<=N; n++) {
    std::vector<int> ticks(n, 0);
    {
      dtc::Reactor R;
      for(auto i=1; i<=n; ++i) {
        R.insert<dtc::PeriodicEvent>(
          std::chrono::milliseconds(i*100), 
          true,
          [&, i] (auto&&) mutable {
            ticks[i-1]++;
          }
        );
      }

      R.insert<dtc::TimeoutEvent>(
        std::chrono::milliseconds(n*100),
        [&] (auto&& e) { R.shutdown(); }
      );
      REQUIRE(R.num_events() == n + 1);
      R.dispatch(); 
      REQUIRE(R.num_events() == n);
    }
    CHECK(std::accumulate(ticks.begin(), ticks.end(), 0) >= n);
  }
}

//-------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.EOF
TEST_CASE("ReactorTest.EOF") {

  constexpr int num_events = 100;
  dtc::Reactor R;
  std::atomic<int> counter {0};

  for(size_t i=0;i<num_events;++i){
    if(int fd[2]; ::pipe2(fd, O_CLOEXEC | O_NONBLOCK) != -1) {
      R.insert<dtc::ReadEvent>(
        fd[0],
        [&] (dtc::Event& e) mutable {
          char c;
          REQUIRE(::read(e.descriptor(), &c, sizeof(char)) == 0);
          ++counter;
          R.remove(e.shared_from_this());
        }
      );
      REQUIRE(::close(fd[1]) != -1);
    }
  }

  R.dispatch();

  REQUIRE(num_events == counter.load());
}

//-------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.IO
TEST_CASE("ReactorTest.IO") {
  
  const auto N = 16;
  constexpr auto S = 256;
      
  std::array<char, S> str;

  dtc::Reactor R;

  for(auto n=0; n<N; n++) {

    if(int fd[2]; ::pipe2(fd, O_CLOEXEC | O_NONBLOCK) != -1) {

      for(size_t i=0; i<str.size(); ++i) str[i] = dtc::random<char>('0', '9');

      // Write end.
      REQUIRE(::write(fd[1], str.data(), str.size()) == str.size());

      // Read end
      R.insert<dtc::ReadEvent>(
        fd[0],
        [&R, str, res=std::array<char, S>()] (dtc::Event& e) mutable {
          if(auto ret = ::read(e.descriptor(), res.data(), res.size()); ret != -1) {
            REQUIRE(res == str);
            R.remove(e.shared_from_this());
          }
        }
      );
    }
  }

  R.dispatch();

}



