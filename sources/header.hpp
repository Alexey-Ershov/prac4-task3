#ifndef HEADER
#define HEADER


#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <map>
#include <filesystem>
#include <functional>
#include <sstream>


struct Item
{
    unsigned num;
    unsigned core_num;
    unsigned ram;

    Item(unsigned n, unsigned c_n, unsigned r)
        : num {n},
          core_num {c_n},
          ram {r}
    {}
};

// Vector of pairs with cores number and RAM.
using CharactVect = std::vector<Item>;


struct ParsingResult
{
    int conf_num;
    CharactVect charact_vect;

    ParsingResult() = default;

    ParsingResult(const int&& cn, const CharactVect&& cv)
        : conf_num {cn},
          charact_vect {cv}
    {}
};

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

    // Internal XML data representation: vector of characteristics and
    // number of configuration.
    ParsingResult req_conf_;
    ParsingResult serv_conf_;

    unsigned first_available_serv_num_ = 0;

    // Vector containing information about current loading of the servers.
    CharactVect serv_load_;

    ParsingResult parse_xml_data(const std::string& input_file);
    VmDeployment algorithm();
    
    void print_depl_to_file(const VmDeployment& vm_depl);
    bool try_deploy_vm(VmDeployment& vm_delp,
                       unsigned vm_num,
                       unsigned serv_num);
};


#endif
