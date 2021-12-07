#pragma once
#include "search_server.h"

void RemoveDuplicates(SearchServer& search_server) {
    search_server.GetDocumentCount();
    search_server.RemoveDocument();
    search_server.GetWordFrequencies();
}