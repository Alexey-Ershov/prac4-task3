#include "header.hpp"

#include "pugixml.hpp"

namespace fs = std::filesystem;


void RecourceDistributor::distributeRecources()
{
    for (const auto& serv_file: fs::directory_iterator(serv_dir_)) {
        for (const auto& req_file: fs::directory_iterator(req_dir_)) {
            ParsingResult req_conf =
                    parse_xml_data(req_file.path().string());
            
            ParsingResult serv_conf =
                    parse_xml_data(serv_file.path().string());

            // Vector containing information about current loading of
            // the servers.
            // In spite of value type Item the field .num should be ignored
            // because order of items in this container corresponds with
            // order in serv_conf_.charact_vect.
            CharactVect serv_load =
                    std::vector(serv_conf.charact_vect.size(), Item());

            print_depl_to_file(algorithm(req_conf, serv_conf, serv_load),
                               req_conf, serv_conf);
        }
    }
}

ParsingResult<>
RecourceDistributor::parse_xml_data(const std::string& input_file)
{
    pugi::xml_document doc;
    if (!doc.load_file(input_file.c_str())) {
        throw std::string("Error: Can't load XML file");
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

VmDeployment RecourceDistributor::algorithm(ParsingResult<>& req_conf,
                                            ParsingResult<>& serv_conf,
                                            CharactVect& serv_load,
                                            bool enable_lim_sch)
{
    // <debug_log>
    std::cerr << "\n\n=== REQUESTS ===" << std::endl;
    std::cerr << "req_conf_num = " << req_conf.conf_num << std::endl;
    for (const auto& req: req_conf.charact_vect) {
        std::cerr << "#" << req.num << "   "
                  << req.core_num << " : "
                  << req.ram
                  << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "=== SERVERS ===" << std::endl;
    std::cerr << "serv_conf_num = " << serv_conf.conf_num << std::endl;
    for (const auto& serv: serv_conf.charact_vect) {
        std::cerr << "#" << serv.num << "   "
                  << serv.core_num << " : "
                  << serv.ram
                  << std::endl;
    }
    std::cerr << "\n***************************************\n";
    // </debug_log>


    if (enable_lim_sch) {
        unsigned sum_core_num_vm = 0;
        unsigned sum_ram_vm = 0;
        unsigned sum_core_num_serv = 0;
        unsigned sum_ram_serv = 0;
        for (const auto& req: req_conf.charact_vect) {
            sum_core_num_vm += req.core_num;
            sum_ram_vm += req.ram;
        }

        for (const auto& serv: serv_conf.charact_vect) {
            sum_core_num_serv += serv.core_num;
            sum_ram_serv += serv.ram;
        }

        std::cerr << std::endl
                  << "DEBUG: core_ratio: "
                  << static_cast<double>(sum_core_num_serv) / sum_core_num_vm
                  << std::endl
                  << "DEBUG: ram_ratio: "
                  << static_cast<double>(sum_ram_serv) / sum_ram_vm
                  << std::endl
                  << std::endl;

        // Critical resource -> number of cores.
        if (static_cast<double>(sum_core_num_serv) / sum_core_num_vm <
            static_cast<double>(sum_ram_serv) / sum_ram_vm) {

            core_num_is_critical_ = true;

            auto comparator_less = [](const Item& left, const Item& right)
                                   {
                                       return left.core_num < right.core_num;
                                   };

            auto comparator_gr = [](const Item& left, const Item& right)
                                 {
                                     return left.core_num > right.core_num;
                                 };

            std::sort(req_conf.charact_vect.begin(),
                      req_conf.charact_vect.end(),
                      comparator_less);

            std::sort(serv_conf.charact_vect.begin(),
                      serv_conf.charact_vect.end(),
                      comparator_gr);
        
        // Critical resource -> RAM.
        } else {
            core_num_is_critical_ = false;

            auto comparator_less = [](const Item& left, const Item& right)
                                   {
                                       return left.ram < right.ram;
                                   };

            auto comparator_gr = [](const Item& left, const Item& right)
                                 {
                                     return left.ram > right.ram;
                                 };

            std::sort(req_conf.charact_vect.begin(),
                      req_conf.charact_vect.end(),
                      comparator_less);

            std::sort(serv_conf.charact_vect.begin(),
                      serv_conf.charact_vect.end(),
                      comparator_gr);
        }
    }

    // <debug_log>
    std::cerr << "\n\n=== REQUESTS ===" << std::endl;
    std::cerr << "req_conf_num = " << req_conf.conf_num << std::endl;
    for (const auto& req: req_conf.charact_vect) {
        std::cerr << "#" << req.num << "   "
                  << req.core_num << " : "
                  << req.ram
                  << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "=== SERVERS ===" << std::endl;
    std::cerr << "serv_conf_num = " << serv_conf.conf_num << std::endl;
    for (const auto& serv: serv_conf.charact_vect) {
        std::cerr << "#" << serv.num << "   "
                  << serv.core_num << " : "
                  << serv.ram
                  << std::endl;
    }
    // </debug_log>

    VmDeployment vm_depl(req_conf.conf_num, serv_conf.conf_num);

    // Greedy algorithm.
    for (unsigned i = 0; i < req_conf.charact_vect.size(); i++) {
        unsigned j = 0;
        bool was_depl_succ = false;
        while (j < serv_conf.charact_vect.size() and not was_depl_succ) {
            was_depl_succ = try_deploy_vm(vm_depl,
                                          i, j,
                                          req_conf,
                                          serv_conf,
                                          serv_load);
            if (was_depl_succ) {
                vm_depl.deployed_vm_num += 1;
                if (vm_depl.deployed_vm_num == req_conf.charact_vect.size()) {
                    vm_depl.was_all_vm_deployed = true;
                }
            
            } else if (j == serv_conf.charact_vect.size() - 1 and
                       enable_lim_sch) {
                
                limited_searching(vm_depl,
                                  i,
                                  req_conf,
                                  serv_conf,
                                  serv_load);
            }
            
            j += 1;
        }
    }

    return vm_depl;
}
    
void RecourceDistributor::print_depl_to_file(const VmDeployment& vm_depl,
                                             const ParsingResult<>& req_conf,
                                             const ParsingResult<>& serv_conf)
{
    std::ofstream ofile(output_filename_, std::ios_base::app);
    if (!ofile) {
        throw std::string("File problem (probably invalid filename)");
    }

    ofile << "Request configuration #" << vm_depl.req_conf_num
          << std::endl;

    ofile << "Server configuration #" << vm_depl.serv_conf_num
          << std::endl;

    ofile << "\n=== DEPLOYMENT ===" << std::endl;

    std::vector<std::pair<unsigned, unsigned>> depl_vect(
            vm_depl.vm_mapping.begin(), vm_depl.vm_mapping.end());

    auto comparator = [&req_conf](const auto& left, const auto& right)
                      {
                          return req_conf.charact_vect[left.first].num <
                                 req_conf.charact_vect[right.first].num;
                      };

    std::sort(depl_vect.begin(),
              depl_vect.end(),
              comparator);

    for (const auto& it: depl_vect) {
        ofile << req_conf.charact_vect[it.first].num
              << " -> "
              << serv_conf.charact_vect[it.second].num << std::endl;
    }

    std::stringstream ss;
    ss << "Number of deployed VM: "
       << vm_depl.deployed_vm_num;

    std::stringstream dashes;
    for (unsigned i = 0; i < ss.str().length(); i++) {
        dashes << "-";
    }

    ofile << dashes.str() << std::endl
          << ss.str() << std::endl;

    if (vm_depl.was_all_vm_deployed) {
        ofile << "All VM deployed: True" << std::endl;
    
    } else {
        ofile << "All VM deployed: False" << std::endl;
    }

    ofile << dashes.str() << "\n\n\n" << std::endl;
}

bool RecourceDistributor::try_deploy_vm(VmDeployment& vm_depl,
                                        unsigned vm_num,
                                        unsigned serv_num,
                                        const ParsingResult<>& req_conf,
                                        const ParsingResult<>& serv_conf,
                                        CharactVect& serv_load)
{
    if (serv_load[serv_num].core_num +
        req_conf.charact_vect[vm_num].core_num <=
        serv_conf.charact_vect[serv_num].core_num
        and
        serv_load[serv_num].ram +
        req_conf.charact_vect[vm_num].ram <=
        serv_conf.charact_vect[serv_num].ram) {
        
        vm_depl.vm_mapping[vm_num] = serv_num;
        
        serv_load[serv_num].core_num +=
                req_conf.charact_vect[vm_num].core_num;
        
        serv_load[serv_num].ram += req_conf.charact_vect[vm_num].ram;

        return true;
    
    } else {
        return false;
    }
}

void
RecourceDistributor::limited_searching(VmDeployment& vm_depl,
                                       unsigned vm_num,
                                       const ParsingResult<>& req_conf,
                                       const ParsingResult<>& serv_conf,
                                       CharactVect& serv_load)
{
    std::cerr << "DEBUG: limited_searching() "
              << vm_num
              << std::endl;

    // The vector which will be filled with pairs of the form
    // <server_number : available_number_of_critical_resource>.
    using ServForSearch = std::pair<unsigned, unsigned>;
    std::vector<ServForSearch> serv_for_search;

    if (core_num_is_critical_) {
        std::cerr << "DEBUG: core_num_is_critical_" << std::endl;

    } else {
        std::cerr << "DEBUG: ram_is_critical_" << std::endl;
    }
    
    for (unsigned j = 0; j < serv_conf.charact_vect.size(); j++) {
        if (/*(serv_load[j].core_num > 0) and*/
                ((serv_conf.charact_vect[j].core_num >=
                req_conf.charact_vect[vm_num].core_num) and
                core_num_is_critical_)
                or
                ((serv_conf.charact_vect[j].ram >=
                req_conf.charact_vect[vm_num].ram) and
                not core_num_is_critical_)) {
            
            if (core_num_is_critical_) {
                serv_for_search.emplace_back(std::make_pair(
                        j,
                        serv_conf.charact_vect[j].core_num - 
                        serv_load[j].core_num));

            } else {
                serv_for_search.emplace_back(std::make_pair(
                        j,
                        serv_conf.charact_vect[j].ram - 
                        serv_load[j].ram));
            }
        }
    }

    /*auto serv_for_search_copy {serv_for_search};*/

    auto comparator = [](const ServForSearch& left, const ServForSearch& right)
                      {
                          return left.second > right.second;
                      };

    std::sort(serv_for_search.begin(),
              serv_for_search.end(),
              comparator);

    // DEBUG PRINT0
    unsigned limit = limit_ <= serv_for_search.size() ?
            limit_ : serv_for_search.size();
    
    std::cerr << "DEBUG: limit = " << limit << std::endl;
    for (unsigned i = 0; i < limit; i++) {
        std::cerr << "#"
                  << serv_conf.charact_vect[serv_for_search[i].first].num
                  << " : "
                  << serv_for_search[i].second
                  << std::endl;
    }
    // DEBUG PRINT1

    // Try to redeploy VM from servers for searching with greedy algorithm.
    ParsingResult<CharactVectWithIndices> req_subconf;
    ParsingResult<CharactVectWithIndices> serv_subconf;
    std::map<unsigned, unsigned> req_subconf_to_conf;
    std::map<unsigned, unsigned> serv_subconf_to_conf;
    
    req_subconf.charact_vect.emplace_back(
            std::make_pair(req_conf.charact_vect[vm_num],
                           vm_num));

    /*req_subconf_to_conf[req_subconf.charact_vect.size() - 1] = vm_num;*/
    std::cerr << "$$$ DEBUG: req_subconf.charact_vect.size() - 1 = "
              << req_subconf.charact_vect.size() - 1
              << std::endl;

    for (unsigned i = 0; i < limit; i++) {
        serv_subconf.charact_vect.emplace_back(
                std::make_pair(
                    serv_conf.charact_vect[serv_for_search[i].first],
                    serv_for_search[i].first));

        std::cerr << "DEBUG: emplace_back Serv #"
                  << serv_conf.charact_vect[serv_for_search[i].first].num
                  << std::endl;

        for (const auto& it: vm_depl.vm_mapping) {
            if (it.second == serv_for_search[i].first) {
                
                req_subconf.charact_vect.emplace_back(
                        std::make_pair(
                            req_conf.charact_vect[it.first],
                            it.first));

                /*req_subconf_to_conf[req_subconf.charact_vect.size() - 1] =
                        it.first;*/
                
                std::cerr << "$$$ DEBUG: req_subconf.charact_vect.size() - 1 = "
                          << req_subconf.charact_vect.size() - 1
                          << std::endl;

                std::cerr << "DEBUG:     emplace_back VM #"
                          << req_conf.charact_vect[it.first].num
                          << std::endl;
            }
        }
        std::cerr << std::endl;
    }

    if (core_num_is_critical_) {
        auto comparator_less = [](const auto& left, const auto& right)
                               {
                                   return left.first.core_num <
                                          right.first.core_num;
                               };

        auto comparator_gr = [](const auto& left, const auto& right)
                             {
                                 return left.first.core_num >
                                        right.first.core_num;
                             };

        std::sort(req_subconf.charact_vect.begin() + 1,
                  req_subconf.charact_vect.end(),
                  comparator_less);

        std::sort(serv_subconf.charact_vect.begin(),
                  serv_subconf.charact_vect.end(),
                  comparator_gr);
    
    } else {
        auto comparator_less = [](const auto& left, const auto& right)
                               {
                                   return left.first.ram < right.first.ram;
                               };

        auto comparator_gr = [](const auto& left, const auto& right)
                             {
                                 return left.first.ram > right.first.ram;
                             };

        std::sort(req_subconf.charact_vect.begin() + 1,
                  req_subconf.charact_vect.end(),
                  comparator_less);

        std::sort(serv_subconf.charact_vect.begin(),
                  serv_subconf.charact_vect.end(),
                  comparator_gr);
    }

    for (unsigned i = 0; i < req_subconf.charact_vect.size(); i++) {
        req_subconf_to_conf[i] =
                req_subconf.charact_vect[i].second;
    }

    for (unsigned i = 0; i < serv_subconf.charact_vect.size(); i++) {
        serv_subconf_to_conf[i] =
                serv_subconf.charact_vect[i].second;
        
        /*std::cerr << "### DEBUG: serv_subconf.charact_vect.size() - 1 = "
                  << serv_subconf.charact_vect.size() - 1
                  << std::endl;*/
    }

    // DEBUG PRINT0
    std::cerr << "<<< req_subconf_to_conf >>>" << std::endl;
    for (const auto& it: req_subconf_to_conf) {
        std::cerr << it.first << " -> " << it.second << std::endl;
    }

    std::cerr << "<<< serv_subconf_to_conf >>>" << std::endl;
    for (const auto& it: serv_subconf_to_conf) {
        std::cerr << it.first << " -> " << it.second << std::endl;
    }
    // DEBUG PRINT1

    // DEBUG PRINT0
    std::cerr << "REQ SUBCONF\n";
    for (const auto& req: req_subconf.charact_vect) {
        std::cerr << "#" << req.first.num << std::endl;
    }
    std::cerr << "SERV SUBCONF\n";
    for (const auto& serv: serv_subconf.charact_vect) {
        std::cerr << "#" << serv.first.num << std::endl;
    }
    // DEBUG PRINT1

    CharactVect serv_subload =
            std::vector(serv_subconf.charact_vect.size(), Item());

    ParsingResult req_subconf_copy;
    for (const auto& it: req_subconf.charact_vect) {
        req_subconf_copy.charact_vect.emplace_back(it.first);
    }

    ParsingResult serv_subconf_copy;
    for (const auto& it: serv_subconf.charact_vect) {
        serv_subconf_copy.charact_vect.emplace_back(it.first);
    }

    VmDeployment vm_subdepl = algorithm(req_subconf_copy,
                                        serv_subconf_copy,
                                        serv_subload,
                                        false);
    
    // DEBUG PRINT0
    /*print_depl_to_file(vm_subdepl, req_subconf_copy, serv_subconf_copy);*/
    // DEBUG PRINT1

    if (vm_subdepl.was_all_vm_deployed) {
        for (const auto& it: vm_subdepl.vm_mapping) {
            if (serv_subconf_to_conf[it.second] !=
                    vm_depl.vm_mapping[req_subconf_to_conf[it.first]]) {

                vm_depl.vm_mapping[req_subconf_to_conf[it.first]] =
                        serv_subconf_to_conf[it.second];
            }
        }

        vm_depl.deployed_vm_num += 1;
        if (vm_depl.deployed_vm_num == req_conf.charact_vect.size()) {
            vm_depl.was_all_vm_deployed = true;
        }

        for (unsigned i = 0; i < serv_subload.size(); i++) {
            serv_load[serv_subconf_to_conf[i]] = serv_subload[i];
        }
    }
}
