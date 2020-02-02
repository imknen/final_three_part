#include "search_server.h"
#include "iterator_range.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <string_view>
/*
vector<string> SplitIntoWords(const string&& line) {
  istringstream words_input(move(line));
 // return {istream_iterator<string>(words_input), istream_iterator<string>()};
 vector<string> ret;
 for (string s; words_input >> s;) {
 	ret.push_back(move(s));
 }
 return ret;
}
*/
std::vector<std::string_view> SplitIntoWords(const std::string& line) {
	std::string_view str = line;
	std::vector<std::string_view> res;
	
	size_t not_space = str.find_first_not_of(' ');
	if (not_space != str.npos) {
		str.remove_prefix(not_space);
	}
	else {
		return {};
	}
	
	while (true) {
		size_t space = str.find(' ');
		res.push_back(str.substr(0, space));
		if (space == str.npos) {
			break;
		}
		else {
			str.remove_prefix(space + 1);
			size_t not_space = str.find_first_not_of(' ');
			if (not_space != str.npos) {
				str.remove_prefix(not_space);
			}
			else {
				break;
			}
		}
	}
	return res;
}

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
    const auto& words = SplitIntoWords(current_query);

    vector<size_t> docid_count(50'000, 0);
		{ADD_DURATION(lookup);
    for (const auto& word : words) {
      for (const size_t& docid : index.Lookup(word)) {
        ++docid_count[docid];
      }
    }

		}

    vector<pair<size_t, size_t>> search_results;
		//search_results.reserve(55'005);
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

  const size_t& docid = docs.size() - 1;
	auto& doc = docs.back();
  for (auto& word : SplitIntoWords(doc)) {
    index[word].push_back(docid);
  }
}

const vector<size_t>& InvertedIndex::Lookup(const string_view word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return tempor; 
  }
	
}
