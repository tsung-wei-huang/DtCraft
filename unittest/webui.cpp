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

// This file is created and modified based on the cereal serialization/deserialization
// library invented by Randolph Voorhies and Shane Grant (Copyright (c) 2014).

#define CATCH_CONFIG_MAIN 

#include <dtc/unittest/catch.hpp>
#include <dtc/webui/webui.hpp>

bool is_ipv6(std::string_view sv) {
  char buf[16];
  return ::inet_pton(AF_INET6, std::string(sv).data(), buf) == 1;
}

void test_url(std::string_view str) {

  auto given = dtc::URL(str);

  auto const [scheme, host, port, path, query, fragment, userinfo] = 
  given.extract(
    dtc::URL::SCHEMA, 
    dtc::URL::HOST, 
    dtc::URL::PORT, 
    dtc::URL::PATH, 
    dtc::URL::QUERY, 
    dtc::URL::FRAGMENT, 
    dtc::URL::USERINFO
  );

  //std::cout << scheme << "+" << host << "+" << port << "+" << path << "+" << query << "+" << fragment << "+" << userinfo <<'\n';
  std::ostringstream url;
  url << scheme;
  
  if(scheme!= "") {
    url << ":";
  }

  // authority = userinfo (optional) + host + port (optional)
  if(host != "") {
    url << "//";
  }

  if(userinfo != "") {
    url << userinfo << "@";
  }
  
  if(host != "" && is_ipv6(host)) {
    url << "[" << host << "]";
  }
  else {
    url << host;
  }

  if(port != "") {
    url << ":" << port;
  }
  
  url << path;

  if(query != "") {
    url << "?" << query;
  }

  if(fragment != "") {
    url << "#" << fragment;
  }

  REQUIRE(url.str() == str);
};
  
auto generate_requests(size_t num_requests) {

  const std::array http_methods {HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_PUT, HTTP_HEAD};
  
  // "Content-Type", "Date" and "Connection" are must-include headers and have
  // specific value format.
  std::vector<std::string> opt_headers {
    "Accept-Language",
    "Accept-Encoding",
    "User-Agent",
    "Host"
  }; 

  std::vector<dtc::HttpRequest> reqs;

  for(size_t i = 0 ;i < num_requests; ++i){

    dtc::HttpRequest req;
    req.method = http_methods[dtc::random<uint32_t>()%http_methods.size()];
    req.url = "/";
    req.keep_alive = true;
    req.headers.emplace("Connection", "keep-alive");

    std::ostringstream os;
    
    auto t = std::time(nullptr);
    os << std::put_time(std::gmtime(&t), "%c %Z");
    req.headers.emplace("Date", os.str());
    req.headers.emplace(
      "Content-Type", 
      std::next(std::begin(dtc::MIME_TYPES), dtc::random<uint32_t>()%dtc::MIME_TYPES.size())->second
    );

    auto num_headers = dtc::random<uint32_t>()%opt_headers.size() + 1;

    // Random shuffle the opt_headers 
    for(size_t j = 0; j < num_headers; ++j) {
      std::swap(opt_headers[0], opt_headers[dtc::random<uint32_t>()%(opt_headers.size()-1)+1]);
    }

    // Below regex removes the leading & trailing spaces from the random string
    for(size_t j = 0; j < num_headers; ++j) {
      req.headers.emplace(
        opt_headers[j], 
        std::regex_replace(dtc::random<std::string>(), std::regex("^ +| +$|( ) +"), "$1")
      );
    }
    reqs.emplace_back(req);
  }

  return reqs;
};

// ------------------------------------------------------------------------------------------------

// Testcase: Webui.URL
// The format of URL (Uniform Resource Locator) is as follows:
//   scheme:[//[user[:password]@]host[:port]][/path][?query][#fragment]
TEST_CASE("WebuiTest.URL") {

  const std::vector<std::string> urls {
   "http://www.google.com",
   "http://www.google.com/",
   "http://www.example.com/index.html",
   "http://www.example.com/p1/p2/p3/file1",
   "foo://example-1.com:8042/over/there?name=ferret#nose",
   "https://user:pass@www.example.com:81/path/index.php?query=toto+le+heros#top",
   "http://www.foo.bar/?listings.html#section-2",
   "ftp://ftp.foo.bar/~john/doe?w=100&h=50",
   "ftp://username:password@host.com/",
   "http://www.foo.bar/image.jpg?height=150;width=100",
   "https://www.secured.com:443/resource.html?id=6e8bc430-9c3a-11d9-9669-0800200c9a66#some-header",
   "https://foo:bar@w1.superman.com/very/long/path.html?p1=v1&p2=v2#more-details",
   "http://www.cwi.nl:80/%7Eguido/Python.html",
   "/js/controller.js",
   "http://127.0.0.1:5050/metrics/snapshot?jsonp=angular.callbacks._1",
   "http://[1080:0:0:0:8:800:200C:417A]:61616/foo/bar?q=z"  /* ipv6 */
  };

  for(const auto &url : urls) {
    test_url(url);
  }
}

// Testcase: WebuiTest.HttpRequestParser
TEST_CASE("WebuiTest.HttpRequestParser") {

  auto stringfy_requests = [&](std::vector<dtc::HttpRequest> reqs){
    std::ostringstream os;
    for(auto &req: reqs){
      os << req.to_string();
    }
    return os.str();
  };

  auto reqs = generate_requests(500);
  auto data = stringfy_requests(reqs);

  dtc::HttpRequestParser P;

  auto B = P(data);

  REQUIRE(B == data.size());

  // Notice: parser stops parsing when the keep_alive is set to close.
  P.on(
    [id = size_t {0}, &reqs](dtc::HttpRequest &req) mutable {
      REQUIRE( reqs[id].method == req.method );
      REQUIRE( reqs[id].url == req.url );
      REQUIRE( reqs[id].keep_alive == req.keep_alive );
      REQUIRE( reqs[id].headers == req.headers );
      ++id;
    }
  );

  
}


