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


#ifndef DTC_WEBUI_WEBUI_HPP_
#define DTC_WEBUI_WEBUI_HPP_

#include <dtc/headerdef.hpp>
#include <dtc/utility.hpp>

namespace dtc {
  
inline std::string_view HTTP_VERSION = "HTTP/1.1";

// Struct: HttpStatusCode
struct HttpStatusCode {
  static constexpr int CONTINUE {100};
  static constexpr int SWITCHING_PROTOCOLS {101};
  static constexpr int PROCESSING {102};
  static constexpr int OK {200};
  static constexpr int CREATED {201};
  static constexpr int ACCEPTED {202};
  static constexpr int NON_AUTHORITATIVE_INFORMATION {203};
  static constexpr int NO_CONTENT {204};
  static constexpr int RESET_CONTENT {205};
  static constexpr int MULTIPLE_CHOICES {300};
  static constexpr int MOVED_PERMANENTLY {301};
  static constexpr int FOUND {302};
  static constexpr int SEE_OTHER {303};
  static constexpr int NOT_MODIFIED {304};
  static constexpr int USE_PROXY {305};
  static constexpr int SWITCH_PROXY {306};
  static constexpr int TEMPORARY_REDIRECT {307};
  static constexpr int PERMANENT_REDIRECT {308};
  static constexpr int BAD_REQUEST {400};
  static constexpr int UNAUTHORIZED {401};
  static constexpr int PAYMENT_REQUIRED {402};
  static constexpr int FORBIDDEN {403}; 
  static constexpr int NOT_FOUND {404};
  static constexpr int METHOD_NOT_ALLOWED {405};
  static constexpr int NOT_ACCEPTABLE {406};
  static constexpr int PROXY_AUTHENTICATION_REQUIRED {407};
  static constexpr int REQUEST_TIMEOUT {408};
  static constexpr int UNSUPPORTED_MEDIA_TYPE {415};
  static constexpr int INTERNAL_SERVER_ERROR {500};
  static constexpr int NOT_IMPLEMENTED {501};
  static constexpr int BAD_GATEWAY {502};
  static constexpr int SERVICE_UNAVAILABLE {503};
  static constexpr int GATEWAY_TIMEOUT {504};
  static constexpr int HTTP_VERSION_NOT_SUPPORTED {505};
};

enum class HttpBodyType {
  UNDEFINED,
  FILE,
  QUERY
};

inline std::unordered_map<std::string_view, std::string_view> MIME_TYPES {
  {"", ""},
  {".woff2", "application/octet-stream"},
  {".woff", "application/octet-stream"},
  {".ttf", "application/octet-stream"},
  {".svg", "image/svg+xml"},
  {".eot", "application/octet-stream"},
  {".txt", "text/plain"},
  {".css", "text/css"},
  {".c", "text/c"},
  {".html", "text/html"},
  {".htm", "text/html"},
  {".ico", "image/x-icon"},
  {".jpg", "image/jpeg"},
  {".jpeg", "image/jpeg"},
  {".json", "application/json"},
  {".js", "application/javascript"},
  {".png", "image/png"},
  {".xml", "application/xml"}
};

inline std::unordered_map<int, const std::string> HTTP_STATUS {
  {100, "CONTINUE"                      },                   
  {101, "SWITCHING_PROTOCOLS"           },
  {102, "PROCESSING"                    },
  {200, "OK"                            },
  {201, "CREATED"                       },
  {202, "ACCEPTED"                      },
  {203, "NON_AUTHORITATIVE_INFORMATION" },
  {204, "NO_CONTENT"                    },
  {205, "RESET_CONTENT"                 },
  {300, "MULTIPLE_CHOICES"              },
  {301, "MOVED_PERMANENTLY"             },
  {302, "FOUND"                         },
  {303, "SEE_OTHER"                     },
  {304, "NOT_MODIFIED"                  },
  {305, "USE_PROXY"                     },
  {306, "SWITCH_PROXY"                  },
  {307, "TEMPORARY_REDIRECT"            },
  {308, "PERMANENT_REDIRECT"            },
  {400, "BAD_REQUEST"                   },
  {401, "UNAUTHORIZED"                  },
  {402, "PAYMENT_REQUIRED"              },
  {403, "FORBIDDEN"                     }, 
  {404, "NOT_FOUND"                     },
  {405, "METHOD_NOT_ALLOWED"            },            
  {406, "NOT_ACCEPTABLE"                },                
  {407, "PROXY_AUTHENTICATION_REQUIRED" },    
  {415, "Unsupported Media Type"        },
  {408, "REQUEST_TIMEOUT"               },               
  {500, "INTERNAL_SERVER_ERROR"         },         
  {501, "NOT_IMPLEMENTED"               },               
  {502, "BAD_GATEWAY"                   },                   
  {503, "SERVICE_UNAVAILABLE"           },           
  {504, "GATEWAY_TIMEOUT"               },               
  {505, "HTTP_VERSION_NOT_SUPPORTED"    }
};

// Struct : URL
struct URL : std::string {

