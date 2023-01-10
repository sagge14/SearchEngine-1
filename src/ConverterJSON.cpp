#include <fstream>
#include <iostream>
#include "../include/ConverterJSON.h"


class FileConfigDoesNotExist: public std::exception{
public:
    const char* what() const noexcept override{ // переопределяем what
        return "Сonfig file is missing";
    }
};


class configFieldIsMissing:public  std::exception{
public:
    const char* what() const noexcept override{ // переопределяем what
        return "Field \"Config\" is missing.";
    }
};

ConverterJSON::ConverterJSON() {
    // считываем requests.json и config.json в структуру conf
    try {
        readConfigFile("../config/config.json");
    }
    catch (FileConfigDoesNotExist &x){
        std::cerr << "Wrong configuration: " << x.what() << std::endl;
    }

    std::ifstream readFrom1("../config/requests.json");
    std::vector<std::string> req;
    nlohmann::json j1;
    readFrom1 >> j1;
    for (auto const& val : j1["requests"]){
        conf.requests.push_back(val); // тут сами запросы
    }
}

int ConverterJSON::GetResponsesLimit() {
    return conf.max_responses;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    return conf.requests;
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    return conf.files;
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
}

void ConverterJSON::readConfigFile(std::string filePath) {
    std::ifstream readFrom(filePath.c_str());
    if(!readFrom.is_open()) {
        throw FileConfigDoesNotExist();
    }

    nlohmann::json j;
    readFrom >> j;
    if (!j.contains("config")) {
        throw configFieldIsMissing();
    }

    conf.name = j["config"]["name"];
    std::cout << "Search Engine " << conf.name << " is running" << std::endl;
    conf.version = j["config"]["version"];
    std::cout << "Search Engine version is " << conf.version << std::endl;
    conf.max_responses = j["config"]["max_responses"];

    if (j.find("files") != j.end()) {
        for (auto const &val: j["files"]) {
            conf.files.push_back(val); // тут имена файлов в которых искать
            int i = 0;
        }
    }
    readFrom.close();
    for(int i=0; i < conf.files.size(); ++i){
        std::ifstream readFrom(conf.files[i]);
        if(!readFrom.is_open()) {
            std::cout << "Data file " << conf.files[i] << "is missing" << std::endl;
        }
    }
}
