#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <thread>
#include "../include/ConverterJSON.h"
#include "../include/SearchServer.h"

template <typename T>
std::string NumberToString ( T Number, int n )
{
    std::ostringstream ss;
    ss.precision(n);
    ss << std::fixed;
    ss << Number;
    return ss.str();
}

SearchServer::SearchServer(InvertedIndex & idx) : _index(idx){}

std::map<std::string, int> SearchServer::wordsSplit(std::string str) {
    std::map<std::string, int> words;
    int start = 0;
    if(str.back() != ' ')   // коряво конечно, но работает. Так как мы хардкодим именно пробел как разделитель
        str +=" ";          // то что бы последнее слово в строке тоже попало, добавим ' ' в конец если его там нет
    int end = str.find(" ");
    while(end != -1){
        std::string word = str.substr(start, end - start);
        if (words.find(word) == words.end()) {
            words[word] = 1;
        } else {
            words[word] = ++words[word];
        }
        start = end + 1;
        end = str.find(" ", start);
    }
    return words;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(ConverterJSON converterJson) {
    //получим из conf структуры запросы
    std::vector  queries_input = converterJson.GetRequests();
    // Вектор запросов {"milk water banana milk water water", "sugar"};
    // будет вот так: vector < {(banana,1)(milk 2)(water 3)}, {sugar, 1}>
    // т е индекс вектора это новый запрос в котором слово->сколько раз
    for (int i = 0; i < queries_input.size(); ++i) {
        _requests.push_back(wordsSplit(queries_input[i]));
    }
    calcResult();
    return _result;
}


void SearchServer::calcResult() {
    // Создадим вектор для накоплениея частоты слов в документах <DocID, Frequency> по запросам request
    // дальше в цикле он считается для каждого запроса std::map<int, int> mapDocCount
    // В vMapDocs его и напихаем.
    // Таким образом каждый элемент вектора это частотная характеристика запроса
    std::vector<std::map<int, int>> vMapDocs;

    std::vector<int> absMax; // вектор по запросам куда положим максимальную абсолютную релевантность

    for (int nRequest = 0; nRequest < _requests.size(); ++nRequest) {
        absMax.push_back(0); // by default

        // по заданию - надо отсортировать слова запроса в порядке увеличения частоты встречаемости: от самых редких до самых частых.
        // Внутри каждого запроса сортируем map-key по map-value
        std::set<std::pair<std::string, int>, comp> set(_requests[nRequest].begin(), _requests[nRequest].end());
        // теперь в set отсортированы по value

        // создадим map для рассчета частоты слов в документе <DocID, Frequency>
        std::map<int, int> mapDocCount;

        // перебираем отсортированные элементы
        for (auto const &pair: set) {
            // тут pair - слово из запроса и сколько этих слов в запросе

            // _index.GetWordCount(pair.first) - это vеctor из документов с <map(docid,count)>
            // перебираем по всем документам в которых если слово pair.first из запроса
            for (int i = 0; i < _index.GetWordCount(pair.first).size(); ++i) {
                // key -> номер документа, value -> накапливаем количество встреч
                mapDocCount[_index.GetWordCount(pair.first)[i].doc_id] += _index.GetWordCount(pair.first)[i].count;
                // 1 request, Next word ALL docs
            }
            // 1 request, ALL word ALL docs
        }
        // и накопим для данного request.
        vMapDocs.push_back(mapDocCount);
        //NEXT request ALL words ALL Docs,

        for (auto it = mapDocCount.begin(); it != mapDocCount.end(); ++it) {
            if (it->second > absMax[nRequest])
                absMax[nRequest] = it->second;
        }
    }
    //Итого ALL requests ALL words ALL Docs накопили в vMapDocs

    nlohmann::json j;
    for (int nRequest = 0; nRequest < vMapDocs.size(); ++nRequest) {

        // по заданию делаю 3-значную слева дополненную нулями строку типа "Request00X"
        std::string iString = std::to_string(nRequest);
        unsigned int number_of_zeros = 3 - iString.length();
        iString.insert(0, number_of_zeros, '0');

        if (absMax[nRequest] > 0){ // т е запрос актуален
            j["answers"]["request" + iString]["result"] = "true";

            // создадим вектор из RelativeIndex что бы хранить результат <doc_id, rel> для конкретного слова из запроса
            std::vector<RelativeIndex> v_RelativeIndex;

            // бежим по mapDocs запроса nRequest в которой считаем relevances (abs & rel)
            for (auto it1 = vMapDocs[nRequest].begin(); it1 != vMapDocs[nRequest].end(); ++it1) {

                RelativeIndex relativeIndex;
                float flRel = (it1->second / static_cast<float> (absMax[nRequest]));
                std::string strDocId    = NumberToString(it1->first, 0);
                std::string strRel      = NumberToString(flRel, 9);
                std::map<std::string, std::string> c_map{
                        {"docid", strDocId},
                        {"rank",  strRel}
                };

                relativeIndex.doc_id = it1->first;
                relativeIndex.rank = flRel;

                j["answers"]["request" + iString]["relevance"] += c_map;
                // и запушим результат по слову из запроса
                v_RelativeIndex.push_back(relativeIndex);
            }
            // тут по всему запросу положим результат
            _result.push_back(v_RelativeIndex);
        } else{
            j["answers"]["request" + iString]["result"] = "false";
            // тут больше ничего не делаем раз false
            _result.push_back({}); // ну запрос есть, но без результатов
        }
    }
    std::ofstream os;
    os.open("answers.json");
    os << j.dump(3) << std::endl;
    std::cout << j.dump(3);
    os.close();
}



