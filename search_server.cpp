#include <stdexcept>

#include "search_server.h"

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words)
    : stop_words_(MakeUniqueNonEmptyStrings(stop_words))
{
    for (const std::string& word : stop_words_) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Stop words contain forbidden characters");
        }
    }
}
SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text))
{
    if (!IsValidWord(stop_words_text)) {
        throw std::invalid_argument("Stop words contain forbidden characters");
    }
}

void SearchServer::AddDocument(int document_id, const std::string& document, 
    DocumentStatus status, const std::vector<int>& ratings) {
    if (document_id < 0) {
        throw std::invalid_argument("Document ID negative");
    }
    if (documents_.count(document_id) > 0) {
        throw std::invalid_argument("Document ID is busy");
    }
    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    for (const std::string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
        id_to_word_freq_[document_id][word] = 0;
    }
    documents_.emplace(document_id,
        DocumentData{
            ComputeAverageRating(ratings),
            status
        });
    document_ids_.push_back(document_id);
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string& word) {
    return std::none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    if (text.empty()) {
        throw std::invalid_argument("Document is empty");
    }
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("There are forbidden characters in the document");
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

const std::map<string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static const map<string, double>& a = {};
    if (!id_to_word_freq_.at(document_id).empty()) return id_to_word_freq_.at(document_id);
    else return a;
}

void SearchServer::RemoveDocument(int document_id) {
    auto it = lower_bound(document_ids_.begin(), document_ids_.end(), document_id);
    document_ids_.erase(it);
    auto itm = find_if(documents_.begin(), documents_.end(), [document_id](auto doc_tmp) {///
        return doc_tmp.first == document_id;
        });
    documents_.erase(itm);
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query,
    int document_id) const {
    const Query query = ParseQuery(raw_query);
    std::vector<std::string> matched_words;
    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    if (matched_words.empty()) {
        throw std::invalid_argument("No matching documents");
    }
    return std::make_tuple(matched_words, documents_.at(document_id).status);
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    if (text.empty()) {
        throw std::invalid_argument("Query is empty");
    }
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    if (text.empty() || text[0] == '-' || !IsValidWord(text)) {
        throw std::invalid_argument("Query error");
    }
    return {
        text,
        is_minus,
        IsStopWord(text)
    };
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    if (text.empty()) {
        throw std::invalid_argument("Query is empty");
    }
    Query query;
    for (const std::string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            }
            else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

std::vector<Document> SearchServer::FindAllDocuments(const Query& query) const {
    std::map<int, double> document_to_relevance;
    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            document_to_relevance[document_id] += term_freq * inverse_document_freq;
        }
    }

    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({
            document_id,
            relevance,
            documents_.at(document_id).rating
            });
    }
    return matched_documents;
}