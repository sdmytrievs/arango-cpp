#include <iostream>
#include "jsonarango/arangoconnect.h"
#include "jsonarango/arangoexception.h"
#include "arangodetail.h"
#include "curlobjects_pool.h"

namespace arangocpp {

CurlPool& pool_connect()
{
    static  CurlPool data;
    return  data;
}

CurlPool::CurlPool()
{
    curl_global_init(CURL_GLOBAL_ALL);
}


CurlPool::~CurlPool()
{
    curl_global_cleanup();
}

size_t RequestCurlObject::headerCallback(
        char* data, size_t size, size_t nitems, std::string *buffer)
{
    size_t realsize = size * nitems;
    size_t result = 0;

    if (buffer != nullptr)
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

    if (buffer != nullptr )
    {
        buffer->append(data, realsize);
        result = realsize;
    }
    return result;
}


int RequestCurlObject::sendRequest( const std::string& theURL, std::unique_ptr<HttpMessage> request )
{
    //std::cout <<  "curl URL Request \n" << _URL << std::endl;

    _URL = theURL;
    _request = (std::move(request));
    _responseHeaders.clear();
    _responseBody.clear();

    if (_curlHeaders != nullptr)
    {
        curl_slist_free_all(_curlHeaders);
        _curlHeaders = nullptr;
    }

    if( !_curl )
        ARANGO_THROW( "RequestCurlObject", 2, "Curl did not initialize!");


    auto aRequest = _request.get();
    bool sendJson = !ArangoDBConnection::use_velocypack_put;

    for (auto const& header : aRequest->header.meta)
    {
        if( header.first == fu_content_type_key && sendJson  )
            continue;
        std::string thisHeader(header.first + ": " + header.second);
        _curlHeaders = curl_slist_append(_curlHeaders, thisHeader.c_str());
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _curlHeaders);
    curl_easy_setopt(_curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(_curl, CURLOPT_URL, _URL.c_str());

    // clear last command
    curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);

    auto verb = aRequest->header.restVerb;
    switch (verb)
    {
    case RestVerb::Post:
        curl_easy_setopt(_curl, CURLOPT_POST, 1);
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL );
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
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL );
        break;

    case RestVerb::Illegal:
        ARANGO_THROW( "RequestCurlObject", 3, "Invalid request type " + to_string(verb));
        break;
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(_curl, CURLOPT_USERNAME, _dbUser.c_str());
    curl_easy_setopt(_curl, CURLOPT_PASSWORD, _dbPasswd.c_str());

    // clear
    //curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, -1);

    if( !sendJson )
    {
        if (aRequest->payloadSize() > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, aRequest->payloadSize());
            curl_easy_setopt(_curl, CURLOPT_COPYPOSTFIELDS, aRequest->payloadData());
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
    if( res != CURLE_OK )
        std::cout << "Curl finish " << res << std::endl;
    return res;
}

RequestCurlObject::RequestCurlObject():
    _URL(""), _dbUser(""), _dbPasswd(""),
    _request(nullptr), _curl(nullptr), _curlHeaders(nullptr)
{
    _curl = curl_easy_init();
    if(!_curl)
        ARANGO_THROW( "RequestCurlObject", 2, "Curl did not initialize!");


    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &RequestCurlObject::bodyCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_responseBody);
    curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, &RequestCurlObject::headerCallback);
    curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_responseHeaders);

#ifndef __unix  // Windows
    //curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER,false);
#endif

}

void RequestCurlObject::setConnectData(const std::string &theUser, const std::string &thePasswd)
{
    _dbUser = theUser;
    _dbPasswd = thePasswd;

    if( !_curl )
        ARANGO_THROW( "RequestCurlObject", 2, "Curl did not initialize!");

    curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(_curl, CURLOPT_USERNAME, _dbUser.c_str());
    curl_easy_setopt(_curl, CURLOPT_PASSWORD, _dbPasswd.c_str());
}

std::unique_ptr<HttpMessage> RequestCurlObject::getResponse()
{
    std::unique_ptr<HttpMessage> response = createResponse();

    long httpStatusCode = 200;
    curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
    response->header.responseCode = static_cast<unsigned>(httpStatusCode);

    if (_responseHeaders.length())
    {

        //std::cout << "_responseHeaders  " << _responseHeaders << std::endl;
        auto headers = detail::split(_responseHeaders, ";");

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
                detail::trim(value);
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
    else
    {
        response->header.addMeta(fu_content_type_key, fu_content_type_vpack);
    }

    return response;
}

namespace detail {


// "a;b;c" to array { "a", "b", "c" }
std::queue<std::string> split(const std::string& str, const std::string& delimiters)
{
    std::queue<std::string> v;

    if( str.empty() )
        return v;

    std::string::size_type start = 0;
    auto pos = str.find_first_of(delimiters, start);
    while(pos != std::string::npos)
    {
        if(pos != start) // ignore empty tokens
        {
            auto vv = std::string(str, start, pos - start);
            trim(vv);
            v.push( vv );
        }
        start = pos + 1;
        pos = str.find_first_of(delimiters, start);
    }
    if(start < str.length()) // ignore trailing delimiter
        v.push( std::string (str, start, str.length() - start) );
    return v;
}

} // namespace detail

} // namespace arangocpp
