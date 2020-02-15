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
#include <array>
#include <stdexcept>
#include <unordered_map>
using namespace std;


template<typename T, size_t N>
class SpeedCont {
public:
	SpeedCont() 
	{
		sizee = 0;
	}

  const auto begin() const
	{
		return data.begin();
	}
  const auto end() const
	{
		return data.begin() + sizee;
	}

	void push_back(const T& t)
	{
	if (sizee >= N) {
      throw overflow_error("Full");
    } else {
      data[sizee++] = t;
    }	
	}
	const T& operator [] (const size_t& pos) const {
		return data[pos];
	}
	const size_t& size()const {return sizee;}

	T& back() {return data[sizee-1];}

private:
	array<T, N> data;
	size_t sizee;

};

struct Elem {
	string_view s;
	int64_t num_doc;
};

class InvertedIndex {
public:
	
  void Add(string_view document);
	const vector<size_t>& Lookup(const string_view& word) const;
  const string GetDocument(size_t id) const {
    string ret;
		for (const auto w : docs[id]) {
			ret += w;
		}
		return ret;
  }

	const auto begin() const {return index.begin();}
	const auto end() const {return index.end();}
private:
  unordered_map<string_view, vector<size_t>> index;
  deque<string> words;
	deque<deque<string_view>> docs;
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

