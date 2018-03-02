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

#define CATCH_CONFIG_MAIN 

#include <dtc/unittest/catch.hpp>
#include <dtc/dtc.hpp>

// Unittest: ReactorTest.EventOperation
TEST_CASE("ReactorTest.EventOperation") {

  dtc::Reactor reactor;

  REQUIRE(reactor.num_events() == 0);

  auto [r1, r2, r3, r4] = reactor.remove(nullptr, nullptr, nullptr, nullptr).get();
  
  REQUIRE((!r1 && !r2 && !r3 && !r4));
  REQUIRE(reactor.num_events() == 0);

  auto e1 = reactor.insert<dtc::TimeoutEvent>(std::chrono::seconds(1), [](dtc::Event&){}).get();
  auto e2 = reactor.insert<dtc::TimeoutEvent>(std::chrono::seconds(1), [](dtc::Event&){}).get();
  auto e3 = reactor.insert<dtc::TimeoutEvent>(std::chrono::seconds(1), [](dtc::Event&){}).get();
  auto e4 = reactor.insert<dtc::TimeoutEvent>(std::chrono::seconds(1), [](dtc::Event&){}).get();

  REQUIRE((e1 && e2 && e3 && e4));
  REQUIRE(reactor.num_events() == 4);

  std::tie(r1, r2, r3, r4) = reactor.remove(nullptr, e2, nullptr, e4).get();

  REQUIRE((!r1 && r2 && !r3 && r4));
  REQUIRE(reactor.num_events() == 2);

  std::tie(r1, r3) = reactor.remove(e1, e3).get();

  REQUIRE((r1 && r3));
  REQUIRE(reactor.num_events() == 0);
}

// ------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.Promise
TEST_CASE("ReactorTest.Promise") {

  auto master = std::this_thread::get_id();

  dtc::Reactor reactor;

  REQUIRE(reactor.is_owner());
  REQUIRE(reactor.num_events() == 0);

  auto ret = reactor.promise(
    [master] () {
      REQUIRE(master == std::this_thread::get_id());
      return true;
    }
  ).get();
  
  REQUIRE(ret);
  
  std::vector<std::thread> threads;  

  int counter {0};

  for(int i=0; i<4; ++i) {
    threads.emplace_back(
      [&] () {
        for(int j=0; j<1000; ++j) {
          reactor.promise([&] () { 
            REQUIRE(master == std::this_thread::get_id()); 
            ++counter;
          });
        }
      }
    );
  }

  for(auto& t : threads) t.join();
  
  REQUIRE(counter == 0);

  reactor.dispatch();

  REQUIRE(counter == 4000);
}

// Unittest: ReactorTest.Async
TEST_CASE("ReactorTest.Async") {

  auto master = std::this_thread::get_id();

  for(int w=0; w<=4; ++w) {
    std::atomic<int> counter {0};
    {
      dtc::Reactor reactor(w);
      for(int i=0; i<1000; ++i) {
        reactor.async([&] {
          counter++; 
          if(w == 0) REQUIRE(std::this_thread::get_id() == master);
          else REQUIRE(std::this_thread::get_id() != master);
        });
      }
    }
    REQUIRE(counter == 1000);
  }
}

// ------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.Master
TEST_CASE("ReactorTest.Master") {

  dtc::Reactor reactor(0);

  REQUIRE(reactor.num_workers() == 0);

  reactor.insert<dtc::TimeoutEvent>(
    std::chrono::milliseconds(0),
    [master=std::this_thread::get_id()] (dtc::Event& e) {
      REQUIRE(master == std::this_thread::get_id());
    }
  );

  REQUIRE(reactor.num_events() == 1);

  reactor.dispatch();
  
  REQUIRE(reactor.num_events() == 0);
}

// ------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.Workers
TEST_CASE("ReactorTest.Workers") {

  auto master = std::this_thread::get_id();

  dtc::Reactor reactor(2);

  REQUIRE(reactor.num_workers() == 2);

  reactor.insert<dtc::TimeoutEvent>(
    std::chrono::milliseconds(0),
    [master] (dtc::Event& e) {
      REQUIRE(master != std::this_thread::get_id());
    }
  );

  REQUIRE(reactor.num_events() == 1);

  reactor.dispatch();
  
  REQUIRE(reactor.num_events() == 0);
}

// ------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.NotifyInsert
TEST_CASE("ReactorTest.NotifyInsert") {

  using namespace std::chrono_literals;

  dtc::Reactor R;
  
  auto notifier = dtc::make_notifier();

  R.insert<dtc::ReadEvent>(
    notifier,
    [&] (dtc::Event& e) {
      return dtc::Event::REMOVE;
    }
  );

  std::thread t1(
    [&] () {
      std::this_thread::sleep_for(1s);
      R.insert<dtc::TimeoutEvent>(
        0ms,
        [&] (dtc::Event& e) {
          std::this_thread::sleep_for(100ms);
          uint64_t c = 1;
          R.promise([&] () { 
            REQUIRE(::write(notifier->fd(), &c, sizeof(c)) == sizeof(c)); 
          });
        }
      );
    }
  );
  
  R.dispatch();

  t1.join();
}

