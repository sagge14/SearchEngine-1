#pragma once
#include <iostream>
#include <map>
#include <set>
#include "nlohmann/json.hpp"
#include "InvertedIndex.h"
#include "ConverterJSON.h"


struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex &other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

struct comp
{
    template<typename T>
    bool operator()(const T &l, const T &r) const
    {
        if (l.second != r.second) {
            return l.second < r.second;
        }
        return l.first < r.first;
    }
};


class SearchServer {
public:
/**
* @param idx в конструктор класса передаётся ссылка на класс InvertedIndex,
чтобы SearchServer мог узнать частоту слов встречаемых в запросе
*/
    SearchServer(InvertedIndex&);
/**
* Метод обработки поисковых запросов
* @param queries_input поисковые запросы взятые из файла requests.json
* @return возвращает отсортированный список релевантных ответов для
заданных запросов
*/
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);


private:
    InvertedIndex _index;
    std::vector<std::string> _requests;
};