#include "search_server.h"
#include "iterator_range.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>
SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
	if (!document_input){baseempty = true; return;} 
 InvertedIndex new_index;
//LOG_DURATION("add Documents")
//{
//TotalDuration function_add("function_add_time ");
  for (string current_document; getline(document_input, current_document); ) {
		//ADD_DURATION(function_add);
   new_index.Add(move(current_document));
  }
	baseempty = false;
  index = move(new_index);
//}
}

	const unordered_map<size_t,size_t> SplitToMap(string_view s)
	{
		unordered_map<size_t,size_t> ret;

	size_t pos = s.find_first_not_of(' ');
		if (pos != s.npos) {
			s.remove_prefix(pos);
		}	else { return {}; }
	while (!s.empty()) {
		pos = s.find(' ');
    ret[hash<string_view>{}(s.substr(0, pos))]++;
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
		if (baseempty) return;
//	TotalDuration read("Total read");
//	TotalDuration split("Total split");
//	TotalDuration lookup("Total lookup");
//	TotalDuration speed_sort("Topositiontal work sort");
//	TotalDuration form_res("Forming result");
//	TotalDuration fill_vec_pair("Total fill vect_pair");

	unordered_map<string, size_t> queries;

  for (string current_query; getline(query_input, current_query); ) {
    vector<size_t> docid_count(50'000, 0);
    const auto words(SplitToMap(current_query));

		//{ADD_DURATION(lookup);

		for (const auto& word : words) {
      for (const size_t docid : index.Lookup(word.first)) {
        docid_count[docid]+=word.second;
      }
    }
		//}
    vector<pair<size_t, size_t>> search_results;
		search_results.reserve(50'000);
		//{
		//ADD_DURATION(fill_vec_pair);
		for (size_t i = 0;i < 50'000; i++) {
			if (docid_count[i] > 0) {
				search_results.push_back({i, docid_count[i]});
			}
		}
		//}

		//{
		//ADD_DURATION(speed_sort);
						partial_sort(
							search_results.begin(),
							search_results.begin() + min<size_t>(search_results.size(), 5),
							search_results.end(),
							[](pair<size_t, size_t>& lhs, pair<size_t, size_t>& rhs) {
								return make_pair(lhs.second, -static_cast<int64_t>(lhs.first)) > make_pair(rhs.second, -static_cast<int64_t>(rhs.first));
							}
						);
		//}
		//{
		//ADD_DURATION(form_res);
    search_results_output << current_query << ':';
    for (auto [docid, hitcount] : Head(search_results, 5)) {
      search_results_output << " {"
        << "docid: " << docid << ", "
        << "hitcount: " << hitcount << '}';
    }
    search_results_output << '\n';
		//}
  }
}

void InvertedIndex::Add(string document) {
  docs.push_back(move(document));
	const size_t docid = docs.size() - 1;
	string_view sv_doc = docs.back();
	
	size_t pos = sv_doc.find_first_not_of(' ');
		if (pos != sv_doc.npos) {
			sv_doc.remove_prefix(pos);
		} else {return;}

	

	while (!sv_doc.empty()) {
		pos = sv_doc.find(' ');
		size_t h = hash<string_view>{}(sv_doc.substr(0, pos));
		index[h%10'000][h].push_back(docid);
		sv_doc.remove_prefix(pos != sv_doc.npos ? pos +1: sv_doc.size());
		while (!sv_doc.empty() && isspace(sv_doc.front())) {
			sv_doc.remove_prefix(1);
		}
	}
}

const vector<size_t>& InvertedIndex::Lookup(size_t h) const {

	const auto& bucket = index[h%10'000];

  if (auto it = bucket.find(h); it != bucket.end()) {
    return it->second;
  } else {
    return empty_vec;
  }
	
}

