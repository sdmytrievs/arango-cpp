#ifndef ARREQUESTS_H
#define ARREQUESTS_H

#include <boost/asio/buffer.hpp>
#include <velocypack/Parser.h>
#include <string>
#include <map>

namespace jsonio { namespace arangodb {

const std::string fu_content_type_key("content-type");
const std::string fu_accept_key("accept");
const std::string fu_content_type_vpack("application/x-velocypack");
const std::string fu_content_type_json("application/json");

using StringMap = std::map<std::string, std::string>;

/// Response status
using StatusCode = uint32_t;

StatusCode const StatusOK = 200;
StatusCode const StatusCreated = 201;
StatusCode const StatusAccepted = 202;
StatusCode const StatusBadRequest = 400;
StatusCode const StatusUnauthorized = 401;
StatusCode const StatusForbidden = 403;
StatusCode const StatusNotFound = 404;
StatusCode const StatusMethodNotAllowed = 405;
StatusCode const StatusConflict = 409;


///  HTTP method ( CURLOPT_CUSTOMREQUEST)
enum class RestVerb
{
  Illegal = -1,
  Delete = 0,
  Get = 1,
  Post = 2,
  Put = 3,
  Head = 4,
  Patch = 5,
  Options = 6
};


enum class MessageType
{
  Undefined = 0,
  Request = 1,
  Response = 2
};


/// Message Data
struct MessageHeader
{
  int version;
  MessageType type;        // Type of message
  StatusCode responseCode; // Response code (response only)
  RestVerb restVerb;       // HTTP method        (GET POST ...)
  std::string path;        // Local path of the request ( equivalent of http path)
  StringMap parameters;    // Query parameters   (equivalent of http parametes ?foo=bar)
  StringMap meta;          // Header meta data   (equivalent of http headers)

  std::string contentTypeString() const;
  // query parameter helpers
  void addParameter(std::string const& key, std::string const& value)
  {
      parameters[key] = value;
  }
  // Header metadata helpers
  void addMeta(std::string const& key, std::string const& value)
  {
    meta[key] = value;
  }
  // Get value for header metadata key, returns empty std::string if not found.
  std::string metaByKey( std::string const& key) const;
};

// Request or Response Message a server.
class HttpMessage {

protected:

  HttpMessage(MessageHeader&& messageHeader = MessageHeader(), StringMap&& headerStrings = StringMap())
    : header(std::move(messageHeader)), _payloadLength(0)
         {
            header.meta = std::move(headerStrings);
            header.type = MessageType::Undefined;
            setContentType();
         }

  HttpMessage(MessageHeader const& messageHeader, StringMap const& headerStrings)
    : header(messageHeader), _payloadLength(0)
         {
            header.meta = std::move(headerStrings);
            header.type = MessageType::Undefined;
            setContentType();
         }

  /// Use default x-velocypack protocol
  void setContentType();

public:

  MessageHeader header;

  void addVPack(::arangodb::velocypack::Buffer<uint8_t>&& buffer);  // bilder.steal()
  void addVPack(::arangodb::velocypack::Slice const& slice);

  std::vector<::arangodb::velocypack::Slice>const & slices();
  boost::asio::const_buffer payload() const
  {
     return boost::asio::const_buffer(_payload.data(), _payloadLength);
  }

  std::string payloadAsString();

  bool isContentTypeVPack() const;
  bool isContentTypeJson() const;
  StatusCode statusCode() { return header.responseCode; }

  friend std::unique_ptr<HttpMessage> createRequest( MessageHeader&& messageHeader,
                 StringMap&& headerStrings,  RestVerb const& verb   );
  friend std::unique_ptr<HttpMessage> createResponse(  MessageHeader&& messageHeader,
                 StringMap&& headerStrings );
private:

  ::arangodb::velocypack::Buffer<uint8_t> _payload;
  std::size_t _payloadLength;
  std::vector<::arangodb::velocypack::Slice> _slices;

};

std::unique_ptr<HttpMessage> createRequest( RestVerb const& verb );
std::unique_ptr<HttpMessage> createRequest(
        RestVerb verb, std::string const& path, StringMap const& parameters );
std::unique_ptr<HttpMessage> createResponse( );

std::string to_string(RestVerb type);
std::string to_string(HttpMessage& message);

} }

#endif // ARREQUESTS_H
