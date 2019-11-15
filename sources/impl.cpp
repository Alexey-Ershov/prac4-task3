#include "header.hpp"

#include "pugixml.hpp"


void RecourceDistributor::distributeRecources()
{
    auto req_conf = parse_xml_data(req_dir_ +
                                   std::string("/") +
                                   std::string("r00.xml"));
    
    auto serv_conf = parse_xml_data(serv_dir_ +
                                    std::string("/") +
                                    std::string("s00.xml"));

    std::cout << "=== REQUESTS ===" << std::endl;
    std::cout << "req_conf_num = " << req_conf.conf_num << std::endl;
    for (const auto& req: req_conf.charact_vect) {
        std::cout << req.first << " : "
                  << req.second
                  << std::endl;
    }
    std::cout << std::endl;

    std::cout << "=== SERVERS ===" << std::endl;
    std::cout << "serv_conf_num = " << serv_conf.conf_num << std::endl;
    for (const auto& serv: serv_conf.charact_vect) {
        std::cout << serv.first << " : "
                  << serv.second
                  << std::endl;
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
