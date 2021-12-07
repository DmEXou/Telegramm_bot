#pragma once
#include <iostream>

#include "paginator.h"
#include "request_queue.h"

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    std::cout << "{ "
        << "document_id = " << document_id << ", "
        << "status = " << static_cast<int>(status) << ", "
        << "words =";
    for (const std::string& word : words) {
        std::cout << ' ' << word;
    }
    std::cout << "}" << std::endl;
}
 
void MatchDocuments(const SearchServer& search_server, const std::string& query) {
    try {
        std::cout << "Matching documents on request: " << query << std::endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            const int document_id = (*search_server.begin() + index);
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error matching documents on request: " << query << ": " << e.what() << std::endl;
    }
}

template <typename Object>
void AddDocument(Object& search_server, int document_id, const std::string& document, DocumentStatus status,
    const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    }
    catch (const std::exception& e) {
        std::cout << "Add document error " << document_id << ": " << e.what() << std::endl;
    }
}

std::vector<Document> FindTopDocuments(const SearchServer& search_server, const std::string& raw_query, RequestQueue& request_queue ){
    try {
        const std::vector<Document> result = request_queue.AddFindRequest(raw_query);
        return result;
    }
    catch (const std::exception& e) {
        std::cout << "Search error: " << e.what() << std::endl;
    }
}

template <typename Iterator>
std::ostream& operator<<(std::ostream& out, IteratorRange<Iterator> it_range) {
    for (auto it = it_range.begin(); it != it_range.end(); ++it) {
        out << *it;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Document& doc) {
    out << "{ ";
    out << "document_id = " << doc.id << ", relevance = " << doc.relevance << ", rating = " << doc.rating;
    out << " }";
    return out;
}

void PrintPage(std::vector<Document>search_results, int page_size) {
    try {
        if (page_size <= 0) {
            throw std::invalid_argument("page size inadmissible");
        }
        if (search_results.empty()) {
            throw std::invalid_argument("mathet documents empty");
        }
        const auto pages = Paginate(search_results, page_size);
        for (auto page = pages.begin(); page != pages.end(); ++page) {
            std::cout << *page << std::endl;
            std::cout << "Page break" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

