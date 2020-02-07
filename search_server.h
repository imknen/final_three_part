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
#include <utility>
using namespace std;

class InvertedIndex {
public:

  void Add(string document);
//  const vector<size_t>& Lookup(string word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

	const auto begin() const {return index.begin();}
	const auto end() const {return index.end();}
private:
  vector<pair<string_view, size_t>> index;
  vector<string> docs;

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

