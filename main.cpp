#include "read_input_functions.h"
#include "remove_duplicates.h"

int main() {
    std::string stop_word = "and with";
    SearchServer search_server(stop_word);

    AddDocument(search_server, 1, "funny pet and nasty rat", DocumentStatus::ACTUAL, { 7, 2, 7 });
    AddDocument(search_server, 2, "funny pet with curly hair", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 3, "funny pet with curly hair", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 4, "funny pet and curly hair", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 5, "funny funny pet and nasty nasty rat", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 6, "funny pet and not very nasty rat", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 7, "very nasty rat and not very funny pet", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 8, "pet with rat and rat and rat", DocumentStatus::ACTUAL, { 1, 2 });
    AddDocument(search_server, 9, "nasty rat with curly hair", DocumentStatus::ACTUAL, { 1, 2 });
    {
        LOG_DURATION("Remove Duplicates");
        std::cout << "Before duplicates removed: " << search_server.GetDocumentCount() << std::endl;
        RemoveDuplicates(search_server);
        std::cout << "After duplicates removed: " << search_server.GetDocumentCount() << std::endl;
    }
    return 0;
}



