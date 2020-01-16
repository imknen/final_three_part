#include "search_server.h"
#include "iterator_range.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>


vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    new_index.Add(move(current_document));
  }

  index = move(new_index);
}

auto SplitIntoWordsDura (string& s, TotalDuration& t) {
	ADD_DURATION(t);
	return SplitIntoWords(s);
}
void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
	TotalDuration read("Total read");
	TotalDuration split("Total split");
	TotalDuration lookup("Total lookup");
	TotalDuration speed_sort("Total work sort");
	TotalDuration form_res("Forming result");
	TotalDuration make_vec_pair("Total add vect_pair");
  for (string current_query; getline(query_input, current_query); ) {
    const auto words = SplitIntoWordsDura(current_query,split);

    vector<size_t> docid_count(10'000, 0);
		{ADD_DURATION(lookup);
    for (const auto& word : words) {
      for (const size_t docid : index.Lookup(word)) {
        docid_count[docid]++;
      }
    }
		}

    vector<pair<size_t, size_t>> search_results;
		search_results.reserve(10'005);
		{
		ADD_DURATION(make_vec_pair);
		for (size_t i = 0;i < 10'000; i++) {
			if (docid_count[i] > 0) {
				search_results.push_back({i, docid_count[i]});
			}
		}
		}

		{
		ADD_DURATION(speed_sort);
						partial_sort(
							search_results.begin(),
							search_results.begin() +5,
							search_results.end(),
							[](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
								int64_t lhs_docid = lhs.first;
								auto lhs_hit_count = lhs.second;
								int64_t rhs_docid = rhs.first;
								auto rhs_hit_count = rhs.second;
								return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
							}
						);
		}
		{
		ADD_DURATION(form_res);
    search_results_output << current_query << ':';
    for (auto [docid, hitcount] : Head(search_results, 5)) {
      search_results_output << " {"
        << "docid: " << docid << ", "
        << "hitcount: " << hitcount << '}';
    }
    search_results_output << endl;
		}
  }
}

void InvertedIndex::Add(const string& document) {
  docs.push_back(document);

  const size_t docid = docs.size() - 1;
  for (const auto& word : SplitIntoWords(document)) {
    index[word].push_back(docid);
  }
}

list<size_t> InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return {};
  }
}
