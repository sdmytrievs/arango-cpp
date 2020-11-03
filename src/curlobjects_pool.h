// https://stackoverflow.com/questions/27827923/c-object-pool-that-provides-items-as-smart-pointers-that-are-returned-to-pool !!!!

#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include "jsonarango/arangocurl.h"

// https://curl.haxx.se/libcurl/c/10-at-a-time.html
// https://pocoproject.org/download.html
// https://stackoverflow.com/questions/44885935/libcurl-use-1-connection-for-multiple-concurrent-requests
/// https://curl.haxx.se/libcurl/c/http2-download.html
/// https://curl.haxx.se/libcurl/c/http2-upload.html
/// https://habr.com/ru/post/349818/

namespace arangocpp {

class CurlPool;
CurlPool& pool_connect();

class CurlPool
{

    CurlPool();
    friend CurlPool& pool_connect();

public:

    using ptr_type = std::unique_ptr<RequestCurlObject>;

    virtual ~CurlPool();

    ptr_type get_resource()
    {
        if( pool_.empty() )
        {
            std::cout << "Add curl " << std::endl;
            pool_.push(std::make_unique<RequestCurlObject>());
        }
        ptr_type tmp(pool_.top().release());
        pool_.pop();
        return tmp;
    }

    void return_resource(std::unique_ptr<RequestCurlObject> t)
    {
        pool_.push(std::move(t));
    }


private:

   std::stack<std::unique_ptr<RequestCurlObject>> pool_;

};


} // arangocpp
