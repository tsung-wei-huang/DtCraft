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

#include <dtc/webui/webui.hpp>

namespace dtc {

// ---- HttpRequest ---------------------------------------

// Procedure: clear
void HttpRequest::clear() {

  //method.clear();
  url.clear();
  headers.clear();
  body.clear();
  
  keep_alive = true;
}

// Function: body_type
HttpBodyType HttpRequest::body_type() const {

  auto [query, path] = url.extract(URL::QUERY, URL::PATH);

  if(query.find("callback") != std::string_view::npos) {
    return HttpBodyType::QUERY;
  }
  else if(!path.empty()) {
    return HttpBodyType::FILE;
  }

  //if(!url_query.empty() && url_query.find("callback") != std::string::npos ){
  //  return HttpBodyType::QUERY;
  //}
  //else if( !url_path.empty() && url_query.empty() /*&& url_frag.empty()*/ ){
  //  return HttpBodyType::FILE;
  //}
  return HttpBodyType::UNDEFINED;
}

// Function: to_string
std::string HttpRequest::to_string() const {
  std::ostringstream os;
  os << http_method_str(method) << " " << url << " " << HTTP_VERSION << "\r\n";

  //os << "Connection: " << (keep_alive ? "keep-alive" : "close") << "\r\n";

  for(const auto &[k, v]: headers) {
    os << k << ": " << v << "\r\n";
  }
  os << "\r\n";
  os << body;
  // TODO (Chun-Xun): need a \r\n?  Ans: No "\r\n" is needed after body.
  return os.str();
}

// ---- HttpRequestParser ---------------------------------

// Constructor
HttpRequestParser::HttpRequestParser() {

  _settings.on_message_begin = &_on_message_begin;
  _settings.on_url = &_on_url;
  _settings.on_header_field = &_on_header_field;
  _settings.on_header_value = &_on_header_value;
  _settings.on_headers_complete = &_on_headers_complete;
  _settings.on_body = &_on_body;
  _settings.on_message_complete = &_on_message_complete;

  http_parser_init(&_handle, HTTP_REQUEST);
}

//std::tuple<std::queue<HttpRequest>, size_t> HttpRequestParser::operator ()(const char* buf, size_t sz) {
//  auto n = http_parser_execute(&_handle, &_settings, buf, sz);
//  return std::make_tuple(std::move(_requests), n);
//}

size_t HttpRequestParser::operator()(std::string_view S) {
  _handle.data = this;
  return http_parser_execute(&_handle, &_settings, S.data(), S.size());
}

// Function: _on_message_bgin  
int HttpRequestParser::_on_message_begin(http_parser* p) {
  auto self = static_cast<HttpRequestParser*>(p->data);
  self->_request.clear();
  self->_buffer.clear();
  self->_last_cb = ON_MESSAGE_BEGIN;
  return 0;
}

// Function: _on_url
int HttpRequestParser::_on_url(http_parser* p, const char* data, size_t length) {
  auto self = static_cast<HttpRequestParser*>(p->data);
  self->_request.url.append(data, length);
  self->_last_cb = ON_URL;
  return 0;
}

// Function: _on_header_field
int HttpRequestParser::_on_header_field(http_parser* p, const char* data, size_t length) {
  auto self = static_cast<HttpRequestParser*>(p->data);
  if( self->_last_cb != ON_HEADER_FIELD ){
    self->_buffer.clear();
  }
  self->_buffer.append(data, length);
  self->_last_cb = ON_HEADER_FIELD;
  return 0;
}

// Function: _on_header_value
int HttpRequestParser::_on_header_value(http_parser* p, const char* data, size_t length)  {
  auto self = static_cast<HttpRequestParser*>(p->data);
  if( self->_last_cb == ON_HEADER_FIELD ) {
    self->_request.headers.emplace(self->_buffer, "");
  }
  self->_request.headers.at(self->_buffer).append(data, length);
  self->_last_cb = ON_HEADER_VALUE;
  return 0;
}

// Function: _on_headers_complete
int HttpRequestParser::_on_headers_complete(http_parser* p) {
  auto self = static_cast<HttpRequestParser*>(p->data);
  self->_buffer.clear();
  self->_request.method = static_cast<http_method>(p->method);
  self->_request.keep_alive = http_should_keep_alive(p) != 0;
  self->_last_cb = ON_HEADERS_COMPLETE;
  return 0;
}

// Function: _on_body
int HttpRequestParser::_on_body(http_parser* p, const char* data, size_t length) {
  auto self = static_cast<HttpRequestParser*>(p->data);
  self->_request.body.append(data, length);
  self->_last_cb = ON_BODY;
  return 0;
}

// Function: _on_message_complete
int HttpRequestParser::_on_message_complete(http_parser* p) {
  auto self = static_cast<HttpRequestParser*>(p->data);
  self->_requests.push(std::move(self->_request));
  self->_last_cb = ON_MESSAGE_COMPLETE;
  return 0;
}

//---- independent functions. -----------------------------

// TODO: replace content_type with ext
HttpResponse::HttpResponse(
  int s,
  HttpBodyType b,
  std::string_view extension,
  std::string_view in_body,
  bool alive
) :
  status {s},
  body_type {b},
  body {in_body},
  content_type {MIME_TYPES.at(extension)},
  connection {alive ? "keep-alive" : "close"} {

  //headers["Content-Type"] = MIME_TYPES.at(extension);
  //headers["Connection"] = alive ? "keep-alive" : "close";
}

std::string HttpResponse::to_string() const {

  std::ostringstream os;
  
  // Http version.
  os << HTTP_VERSION << ' ' << status << ' ' << HTTP_STATUS.at(status) << "\r\n";
  
  // According to HTTP 1.1, "Date" header is required
  auto t = std::time(nullptr);
  os << "Date: " << std::put_time(std::gmtime(&t), "%c %Z") << "\r\n";

  // Content type.
  os << "Content-Type: " << content_type << "\r\n";

  // Connection
  os << "Connection: " << connection << "\r\n";
  
  // Key-value
  //for( auto const &[k,v] : headers ) {
  //  os << k << ": " << v << "\r\n";
  //}

  switch(os << "Content-Length: "; body_type){

    case HttpBodyType::FILE: 
      os << std::filesystem::file_size(body) << "\r\n\r\n";
    break;

    case HttpBodyType::QUERY:
      os << body.size() << "\r\n\r\n" << body;
    break;

    default:
      os << 0 << "\r\n\r\n";
    break;
  }
  
  //std::cout << "response to string = " << os.str() << std::endl;

  return os.str();
}

// Function: send_response
std::streamsize send_response(int fd, HttpResponse&& resp) {
  
  auto data = resp.to_string();

  std::streamsize byte_sent = write_all(fd, data.c_str(), data.size());
  
  // Send the file to the fd.
  if(resp.body_type == HttpBodyType::FILE) {
    byte_sent = write_all(fd, resp.body);
  }

  return byte_sent;
}

}; // End of namespace dtc. --------------------------------------------------- 

