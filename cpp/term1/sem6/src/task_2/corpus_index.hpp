#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace cls_06 {

struct Document {
    std::string uri;
    std::string text;
};

/*
 * Entry in the index, which contains the index of the document in the corpus and
 * the number of occurrences of the word in the document.
 */
struct DocumentEntry {
    Document *document;
    size_t document_index;
    size_t occurrences;

    DocumentEntry(Document *document, size_t document_index, size_t occurrences);
    ~DocumentEntry() = default;

    bool operator<(const DocumentEntry &other) const;
};

using Corpus = std::vector<Document>;

class CorpusIndex {
  private:
    Corpus corpus_;
    std::unordered_map<std::string, std::set<DocumentEntry>> index_;

  public:
    CorpusIndex() = delete;
    CorpusIndex(const CorpusIndex &) = delete;
    CorpusIndex &operator=(const CorpusIndex &) = delete;

    CorpusIndex(Corpus corpus);

    /*
     * Searching for a word in the corpus, returns a vector of pointers to documents, which sorted
     * by the number of occurrences of the word in the document.
     * If multiple documents have the same number of occurrences, they are sorted by the uri.
     *
     * @param word — word to search
     * @return vector of pointers to documents, which contain the word
     */
    std::vector<const Document *> find(std::string_view word) const;
};

}
