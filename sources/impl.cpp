#include "header.hpp"

#include "pugixml.hpp"


void RecourceDistributor::distributeRecources()
{
    auto req_vect = parse_xml_data(req_dir_ +
                                   std::string("/") +
                                   std::string("r00.xml"));
    
    auto serv_vect = parse_xml_data(serv_dir_ +
                                    std::string("/") +
                                    std::string("s00.xml"));

    std::cout << "=== REQUESTS ===" << std::endl;
    for (const auto& req: req_vect) {
        std::cout << req.first << " : "
                  << req.second
                  << std::endl;
    }
    std::cout << std::endl;

    std::cout << "=== SERVERS ===" << std::endl;
    for (const auto& serv: serv_vect) {
        std::cout << serv.first << " : "
                  << serv.second
                  << std::endl;
    }
}

CharactVect
RecourceDistributor::parse_xml_data(const std::string& input_file)
{
    pugi::xml_document doc;
    if (!doc.load_file(input_file.c_str())) {
        throw std::string("Can't load XML file");
    }

    CharactVect output_vect;
    
    for (const auto& node: doc.child("server_conf")) {
        output_vect.emplace_back(std::make_pair(
                node.attribute("core_num").as_int(),
                node.attribute("ram").as_int()));
    }

    return output_vect;
}
