#include "search_server.h"
#include "iterator_range.h"
#include "paginator.h"
#include "duration.h"
#include <future>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>
SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void InvertedIndex::AddDocuments(istream& input)
{
	for (string document;  getline(input, document); ) {
			docs.push_back(move(document));
	}
}
void InvertedIndex::FillIndex(const IteratorRange<deque<string>::const_iterator> range, size_t numberdoc) {
	for (string_view document : range) {
		Add(document, numberdoc);
		++numberdoc;
	}
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
	if (!document_input){baseempty = true; return;} 
 InvertedIndex new_index;

  new_index.AddDocuments(document_input);
	{
	size_t numberdoc = 0;
	const size_t PAGE_SIZE =  3'000;
	for  (const IteratorRange<deque<string>::const_iterator> range  : Paginate(new_index.GetDocuments(), PAGE_SIZE)) {
	auto tmp = async(&InvertedIndex::FillIndex, &new_index, range, numberdoc);
	//йцуке
	numberdoc+=PAGE_SIZE; 
	}
	}
	baseempty = false;

	lock_guard<mutex> lock_add(m);
	swap(index,new_index);
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
    std::istream& query_input, std::ostream& search_results_output
) {
		if (baseempty) return;
    std::vector<std::string> queries;
    queries.reserve(500'000);
    for (std::string current_query; getline(query_input, current_query); ) {
        queries.push_back(move(current_query));
    }
 
    size_t thread_count = std::thread::hardware_concurrency() != 0 ?
        std::thread::hardware_concurrency() : 8;
    size_t step = queries.size() / thread_count;
 
    std::vector<std::vector<std::string>::iterator> queries_it;
    auto it = queries.begin();
    queries_it.push_back(it);
    for (size_t i = 0; i < thread_count - 1; ++i) {
        it = it + step;
        queries_it.push_back(it);
    }
    queries_it.push_back(queries.end());
 
    for (size_t i = 0; i < thread_count; ++i) {
        futures.push_back(async
        (&SearchServer::AddQueriesStreamSingleThread, this, queries_it[i], queries_it[i + 1]));
    }
    for (auto& elem : futures) {
        search_results_output << elem.get();
    }
    futures.clear();
}




string SearchServer::AddQueriesStreamSingleThread(
std::vector<std::string>::iterator begin,
std::vector<std::string>::iterator end
) {
//	TotalDuration read("Total read");
//	TotalDuration split("Total split");
//	TotalDuration lookup("Total lookup");
//	TotalDuration speed_sort("Topositiontal work sort");
//	TotalDuration form_res("Forming result");
//	TotalDuration fill_vec_pair("Total fill vect_pair");

	unordered_map<string, size_t> queries;


	std::ostringstream output;

  for (auto it = begin; it != end; ++it) {
    vector<size_t> docid_count(50'000, 0);

    const auto words(SplitToMap(*it));

		//{ADD_DURATION(lookup);
	{
	for (const auto& word : words) {
		 lock_guard<mutex> lock_search(m);
      for (const size_t docid : index.Lookup(word.first)) {
        docid_count[docid]+=word.second;
      }
			
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

		output << *it << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
            output << " {"
                << "docid: " << docid << ", "
                << "hitcount: " << hitcount << '}';
        }
        output << std::endl;


  }
	return output.str();
}

void InvertedIndex::Add(string_view sv_doc, size_t numberdoc) {
	
	size_t pos = sv_doc.find_first_not_of(' ');
		if (pos != sv_doc.npos) {
			sv_doc.remove_prefix(pos);
		} else {return;}

	while (!sv_doc.empty()) {
		pos = sv_doc.find(' ');
		size_t h = hash<string_view>{}(sv_doc.substr(0, pos));
		{
		lock_guard<mutex> lock_index(m_index);
		index[h%10'000][h].push_back(numberdoc);
		}
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

