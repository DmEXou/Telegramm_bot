#pragma once
#include "search_server.h"

void RemoveDuplicates(SearchServer& search_server) {
//    search_server.GetDocumentCount();
    for (int i = 0; i < search_server.GetDocumentCount(); ++i) {
        for (int j = i+1; j < search_server.GetDocumentCount(); ++j) {
            std::cout << "!";
            if ((search_server.GetWordFrequencies(i) == search_server.GetWordFrequencies(j))&&(j < search_server.GetDocumentCount())) {
                //search_server.RemoveDocument(j);
            }
        }
    }
//    search_server.GetWordFrequencies();
//    search_server.RemoveDocument();
//    search_server.GetWordFrequencies();
}