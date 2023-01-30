#pragma once
#include <exception>
#include "vector"
#include "nlohmann/json.hpp"

struct Conf{
    std::string name;
    std::string version;
    int max_responses;
    std::vector<std::string> files;
    std::vector<std::string> requests;
};


class ConverterJSON {
public:
    ConverterJSON(std::string, std::string);

// @return Возвращает список с содержимым файлов перечисленных в config.json
    std::vector<std::string> GetTextDocuments();

// Метод считывает поле max_responses для определения предельного
// количества ответов на один запрос  @return
    int GetResponsesLimit();

// @return возвращает список запросов из файла requests.json
    std::vector<std::string> GetRequests();

protected:
    void readConfigFile(std::string);

private:
    Conf conf;
    std::vector<std::vector<std::pair<int, float>>> _answers;
};

