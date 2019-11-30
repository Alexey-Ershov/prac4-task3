#ifndef HEADER
#define HEADER


#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <functional>
#include <sstream>


struct Item
{
    unsigned num;
    unsigned core_num;
    unsigned ram;

    Item(unsigned n = 0, unsigned c_n = 0, unsigned r = 0)
        : num {n},
          core_num {c_n},
          ram {r}
    {}
};

// Vector of pairs with cores number and RAM.
using CharactVect = std::vector<Item>;
using CharactVectWithIndices = std::vector<std::pair<Item, unsigned>>;

template <typename T = CharactVect>
struct ParsingResult
{
    int conf_num;
    T charact_vect;

    ParsingResult() = default;

    ParsingResult(const int&& cn, const T&& cv)
        : conf_num {cn},
          charact_vect {cv}
    {}
};

struct VmDeployment
{
    // # VM -> # Server.
    std::unordered_map<unsigned, unsigned> vm_mapping;
    unsigned deployed_vm_num;
    bool was_all_vm_deployed;

    VmDeployment()
        : deployed_vm_num {0},
          was_all_vm_deployed {false}
    {}
};

class RecourceDistributor
{
public:
    RecourceDistributor(unsigned limit,
                        std::string req_dir = "../id/requests",
                        std::string serv_dir = "../id/servers")
        : limit_ {limit},
          req_dir_ {req_dir},
          serv_dir_ {serv_dir}
        {}

    void distributeRecources();

private:
    // Searching depth of algorithm.
    unsigned limit_;

    std::string req_dir_;
    std::string serv_dir_;

    // Internal XML data representation: vector of characteristics and
    // number of configuration.
    /*ParsingResult req_conf_;
    ParsingResult serv_conf_;*/

    unsigned first_available_serv_num_ = 0;

    // Flag showing that number of cores is critical resource.
    bool core_num_is_critical_ = false;

    ParsingResult<> parse_xml_data(const std::string& input_file);
    VmDeployment algorithm(ParsingResult<>& req_conf,
                           ParsingResult<>& serv_conf,
                           CharactVect& serv_load,
                           bool enable_lim_sch = true);
    
    void print_depl_to_file(const VmDeployment& vm_depl,
                            const ParsingResult<>& req_conf,
                            const ParsingResult<>& serv_conf);
    
    bool try_deploy_vm(VmDeployment& vm_delp,
                       unsigned vm_num,
                       unsigned serv_num,
                       const ParsingResult<>& req_conf,
                       const ParsingResult<>& serv_conf,
                       CharactVect& serv_load);
    
    void limited_searching(VmDeployment& vm_delp,
                           unsigned vm_num,
                           const ParsingResult<>& req_conf,
                           const ParsingResult<>& serv_conf,
                           CharactVect& serv_load);
};


#endif
