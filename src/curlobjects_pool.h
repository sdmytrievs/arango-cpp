// https://stackoverflow.com/questions/27827923/c-object-pool-that-provides-items-as-smart-pointers-that-are-returned-to-pool !!!!

#include <functional>
#include <iostream>
#include <memory>
#include <stack>
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

    ptr_type get_resource()
    {
        if( pool_.empty() )
        {
      //      std::cout << "Add Streaming Requests object " << std::endl;
            pool_.push(std::make_unique<RequestCurlObject>());
        }
        ptr_type tmp(pool_.top().release());
        pool_.pop();
        return tmp;
    }

    void return_resource( ptr_type t)
    {
        pool_.push(std::move(t));
    }


private:
   std::stack<std::unique_ptr<RequestCurlObject>> pool_;

};


} // arangocpp
