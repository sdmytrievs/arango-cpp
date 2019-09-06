#include <iostream>
using namespace std;


int main()
{
    try{
        cout << "Hello World!" << endl;

    }
    /*catch(arango_exception& e)
    {
        std::cout <<   e.what() <<  std::endl;
    }*/
    catch(std::exception& e)
    {
        std::cout <<   "std::exception: " << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  "unknown exception" <<  std::endl;
    }

    return 0;
}


