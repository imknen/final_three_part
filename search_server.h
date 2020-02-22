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
#include <array>
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

class InvertedIndex {
public:

  void Add(const string& document);

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

	const auto begin() const {return index.begin();}
	const auto end() const {return index.end();}
private:
  deque<pair<string_view, size_t>> index;
  SpeedCont<string, 50'000> docs;
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


