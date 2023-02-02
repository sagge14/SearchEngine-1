#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

int main() {
    ConverterJSON converterJson("config.json","requests.json");
    InvertedIndex invertedIndex;
    invertedIndex.UpdateDocumentBase(converterJson);
    SearchServer searchServer(invertedIndex);
    searchServer.search(converterJson);
    return 0;
}


