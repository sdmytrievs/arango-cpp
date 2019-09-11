#include <sstream>
#include "arangorequests.h"
#include "arangoexception.h"
//#include "jsonio/io_settings.h"

namespace arangocpp {

std::string to_string(MessageType type);
std::string to_string(const MessageHeader& header);

// class MessageHeader

std::string MessageHeader::contentTypeString() const
{
  return metaByKey(fu_content_type_key);
}

// Get value for header metadata key, returns empty string if not found.
std::string MessageHeader::metaByKey(std::string const& key) const
{
  if (meta.empty())
    return "";
  auto found =  meta.find(key);
  return (found == meta.end()) ? "" : found->second;
}

bool HttpMessage::isContentTypeVPack() const
{
  return (header.contentTypeString() == fu_content_type_vpack || _payloadLength );
}

bool HttpMessage::isContentTypeJson() const
{
  return (header.contentTypeString() == fu_content_type_json  );
}

void HttpMessage::setContentType()
{

    //if( ioSettings().PutVelocypack() )
    header.addMeta(fu_content_type_key, fu_content_type_vpack);

    if( ioSettings().useVelocypackGet() )
        header.addMeta(fu_accept_key, fu_content_type_vpack);
}

void HttpMessage::addVPack(::arangodb::velocypack::Buffer<uint8_t>&& buffer)
{
  _slices.clear();
  _payloadLength += buffer.byteSize();
  _payload = std::move(buffer);
  _payload.resetTo(_payloadLength);
}

void HttpMessage::addVPack(::arangodb::velocypack::Slice const& slice)
{
  _slices.clear();
  auto _builder = std::make_shared<::arangodb::velocypack::Builder>(_payload);
  _builder->add(slice);
  _payloadLength += slice.byteSize();
  _payload.resetTo(_payloadLength);
}

std::vector<::arangodb::velocypack::Slice>const & HttpMessage::slices()
{
  if (_slices.empty())
  {
    auto length = _payload.byteSize();
    auto cursor = _payload.data();
    while (length)
    {
      _slices.emplace_back(cursor);
      auto sliceSize = _slices.back().byteSize();
      if (length < sliceSize){
        ARANGO_THROW( "HttpMessage", 1, "Invalid buffer" );
      }
      cursor += sliceSize;
      length -= sliceSize;
    }
  }
  return _slices;
}

std::string HttpMessage::payloadAsString()
{
  if(  isContentTypeVPack() )
  {
      ::arangodb::velocypack::Options options;
      options.unsupportedTypeBehavior = ::arangodb::velocypack::Options::ConvertUnsupportedType;
      options.buildUnindexedArrays = true;
      auto jsonstr = slices().front().toJson(&options);
      return jsonstr;
  }
  else
  {
      auto p = payload();
      return std::string(boost::asio::buffer_cast<char const*>(p), boost::asio::buffer_size(p));
  }
}
//-------------------------------------------------------------

// Helper and Implementation
std::unique_ptr<HttpMessage> createRequest(
        MessageHeader&& messageHeader, StringMap&& headerStrings, RestVerb const& verb )
{
  auto request = std::unique_ptr<HttpMessage>(new HttpMessage(std::move(messageHeader),std::move(headerStrings)));
  request->header.restVerb = verb;
  request->header.type = MessageType::Request;
  return request;
}

std::unique_ptr<HttpMessage> createRequest( RestVerb const& verb )
{
  return createRequest(MessageHeader(), StringMap(), verb );
}

std::unique_ptr<HttpMessage> createRequest(
        RestVerb verb, std::string const& path, StringMap const& parameters )
{
  auto request = createRequest(verb );
  request->header.path = path;
  request->header.parameters = parameters;
  return request;
}

std::unique_ptr<HttpMessage> createResponse(
        MessageHeader&& messageHeader, StringMap&& headerStrings )
{
  auto request = std::unique_ptr<HttpMessage>(new HttpMessage(std::move(messageHeader),std::move(headerStrings)));
  request->header.type = MessageType::Response;
  return request;
}

std::unique_ptr<HttpMessage> createResponse( )
{
  return createResponse(MessageHeader(), StringMap() );
}


//----------------------------------------------------------------------

std::string to_string(RestVerb type)
{
  switch (type)
  {
    case RestVerb::Illegal:
      return "illegal";

    case RestVerb::Delete:
      return "delete";

    case RestVerb::Get:
      return "get";

    case RestVerb::Post:
      return "post";

    case RestVerb::Put:
      return "put";

    case RestVerb::Head:
      return "head";

    case RestVerb::Patch:
      return "patch";

    case RestVerb::Options:
      return "options";
  }
  return "undefined";
}

std::string to_string(MessageType type)
{
  switch (type)
  {
    case MessageType::Undefined:
      return "undefined";

    case MessageType::Request:
      return "request";

    case MessageType::Response:
      return "response";

  }
  return "undefined";
}

std::string to_string(const MessageHeader& header)
{
  std::stringstream ss;

  ss << "version: " << header.version  << std::endl;
  ss << "type: " << static_cast<int>(header.type ) << std::endl;
  ss << "responseCode: " << header.responseCode  << std::endl;
  ss << "restVerb: " << to_string(header.restVerb ) << std::endl;
  ss << "path: " << header.path << std::endl;

  if( !header.parameters.empty() )
  {
    ss << "parameters: ";
    for(auto const& item : header.parameters)
      ss << item.first <<  " -:- " << item.second << "\n";
    ss<< std::endl;
  }

  ss << "meta:\n";
  for(auto const& item : header.meta)
      ss << "\t" << item.first <<  " -:- " << item.second << "\n";
  ss<< std::endl;

  ss << "contentType: " << header.contentTypeString() << std::endl;

  return ss.str();
}

//message is not const because message.slices is not
std::string to_string(HttpMessage& message)
{
  std::stringstream ss;
  ss << "\n#### Message #####################################\n";
  ss << "Header:\n";
  ss << to_string(message.header);
  ss << "\nBody:\n";
  ss << message.payloadAsString();
  ss << "\n";
  ss << "##################################################\n";
  return ss.str();
}

} // namespace arangocpp
