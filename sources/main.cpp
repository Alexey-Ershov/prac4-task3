#include "header.hpp"


int main(int argc, char const *argv[])
{
    try {
        RecourceDistributor rd;
        rd.distributeRecources();
    
    } catch (const std::string& err_message) {
        std::cerr << err_message << std::endl;
        return -1;
    }
    
    return 0;
}
