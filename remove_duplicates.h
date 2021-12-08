#pragma once
#include "search_server.h"

void RemoveDuplicates(SearchServer& search_server) {
    size_t Count = search_server.GetDocumentCount();
    vector<map<string, double>> tmp;
    for (int i = 1; i < Count; ++i) {
        tmp.push_back(search_server.GetWordFrequencies(i));
        if (count(tmp.begin(), tmp.end(), search_server.GetWordFrequencies(i+1)) > 0) {
            cout << "Found duplicate document id " << i+1 << endl;
            search_server.RemoveDocument(i+1);
        }
    }

    //auto it = upper_bound();



    //for (int i = 1; i < search_server.GetDocumentCount(); ++i) {
    //    for (int j = i+1; j < search_server.GetDocumentCount(); ++j) {
    //        if ((search_server.GetWordFrequencies(i) == search_server.GetWordFrequencies(j))&&(j < search_server.GetDocumentCount())) {
    //            std::cout << "Found duplicate document id " << i << " - " << j << endl;
    //            search_server.RemoveDocument(j);
    //        }
    //    }
    //}
    //O(WNlogN)
//    search_server.GetWordFrequencies();
//    search_server.RemoveDocument();
}