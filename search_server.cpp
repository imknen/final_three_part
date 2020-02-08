#include "search_server.h"
#include "iterator_range.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>


SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  //InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    index.Add(move(current_document));
  }
//	sort(index.begin(), index.end(), comparator);
//  index = move(new_index);
}

	const deque<string_view> SplitToMap(string_view s)
	{
		deque<string_view> ret;

	size_t pos = s.find_first_not_of(' ');
		if (pos != s.npos) {
			s.remove_prefix(pos);
		}	else { return {}; }
	while (!s.empty()) {
		pos = s.find(' ');
    ret.push_back(s.substr(0, pos));
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


  for (string current_query; getline(query_input, current_query); ) {
    const auto words(SplitToMap(current_query));

    vector<size_t> docid_count(50'000, 0);
		{ADD_DURATION(lookup);
    for (const auto v : index) {
			for (auto s : words) {
			 if (v.first == s)
        docid_count[v.second]++;
			} 
    }

		}
		

    vector<pair<size_t, size_t>> search_results;
		search_results.reserve(55'005);
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
    search_results_output << '\n';
		}

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
		index.push_back({sv_doc.substr(0, pos), docid});
		sv_doc.remove_prefix(pos != sv_doc.npos ? pos +1: sv_doc.size());
		while (!sv_doc.empty() && isspace(sv_doc.front())) {
			sv_doc.remove_prefix(1);
		}
	}
}

