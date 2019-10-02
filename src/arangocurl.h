#pragma once

#include <curl/curl.h>
#include "jsonarango/arangorequests.h"

// https://news.ycombinator.com/item?id=7695676
// https://stackoverflow.com/questions/32852302/why-are-streaming-requests-with-libcurl-in-c-slow
// https://gist.github.com/bagder/7eccf74f8b6d70b5abefeb7f288dba9b

namespace arangocpp {

class RequestCurlObject {

  public:

    RequestCurlObject( const std::string& theURL, const std::string& theUser,
                       const std::string& thePasswd,
                       std::unique_ptr<HttpMessage> request );

    ~RequestCurlObject()
    {
      if (_curlHeaders != nullptr)
        curl_slist_free_all(_curlHeaders);
      if (_curl != nullptr)
        curl_easy_cleanup(_curl);
    }

    static size_t bodyCallback(  char* pdatatr, size_t size, size_t nmemb, std::string* buffer);
    static size_t headerCallback( char* data, size_t size, size_t nitems, std::string* buffer);


    std::unique_ptr<HttpMessage> getResponse();

    std::string jsonBody()
    {
        std::string data;
        if (_responseBody.length())
        {
              ::arangodb::velocypack::Buffer<uint8_t> buffer;
              buffer.append(_responseBody);
              ::arangodb::velocypack::Slice slice(buffer.data());
              data = slice.toJson();
       }
       return data;
    }


  protected:

    std::string _URL;
    std::string _dbUser;
    std::string _dbPasswd;
    std::unique_ptr<HttpMessage> _request;
    std::string _responseHeaders;
    std::string _responseBody;

    CURL* _curl;
    struct curl_slist* _curlHeaders;
};


} // arangocpp


/*

curl -X PUT --data-binary @- --dump - https://db.cemgems.app/_db/cemgems/_api/simple/all -u backend:"cement consumption in megatons" <<EOF
{ "collection": "anonym" }
EOF

curl -u username:password http://example.com

curl -u adminrem:"Administrator@Remote-ThermoHub-Server" https://db.thermohub.net/_db/hub_test/_api/collection/docpages/properties

curl -u adminrem:"Administrator@Remote-ThermoHub-Server" https://db.thermohub.net/_db/hub_test/_api/collection/elements/properties

curl -u adminrem:"Administrator@Remote-ThermoHub-Server" https://db.thermohub.net/_db/hub_test/_api/collection

*/


