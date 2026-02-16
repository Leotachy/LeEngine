
#include "FirstApp.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    Le::FirstApp FirstApp;

    try
    {
        FirstApp.Run();
    }
    catch (const std::exception& e)
    {

        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}