#include "header.hpp"


int main(int argc, char const* argv[])
{
    try {
        unsigned limit = 2;
        if (argc >= 2) {
            limit = std::stoi(argv[1]);
        }

        std::cerr << "MAIN DEBUG: limit = " << limit << std::endl;

        RecourceDistributor rd(limit);
        rd.distributeRecources();
    
    // Bad cast, the user has inputed some dirt instead of integers.
    } catch (const std::invalid_argument& exception) {
        std::cerr << "Error: Bad cast" << std::endl;
        return -1;
    
    } catch (const std::string& err_message) {
        std::cerr << err_message << std::endl;
        return -2;
    }
    
    return 0;
}
