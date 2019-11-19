#include "header.hpp"

#include "pugixml.hpp"

namespace fs = std::filesystem;


void RecourceDistributor::distributeRecources()
{
    for (const auto& serv_file: fs::directory_iterator(serv_dir_)) {
        for (const auto& req_file: fs::directory_iterator(req_dir_)) {
            req_conf_ = parse_xml_data(req_file.path().string());
            serv_conf_ = parse_xml_data(serv_file.path().string());

            // serv_load_ initialization.
            for (unsigned i = 0; i < serv_conf_.charact_vect.size(); i++) {
                serv_load_.emplace_back(Item(i, 0, 0));
            }

            print_depl_to_file(algorithm());
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
    unsigned i = 0;
    for (const auto& node: conf) {
        output_vect.emplace_back(Item(
                i,
                node.attribute("core_num").as_int(),
                node.attribute("ram").as_int()));

        i += 1;
    }

    return ParsingResult(std::move(conf.attribute("n").as_int()),
                         std::move(output_vect));
}

VmDeployment RecourceDistributor::algorithm()
{
    // <debug_log>
    std::cerr << "\n\n=== REQUESTS ===" << std::endl;
    std::cerr << "req_conf_num = " << req_conf_.conf_num << std::endl;
    for (const auto& req: req_conf_.charact_vect) {
        std::cerr << "#" << req.num << "   "
                  << req.core_num << " : "
                  << req.ram
                  << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "=== SERVERS ===" << std::endl;
    std::cerr << "serv_conf_num = " << serv_conf_.conf_num << std::endl;
    for (const auto& serv: serv_conf_.charact_vect) {
        std::cerr << "#" << serv.num << "   "
                  << serv.core_num << " : "
                  << serv.ram
                  << std::endl;
    }
    // </debug_log>


    unsigned sum_core_num_vm = 0;
    unsigned sum_ram_vm = 0;
    unsigned sum_core_num_serv = 0;
    unsigned sum_ram_serv = 0;
    for (const auto& req: req_conf_.charact_vect) {
        sum_core_num_vm += req.core_num;
        sum_ram_vm += req.ram;
    }

    for (const auto& serv: serv_conf_.charact_vect) {
        sum_core_num_serv += serv.core_num;
        sum_ram_serv += serv.ram;
    }

    if (static_cast<double>(sum_core_num_serv) / sum_core_num_vm <
        static_cast<double>(sum_ram_serv) / sum_ram_vm) {

        auto comparator_less = [](Item& left, Item& right)
                               {
                                   return left.core_num < right.core_num;
                               };

        auto comparator_gr = [](Item& left, Item& right)
                             {
                                 return left.core_num > right.core_num;
                             };

        std::sort(req_conf_.charact_vect.begin(),
                  req_conf_.charact_vect.end(),
                  comparator_less);

        std::sort(serv_conf_.charact_vect.begin(),
                  serv_conf_.charact_vect.end(),
                  comparator_gr);
    
    } else {
        auto comparator_less = [](Item& left, Item& right)
                               {
                                   return left.ram < right.ram;
                               };

        auto comparator_gr = [](Item& left, Item& right)
                             {
                                 return left.ram > right.ram;
                             };

        std::sort(req_conf_.charact_vect.begin(),
                  req_conf_.charact_vect.end(),
                  comparator_less);

        std::sort(serv_conf_.charact_vect.begin(),
                  serv_conf_.charact_vect.end(),
                  comparator_gr);
    }

    // <debug_log>
    std::cerr << "\n\n=== REQUESTS ===" << std::endl;
    std::cerr << "req_conf_num = " << req_conf_.conf_num << std::endl;
    for (const auto& req: req_conf_.charact_vect) {
        std::cerr << "#" << req.num << "   "
                  << req.core_num << " : "
                  << req.ram
                  << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "=== SERVERS ===" << std::endl;
    std::cerr << "serv_conf_num = " << serv_conf_.conf_num << std::endl;
    for (const auto& serv: serv_conf_.charact_vect) {
        std::cerr << "#" << serv.num << "   "
                  << serv.core_num << " : "
                  << serv.ram
                  << std::endl;
    }
    // </debug_log>

    VmDeployment vm_depl;    

    // Greedy algorithm.
    for (unsigned i = 0; i < req_conf_.charact_vect.size(); i++) {
        unsigned j = 0;
        bool was_depl_succ = false;
        while (j < serv_conf_.charact_vect.size() and !was_depl_succ) {
            was_depl_succ = try_deploy_vm(vm_depl, i, j);
            if (was_depl_succ) {
                vm_depl.deployed_vm_num += 1;
                if (vm_depl.deployed_vm_num == req_conf_.charact_vect.size()) {
                    vm_depl.was_all_vm_deployed = true;
                }
            }
            j += 1;
        }
    }


    



    return vm_depl;
}
    
void RecourceDistributor::print_depl_to_file(const VmDeployment& vm_depl)
{
    std::cerr << "\n\n" << "=== DEPL ===" << std::endl;

    for (const auto& it: vm_depl.vm_mapping) {
        std::cerr << it.first << " -> " << it.second << std::endl;
    }

    std::stringstream ss;
    ss << "Number of deployed VM: "
       << vm_depl.deployed_vm_num;

    std::stringstream dashes;
    for (unsigned i = 0; i < ss.str().length(); i++) {
        dashes << "-";
    }

    std::cerr << dashes.str() << std::endl
              << ss.str() << std::endl;

    if (vm_depl.was_all_vm_deployed) {
        std::cerr << "All VM deployed: True" << std::endl;
    
    } else {
        std::cerr << "All VM deployed: False" << std::endl;
    }

    std::cerr << dashes.str() << std::endl;
}

bool RecourceDistributor::try_deploy_vm(VmDeployment& vm_depl,
                                        unsigned vm_num,
                                        unsigned serv_num)
{
    if (serv_load_[serv_num].core_num +
        req_conf_.charact_vect[vm_num].core_num <=
        serv_conf_.charact_vect[serv_num].core_num
        and
        serv_load_[serv_num].ram +
        req_conf_.charact_vect[vm_num].ram <=
        serv_conf_.charact_vect[serv_num].ram) {
        
        vm_depl.vm_mapping[req_conf_.charact_vect[vm_num].num] =
                serv_conf_.charact_vect[serv_num].num;
        
        serv_load_[serv_num].core_num +=
                req_conf_.charact_vect[vm_num].core_num;
        
        serv_load_[serv_num].ram += req_conf_.charact_vect[vm_num].ram;

        return true;
    
    } else {
        return false;
    }
}