  enum Field {
    SCHEMA = UF_SCHEMA,
    HOST = UF_HOST,
    PORT = UF_PORT,
    PATH = UF_PATH,
    QUERY = UF_QUERY,
    FRAGMENT = UF_FRAGMENT,
    USERINFO = UF_USERINFO
  };
  
  template <typename... ArgsT>
  URL(ArgsT&&...);

  inline std::string_view schema() const;
  inline std::string_view host() const;
  inline std::string_view port() const;
  inline std::string_view path() const;
  inline std::string_view query() const;
  inline std::string_view fragment() const;
  inline std::string_view userinfo() const;
	
	template <typename... T>
  auto extract(T&&... f) const;
};

// Constructor
template <typename... ArgsT>
URL::URL(ArgsT&&... args) : std::string {std::forward<ArgsT>(args)...} {
};

// Function: extract
template <typename... T>
auto URL::extract(T&&... f) const {

  http_parser_url D;
  http_parser_url_init(&D);
  http_parser_parse_url(data(), size(), 0, &D);

	auto lambda = [this, &D] (auto f) -> std::string_view {
    if(D.field_set & (1 << f)) {
      return {this->data() + D.field_data[f].off, D.field_data[f].len};
    }   
    return {nullptr, 0};
  };

	return std::make_tuple(lambda(f)...);
} 

// Function: schema
inline std::string_view URL::schema() const {
  return std::get<0>(extract(SCHEMA));
}

// Function: host
inline std::string_view URL::host() const {
  return std::get<0>(extract(HOST));
}

// Function: port
inline std::string_view URL::port() const {
  return std::get<0>(extract(PORT));
}

// Function: path
inline std::string_view URL::path() const {
  return std::get<0>(extract(PATH));
}

// Function: query
inline std::string_view URL::query() const {
  return std::get<0>(extract(QUERY));
}

// Function: fragment
inline std::string_view URL::fragment() const {
  return std::get<0>(extract(FRAGMENT));
}

// Function: userinfo
inline std::string_view URL::userinfo() const {
  return std::get<0>(extract(USERINFO));
}

// --------------------------------------------------------

// Struct: HttpRequest
// An http request consists of GET, POST, DELETE, and so on.
struct HttpRequest {

  http_method method;
  URL url;
  std::string body;
  
  bool keep_alive {true};

  std::unordered_map<std::string, std::string> headers; 

  void clear();

  HttpBodyType body_type() const;

  std::string to_string() const;
};

// Class: HttpRequestParser
class HttpRequestParser {

  public:

    HttpRequestParser();

    //std::tuple<std::vector<HttpRequest>, size_t> operator()(const char*, size_t);
    size_t operator()(std::string_view);

    template <typename C>
    auto on(C&&);

  private: 

  enum LastCallback{
    NONE = 0,
    ON_MESSAGE_BEGIN,
    ON_URL,
    ON_HEADER_FIELD,
    ON_HEADER_VALUE,
    ON_HEADERS_COMPLETE,
    ON_BODY,
    ON_MESSAGE_COMPLETE
  };
    
    http_parser _handle;
    http_parser_settings _settings;
  
    LastCallback _last_cb {NONE};
    
    std::queue<HttpRequest> _requests;
  
    HttpRequest _request;
    std::string _buffer;

    static int _on_message_begin(http_parser*);
    static int _on_url(http_parser*, const char*, size_t);
    static int _on_header_field(http_parser*, const char*, size_t);
    static int _on_header_value(http_parser*, const char*, size_t);
    static int _on_body(http_parser*, const char*, size_t);
    static int _on_headers_complete(http_parser*);
    static int _on_message_complete(http_parser*);
};

template <typename C>
auto HttpRequestParser::on(C&& c) {
  while(!_requests.empty()) {
    c(_requests.front());
    _requests.pop();
  }
}

//---------------------------------------------------------

struct HttpResponse {

  // Header fields
  int status;
  HttpBodyType body_type;
  std::string body;

  std::string_view content_type;
  std::string_view connection;

  //std::unordered_map<std::string,std::string> headers;  // other header field 
  
  HttpResponse(int, HttpBodyType, std::string_view, std::string_view, bool);

  std::string to_string() const;
};

// ---- Http-specific utilities ---------------------------

std::streamsize send_response(int, HttpResponse&&);

};  // End of namespace dtc. --------------------------------------------------------------

#endif
