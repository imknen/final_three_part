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
  const vector<size_t>& Lookup(string word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  vector<pair<string_view, size_t>> index;
  vector<string> docs;
//	vector<size_t> tempor;

	struct Comp {
		bool operator() (pair<string_view, size_t>& rhs, pair<string_view, size_t>& lhs) {
			return rhs.first < lhs.first;
		}
	}comparator;
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

