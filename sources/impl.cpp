#include "header.hpp"

#include "pugixml.hpp"

namespace fs = std::filesystem;


void RecourceDistributor::distributeRecources()
{
    for (const auto& serv_file: fs::directory_iterator(serv_dir_)) {
        for (const auto& req_file: fs::directory_iterator(req_dir_)) {
            auto req_conf = parse_xml_data(req_file.path().string());
            auto serv_conf = parse_xml_data(serv_file.path().string());

            print_depl_to_file(algorithm(req_conf, serv_conf));
        }
    }
}

ParsingResult
RecourceDistributor::parse_xml_data(const std::string& input_file)
{
    pugi::xml_document doc;
    if (!doc.load_file(input_file.c_str())) {
        throw std::string("Can't load XML file");
    }

    CharactVect output_vect;
    
    auto conf = doc.child("configuration");
    for (const auto& node: conf) {
        output_vect.emplace_back(std::make_pair(
                node.attribute("core_num").as_int(),
                node.attribute("ram").as_int()));
    }

    return ParsingResult(std::move(conf.attribute("n").as_int()),
                         std::move(output_vect));
}

VmDeployment RecourceDistributor::algorithm(ParsingResult& req_conf,
                                            ParsingResult& serv_conf)
{
    // <debug_log>
    std::cerr << "\n\n=== REQUESTS ===" << std::endl;
    std::cerr << "req_conf_num = " << req_conf.conf_num << std::endl;
    for (const auto& req: req_conf.charact_vect) {
        std::cerr << req.first << " : "
                  << req.second
                  << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "=== SERVERS ===" << std::endl;
    std::cerr << "serv_conf_num = " << serv_conf.conf_num << std::endl;
    for (const auto& serv: serv_conf.charact_vect) {
        std::cerr << serv.first << " : "
                  << serv.second
                  << std::endl;
    }
    // </debug_log>


    unsigned sum_core_num_vm = 0;
    unsigned sum_ram_vm = 0;
    unsigned sum_core_num_serv = 0;
    unsigned sum_ram_serv = 0;
    for (const auto& req: req_conf.charact_vect) {
        sum_core_num_vm += req.first;
        sum_ram_vm += req.second;
    }

    for (const auto& serv: serv_conf.charact_vect) {
        sum_core_num_serv += serv.first;
        sum_ram_serv += serv.second;
    }

    if (static_cast<double>(sum_core_num_serv) / sum_core_num_vm <
        static_cast<double>(sum_ram_serv) / sum_ram_vm) {

        auto comparator = [](auto& left, auto& right)
                          {
                              return left.first > right.first;
                          };

        std::sort(req_conf.charact_vect.begin(),
                  req_conf.charact_vect.end(),
                  comparator);

        std::sort(serv_conf.charact_vect.begin(),
                  serv_conf.charact_vect.end(),
                  comparator);
    
    } else {
        auto comparator = [](auto& left, auto& right)
                          {
                              return left.second > right.second;
                          };

        std::sort(req_conf.charact_vect.begin(),
                  req_conf.charact_vect.end(),
                  comparator);

        std::sort(serv_conf.charact_vect.begin(),
                  serv_conf.charact_vect.end(),
                  comparator);
    }

    // <debug_log>
    std::cerr << "\n\n=== REQUESTS ===" << std::endl;
    std::cerr << "req_conf_num = " << req_conf.conf_num << std::endl;
    for (const auto& req: req_conf.charact_vect) {
        std::cerr << req.first << " : "
                  << req.second
                  << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "=== SERVERS ===" << std::endl;
    std::cerr << "serv_conf_num = " << serv_conf.conf_num << std::endl;
    for (const auto& serv: serv_conf.charact_vect) {
        std::cerr << serv.first << " : "
                  << serv.second
                  << std::endl;
    }
    // </debug_log>
    



    return VmDeployment();
}
    
void RecourceDistributor::print_depl_to_file(const VmDeployment& vm_depl)
{

}
