#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

#include "corpus_index.hpp"

cls_06::DocumentEntry::DocumentEntry(Document *document, size_t document_index, size_t occurrences)
    : document(document), document_index(document_index), occurrences(occurrences) {}

bool cls_06::DocumentEntry::operator<(const DocumentEntry &other) const {
    if (occurrences == other.occurrences) {
        return document->uri < other.document->uri;
    }

    return occurrences > other.occurrences;
}

cls_06::CorpusIndex::CorpusIndex(cls_06::Corpus corpus) : corpus_(std::move(corpus)) {
    for (size_t i = 0; i < corpus_.size(); ++i) {
        const std::string &document = corpus_[i].text;
        std::unordered_map<std::string, size_t> word_occurrences;

        std::string cleaned_word;
        for (char c : document) {
            if (std::isalnum(c)) {
                cleaned_word.push_back(std::tolower(c));
            } else if (!cleaned_word.empty()) {
                ++word_occurrences[cleaned_word];
                cleaned_word.clear();
            }
        }

        if (!cleaned_word.empty()) {
            ++word_occurrences[cleaned_word];
        }

        for (const auto &[word, occurrences] : word_occurrences) {
            index_[word].insert(DocumentEntry(&corpus_[i], i, occurrences));
        }
    }
}

std::vector<const cls_06::Document *> cls_06::CorpusIndex::find(std::string_view word) const {
    auto it = index_.find(std::string(word));
    if (it == index_.end()) {
        return {};
    }

    std::vector<const Document *> result;
    for (const DocumentEntry &entry : it->second) {
        result.push_back(entry.document);
    }

    return result;
}
