#include "search_server.h"
#include "iterator_range.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <unordered_map>

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  //InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    index.Add(move(current_document));
  }
//  index = move(new_index);
}

	const unordered_map<string_view, size_t> SplitToMap(string_view s)
	{
		unordered_map<string_view, size_t> ret;

	size_t pos = s.find_first_not_of(' ');
		if (pos != s.npos) {
			s.remove_prefix(pos);
		}	else { return {}; }
	while (!s.empty()) {
		pos = s.find(' ');
    ret[s.substr(0, pos)]++;
		s.remove_prefix(pos != s.npos ? pos +1: s.size());
		while (!s.empty() && isspace(s.front())) {
			s.remove_prefix(1);
		}
	}
		return ret;
	}
void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {

	TotalDuration read("Total read");
	TotalDuration split("Total split");
	TotalDuration lookup("Total lookup");
	TotalDuration speed_sort("Topositiontal work sort");
	TotalDuration form_res("Forming result");
	TotalDuration fill_vec_pair("Total fill vect_pair");

vector<pair<size_t, size_t>> search_results;
		search_results.reserve(55'005);

  for (string current_query; getline(query_input, current_query); ) {
    const auto words_query(SplitToMap(current_query));

    vector<size_t> docid_count(50'000, 0);
		{ADD_DURATION(lookup);
    for (const auto& word : words_query) {
			for (const size_t& docid : index.Lookup(word.first)) {
        docid_count[docid]+=word.second;
      }
    }

		}
		

    		{
		ADD_DURATION(fill_vec_pair);
		for (size_t i = 0;i < 50'000; i++) {
			if (docid_count[i] > 0) {
				search_results.push_back({i, docid_count[i]});
			}
		}
		}

		{
		ADD_DURATION(speed_sort);
						partial_sort(
							search_results.begin(),
							search_results.begin() + min<size_t>(search_results.size(), 5),
							search_results.end(),
							[](const pair<size_t, size_t>& lhs,const pair<size_t, size_t>& rhs) {
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
	
		search_results.clear();
  }
}

void InvertedIndex::Add(string_view document) {
	deque<string_view> list_words;
	const size_t docid = docs.size();

size_t pos = document.find_first_not_of(' ');
		if (pos != document.npos) {
			document.remove_prefix(pos);
		} else {return;}

	while (!document.empty()) {
		pos = document.find(' ');
		if (!index.count(document.substr(0, pos))) {
			words.push_back(static_cast<string>(document.substr(0, pos)));
			index[words.back()].push_back(docid);
		} else {
			index[document.substr(0, pos)].push_back(docid);
		}
		list_words.push_back(words.back());
		document.remove_prefix(pos != document.npos ? pos +1: document.size());
		while (!document.empty() && isspace(document.front())) {
			document.remove_prefix(1);
		}
	}
	docs.push_back(move(list_words));
}
const vector<size_t>& InvertedIndex::Lookup(const std::string_view& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return tempor; 
  }
	
}

