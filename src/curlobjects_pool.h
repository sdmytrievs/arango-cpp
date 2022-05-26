// https://stackoverflow.com/questions/27827923/c-object-pool-that-provides-items-as-smart-pointers-that-are-returned-to-pool !!!!

#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <mutex>
#include "arangocurl.h"


namespace arangocpp {

class CurlPool;

/// Object Pools are usually implemented as Singletons
CurlPool& pool_connect();


/// A C ++ object pool that provides streaming request objects
/// as smart pointers that return to the pool.
class CurlPool
{

    CurlPool();
    friend CurlPool& pool_connect();

public:

    using ptr_type = std::unique_ptr<RequestCurlObject>;

    virtual ~CurlPool();

    ptr_type get_resource();

    void return_resource( ptr_type t)
    {
        std::lock_guard<std::mutex> guard(pool_mutex);
        pool_.push(std::move(t));
    }

private:
   std::stack<std::unique_ptr<RequestCurlObject>> pool_;
   std::mutex pool_mutex;

};


} // arangocpp
