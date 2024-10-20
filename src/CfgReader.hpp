#include <iostream>
#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>

class Settings {
public:
    std::string cameratype;
    std::string outputname;
    std::string outputtype;
};

class CfgReader {
public:
    Settings settings;

    void getInfo() {

        std::string source_dir = __FILE__;
        source_dir = source_dir.substr(0, source_dir.find_last_of("/\\"));

        std::string fullpath_json = std::filesystem::path(source_dir).string() + "/cfgfile.json";
        std::ifstream file(fullpath_json);

        if (!file.is_open()) {
            std::cerr << "Error opening config file" << std::endl;
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();

        Json::Value root;
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();
        std::string errors;

        if (!reader->parse(content.c_str(), content.c_str() + content.size(), &root, &errors)) {
            std::cerr << "Error parsing JSON: " << errors << std::endl;
            delete reader;
            return;
        }

        delete reader;

        Settings settings;
        if (root.isMember("cameratype") && root["cameratype"].isString()) {
            settings.cameratype = root["cameratype"].asString();
        }
        if (root.isMember("outputname") && root["outputname"].isString()) {
            settings.outputname = root["outputname"].asString();
        }
        if (root.isMember("outputtype") && root["outputtype"].isString()) {
            settings.outputtype = root["outputtype"].asString();
        }

        // Use the settings object as needed
        std::cout << "Camera Type: " << settings.cameratype << std::endl;
        std::cout << "Output Name: " << settings.outputname << std::endl;
        std::cout << "Output Type: " << settings.outputtype << std::endl;

        this->settings = settings;
    }
};