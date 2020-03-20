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
#include <mutex>
#include <functional>
#include <future>
#include "duration.h"
#include "iterator_range.h"

using namespace std;


class InvertedIndex {
public:

	InvertedIndex(InvertedIndex&& inv_idx)
	: index(move(inv_idx.index)),
		docs(move(inv_idx.docs)), 
		empty_vec(move(inv_idx.empty_vec)),
		m_index()
		{}

	InvertedIndex() : index(10'000,map<size_t, vector<size_t>>()){
	}

	void operator=(InvertedIndex&& inv_idx) {
		index = move(inv_idx.index);
		docs = move(inv_idx.docs);
		empty_vec = move(inv_idx.empty_vec);
	}
	void AddDocuments(istream& input);
	void FillIndex(const IteratorRange<deque<string>::const_iterator> range, size_t numberdoc);

  void Add(string_view document, size_t);
	const vector<size_t>& Lookup(size_t) const;
  const string& GetDocument(size_t id) const {
    return docs[id];
  }
	const deque<string>& GetDocuments() const {
		return docs;
	}

	const auto begin() const {return index.begin();}
	const auto end() const {return index.end();}

private:
  vector<map<size_t, vector<size_t>>> index;
	//deque<pair<string_view, size_t>> index;
  deque<string> docs;
	vector <size_t> empty_vec;
	mutex m_index;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);
	string AddQueriesStreamSingleThread(
			 vector<string>::iterator begin,
 			 vector<string>::iterator end);

private:
  InvertedIndex index;
	bool baseempty;
	mutex m;
	vector<future<string>> futures;
};


