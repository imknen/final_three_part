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
	IdDocCountWord(const size_t& size) : quantity_word(size, 0) {}

	size_t& operator [] (const size_t& position) {
		return quantity_word[position];
	}
	const size_t& operator [] (const size_t& position) const {
		return quantity_word[position];
	}

	bool Isempty() const {return quantity_word.empty();}

private:
	vector<size_t> quantity_word;
};

class InvertedIndex {
public:
	
	InvertedIndex() : tempor(0) {docs.reserve(50'000);}

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

