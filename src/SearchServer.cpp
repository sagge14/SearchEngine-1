#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <thread>

#include "../include/SearchServer.h"
#include <jsoncpp/json/json.h>

class MyParser {
public:
    MyParser() = default;
    ~MyParser() = default;
    inline static
    Json::Value parse(const char* inputFile) {
        Json::Value val;
        std::ifstream ifs(inputFile);
        Json::Reader reader;
        reader.parse(ifs, val);
        return val;
    }
};


SearchServer::SearchServer(InvertedIndex & idx) : _index(idx)
{
    // создадим map для рассчета частоты слов в документе <DocID, Frequency>
    std::map<int, int> mapDocCount;

    // получим в requestsMap разобранные запросы (requests) <слово, и сколько раз оно встречается>
    std::map<std::string, int> requestsMap = idx.get_requestsMap();

    // не все запросы могут быть найдены в документах, те, которые не найдены надо будет делать false в answers.json
    std::map <std::string, bool> isUsedRequest;

    // по заданию надо сначала обработать самые "редкие" запросы т е надо отсортировать key по value
    std::set<std::pair<std::string, int>, comp> set(requestsMap.begin(), requestsMap.end());

    // теперь в set отсортированы по value
    for (auto const &pair: set) {
        // бежим по запросам (начиная с самых редких)

        isUsedRequest[pair.first] = false; // для начала все в false
        // Если idx.GetWordCount(pair.first).size() == 0 то на этот запрос нет ни одного результата в документах и его
        // надо будет делать false

        for(int i = 0; i < idx.GetWordCount(pair.first).size(); ++i) {
//            std::cout << "doc_id " << idx.GetWordCount(pair.first)[i].doc_id << " ";
//            std::cout << "frequency " << idx.GetWordCount(pair.first)[i].count << std::endl;

            // если мы тут то хоть где то в документах нашли совпадение с запросом, т е надо будет делать true в answers.json
            isUsedRequest[pair.first] = true;

            // нвпример слово russia заполнит так:
            // mapDocCount[0] - 7, 1 - т е слово встречается в 8 документе 1 раз
            // mapDocCount[0] - 14, 1 - т е слово встречается в 15 документе 1 раз
            mapDocCount[idx.GetWordCount(pair.first)[i].doc_id] +=  idx.GetWordCount(pair.first)[i].count;
        }
        int a=0;
    }

    // Итого в mapDocCount теперь  <номер документа, сколько всего слов из запросов входит>

    // создадим answers.json
    std::ofstream f;
    f.open("../answers.json",std::ios_base::trunc |std::ios_base::out);
    nlohmann::json j;
    int requestCounter=0;
    // внешний цикл - по запросам
    for (auto it = requestsMap.begin(); it != requestsMap.end(); ++it) {
        requestCounter++;
        // по заданию делаю 3 значную слева дополненную нулями строку типа Request00i
        std::string iString = std::to_string(requestCounter);
        unsigned int number_of_zeros = 3 - iString.length();
        iString.insert(0, number_of_zeros, '0');

        if(isUsedRequest[it->first]) {
            j["answers"]["request" + iString]["result"] = "true";

            // бежим по mapDocCount в которой считаем relevances (abs & rel)
            for (auto it1 = mapDocCount.begin(); it1 != mapDocCount.end(); ++it1) {
                //std::cout << std::endl << "DOC_ID " << it1->first << " ";
                //std::cout << "Abs relevance: " << it1->second << std::endl;
                float rel=0;
                if(idx.get_DocumentCountWords()[it1->first] >0) // от греха / на 0
                    rel = it1->second / static_cast<float> (idx.get_DocumentCountWords()[it1->first]);
                //std::cout << "Rel relevance: " <<  rel << std::endl;

                std::map<std::string , float> c_map { {"docid", it1->first}, {"rank", rel} };
                j["answers"]["request" + iString]["relevance"] += c_map;
            }


        } else{
            j["answers"]["request" + iString]["result"] = "false";
            // тут больше ничего не делаем раз false
        }
    }
    f << j << std::endl;
    f.close();

    Json::Value val = MyParser::parse("answers.json");
    std::cout << val["answers"] << std::endl;


}