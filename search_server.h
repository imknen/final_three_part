#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <deque>
using namespace std;

class IdDocCountWord {
public:
	IdDocCountWord() : quantity_word(50'000, 0) {}

	auto& operator [] (const size_t& position) {
		return quantity_word[position];
	}
private:
	vector<size_t> quantity_word;
};

class InvertedIndex {
public:
	
	InvertedIndex() {docs.reserve(50'000);}

  void Add(const string&& document);
  const IdDocCountWord& Lookup(const string& word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  map<string, IdDocCountWord> index;
  vector<string> docs;
	 IdDocCountWord tempor;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex index;
};

