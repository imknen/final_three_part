#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <deque>
#include <string_view>
using namespace std;

class InvertedIndex {
public:

//	InvertedIndex() {docs.reserve(50'000);}

  void Add(string document);
  const vector<size_t>& Lookup(const string_view& word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  map<string_view, vector<size_t>> index;
  vector<string> docs;
	vector<size_t> tempor;
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

