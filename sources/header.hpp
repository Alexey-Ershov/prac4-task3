#ifndef HEADER
#define HEADER


#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <map>


// Vector of pairs with cores number and RAM.
using CharactVect = std::vector<std::pair<int, int>>;


struct VmDeployment
{
    // # VM -> # Server.
    std::map<int, int> vm_mapping;
    unsigned deployed_vm_num;
    bool was_all_vm_deployed;
};

class RecourceDistributor
{
public:
    RecourceDistributor(std::string req_dir = "../id/requests",
                        std::string serv_dir = "../id/servers")
        : req_dir_ {req_dir},
          serv_dir_ {serv_dir}
        {}

    void distributeRecources();

private:
    std::string req_dir_;
    std::string serv_dir_;

    CharactVect parse_xml_data(const std::string& input_file);
    VmDeployment algorithm(const CharactVect& requests,
                           const CharactVect& servers);
    void print_depl_to_file(VmDeployment);
};


#endif
