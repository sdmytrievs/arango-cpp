#include <iostream>
#include "arangocurl.h"
//#include "jsonio/nservice.h"
//#include "jsonio/io_settings.h"

namespace jsonio { namespace arangodb {


size_t RequestCurlObject::headerCallback(
        char* data, size_t size, size_t nitems, std::string *buffer)
{
    size_t realsize = size * nitems;
    size_t result = 0;

    if (buffer != NULL)
    {
        buffer->append(data, realsize);
        buffer->append(";", 1);
        result = realsize;
    }
    return result;
}

size_t RequestCurlObject::bodyCallback(
        char* data, size_t size, size_t nitems, std::string *buffer)
{
    size_t realsize = size * nitems;
    size_t result = 0;

    if (buffer != NULL)
    {
        buffer->append(data, realsize);
        result = realsize;
    }
    return result;
}


RequestCurlObject::RequestCurlObject( const std::string& theURL, const std::string& theUser,
                                      const std::string& thePasswd,
                                      std::unique_ptr<HttpMessage> request ):
    _URL(theURL), _dbUser(theUser), _dbPasswd(thePasswd),
    _request(std::move(request)), _curl(nullptr), _curlHeaders(nullptr)
{
    _curl = curl_easy_init();
    if(!_curl)
        ::jsonio::jsonioErr("RequestCurlObject", "Curl did not initialize!");

    auto aRequest = _request.get();
    bool sendJson = !ioSettings().useVelocypackPut();

    for (auto const& header : aRequest->header.meta)
    {
        if( header.first == fu_content_type_key && sendJson  )
             continue;
        std::string thisHeader(header.first + ": " + header.second);
        _curlHeaders = curl_slist_append(_curlHeaders, thisHeader.c_str());
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _curlHeaders);
    curl_easy_setopt(_curl, CURLOPT_HEADER, 0L);

    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &RequestCurlObject::bodyCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_responseBody);
    curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, &RequestCurlObject::headerCallback);
    curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_responseHeaders);

    curl_easy_setopt(_curl, CURLOPT_URL, _URL.c_str());

#ifndef __unix  // Windows
    //curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER,false);
#endif
    auto verb = aRequest->header.restVerb;
    switch (verb)
    {
    case RestVerb::Post:
        curl_easy_setopt(_curl, CURLOPT_POST, 1);
        break;

    case RestVerb::Put:
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
        break;

    case RestVerb::Delete:
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;

    case RestVerb::Head:
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "HEAD");
        curl_easy_setopt(_curl, CURLOPT_NOBODY, 1L );
        break;

    case RestVerb::Patch:
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        break;

    case RestVerb::Options:
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
        break;

    case RestVerb::Get:
        break;

    case RestVerb::Illegal:
        ::jsonio::jsonioErr( "runtime_error", "Invalid request type " + to_string(verb));
        break;
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(_curl, CURLOPT_USERNAME, _dbUser.c_str());
    curl_easy_setopt(_curl, CURLOPT_PASSWORD, _dbPasswd.c_str());

    //std::cout <<  "curl URL Request \n" << _URL << std::endl;

    if( !sendJson )
    {
        auto pay = aRequest->payload();
        auto paySize = boost::asio::buffer_size(pay);

        if (paySize > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, paySize);
            curl_easy_setopt(_curl, CURLOPT_COPYPOSTFIELDS, boost::asio::buffer_cast<const char*>(pay));
        }
    }
    else
    {
        std::string jsonrequest = aRequest->payloadAsString();
        //std::cout <<  "curl json Request body \n" <<jsonrequest << std::endl;
        size_t paySize = jsonrequest.length();

        if (paySize > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, paySize);
            curl_easy_setopt(_curl, CURLOPT_COPYPOSTFIELDS, jsonrequest.c_str());
        }
    }

    auto res = curl_easy_perform(_curl);
    if(res != CURLE_OK)
          std::cout << "Curl finish " << res << std::endl;
}

std::unique_ptr<HttpMessage> RequestCurlObject::getResponse()
{
    std::unique_ptr<HttpMessage> response = createResponse();

    long httpStatusCode = 200;
    curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
    response->header.responseCode = static_cast<unsigned>(httpStatusCode);

    if (_responseHeaders.length())
    {

        //cout << "_responseHeaders  " << _responseHeaders << endl;
        auto headers = ::jsonio::split(_responseHeaders, ";");

        while( !headers.empty() )
        {
            auto header = headers.front();
            headers.pop();
            size_t pit = header.find_first_of(':');

            if (pit != std::string::npos)
            {
                auto key =  header.substr( 0 , pit);
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                if( pit + 2 >= header.length() )
                    continue;
                auto value = header.substr( pit + 2 );
                ::jsonio::strip_all(value, " \t\n\r");
                response->header.addMeta( key, value);
            }
        }
    }

    if (_responseBody.length())
    {
        if( response->isContentTypeJson() )
        {  // Some request bad with fu_content_type_vpack
            auto data = ::arangodb::velocypack::Parser::fromJson(_responseBody);
            response->addVPack(data->slice());
            response->header.addMeta(fu_content_type_key, fu_content_type_vpack);

           // std::cout <<  "curl json response body " << std::endl;
        }
        else
        { ::arangodb::velocypack::Buffer<uint8_t> buffer;
            buffer.append(_responseBody);
            response->addVPack(std::move(buffer));
        }
    }

    return response;
}

} }