// Unittest: ReactorTest.NotifyRemove
TEST_CASE("ReactorTest.NotifyRemove") {

  using namespace std::chrono_literals;

  dtc::Reactor R;
  
  auto event = R.insert<dtc::ReadEvent>(
    dtc::make_notifier(), [&] (dtc::Event& e) {}
  ).get();

  std::thread t1(
    [&] () {
      std::this_thread::sleep_for(1s);
      auto [r] = R.remove(event).get();
      REQUIRE(r == true);
    }
  );
  
  R.dispatch();

  t1.join();

  REQUIRE(R.num_events() == 0);
}

// Unittest: ReactorTest.NotifyThaw
TEST_CASE("ReactorTest.NotifyThaw") {

  using namespace std::chrono_literals;

  dtc::Reactor R;
  
  std::atomic<int> counter {0};

  auto event = R.insert<dtc::WriteEvent>(
    dtc::make_notifier(), 
    [&] (dtc::Event& e) {
      ++counter;
      return dtc::Event::REMOVE;
    }
  ).get();

  std::thread t1(
    [&] () {
      std::this_thread::sleep_for(1s);
      REQUIRE(counter == 0);
      auto [r] = R.thaw(event).get();
      REQUIRE(r == true);
    }
  );
  
  R.dispatch();

  t1.join();

  REQUIRE(R.num_events() == 0);
  REQUIRE(counter == 1);
}

// ------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.TimeoutEvent
TEST_CASE("ReactorTest.TimeoutEvent") {

  const auto N = 65536;
  size_t w = 1;
  size_t n = 1;

  for(size_t w=0; w<=4; w++) {
    for(size_t n=1; n<=N; n <<= 1) {
      std::atomic<size_t> counter {0};
      {
        dtc::Reactor R(w);
        for(size_t i=0; i<n; ++i) {
          R.insert<dtc::TimeoutEvent>(
            std::chrono::milliseconds(1), 
            [&] (auto&&) { counter++; }
          );
        }
        REQUIRE(R.num_events() == n);
        R.dispatch();
      }
      REQUIRE(counter.load() == n);
    }
  }
} 

// ------------------------------------------------------------------------------------------------

// Unittest: ReactorTest.EOF
TEST_CASE("ReactorTest.EOF") {

  constexpr int num_events = 64;

  for(size_t w=0; w<=4; w++) {

    dtc::Reactor R(w);
    std::atomic<int> counter {0};

    for(size_t i=0;i<num_events;++i){
      auto [r, w] = dtc::make_pipe();
      R.insert<dtc::ReadEvent>(
        std::move(r),
        [&] (dtc::Event& e) mutable {
          char c;
          REQUIRE(::read(e.device()->fd(), &c, sizeof(char)) == 0);
          ++counter;
          R.remove(e.shared_from_this());
        }
      );
    }

    R.dispatch();

    REQUIRE(num_events == counter.load());
  }
}


// Unittest: ReactorTest.Read
TEST_CASE("ReactorTest.Read") {
  
  for(size_t w=0; w<=4; ++w) {      // num workers
    for(auto n=0; n<=32; n++) {    // num events

      dtc::Reactor R(w);
      std::atomic<size_t> num_actives {0};
      std::vector<std::shared_ptr<dtc::Notifier>> notifiers;
      
      for(int i=1; i<=n; ++i) {

        try {
          auto n = dtc::make_notifier(1);
          notifiers.push_back(n);
          R.insert<dtc::ReadEvent>(
            n,
            [&] (dtc::Event& e) {
              int64_t cnt;
              auto ret = ::read(e.device()->fd(), &cnt, sizeof(cnt));
              REQUIRE(ret == sizeof(cnt));
              REQUIRE(cnt == 1);
              num_actives++;
              return dtc::Event::REMOVE;
            }
          );
        }
        catch(...) {
          break;
        }
      }

      R.dispatch();
      REQUIRE(notifiers.size() == num_actives);
    }
  }
}

// Unittest: ReactorTest.Write
TEST_CASE("ReactorTest.Write") {
  
  for(size_t w=0; w<=4; ++w) {
    for(auto n=0; n<=32; n++) {

      dtc::Reactor R(w);
      std::atomic<size_t> num_actives {0};
      std::vector<std::shared_ptr<dtc::Notifier>> notifiers;
      std::vector<std::shared_ptr<dtc::WriteEvent>> events;
      
      for(int i=1; i<=n; ++i) {

        try {
          auto n = dtc::make_notifier();
          notifiers.push_back(n);
          events.push_back(
            R.insert<dtc::WriteEvent>(
              n,
              [&] (dtc::Event& e) {
                num_actives++;
                R.remove(e.shared_from_this());
              }
            ).get()
          );
        }
        catch(...) {
          break;
        }
      }

      R.insert<dtc::TimeoutEvent>(
        std::chrono::milliseconds(10),
        [&] (auto&) {
          REQUIRE(num_actives == 0);
          for(auto& e : events) {
            R.thaw(e);
          }
        }
      );
      
      R.dispatch();
      REQUIRE(notifiers.size() == num_actives);
    }
  }
}

