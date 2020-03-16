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
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include "duration.h"
using namespace std;


class InvertedIndex {
public:

	InvertedIndex() : index(10'000,map<size_t, vector<size_t>>()){
	docs.reserve(50'000);
		
	}

  void Add(string document);
	const vector<size_t>& Lookup(size_t) const;
  const string& GetDocument(size_t id) const {
    return docs[id];
  }

	const auto begin() const {return index.begin();}
	const auto end() const {return index.end();}

private:
  vector<map<size_t, vector<size_t>>> index;
	//deque<pair<string_view, size_t>> index;
  vector<string> docs;
	vector <size_t> empty_vec;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex index;
	bool baseempty;
};


