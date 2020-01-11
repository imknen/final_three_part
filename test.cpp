#include <iostream>
#include <fstream>
#include	<set>
#include <string>
#include <list>
#include <sstream>
using namespace std;


int main ()
{

	ifstream in("documents.txt");

	set<string> ss;

	size_t count = 0;
	for (string word; in >> word;) {
		count++;
		ss.insert(word);
	} cout << count << ' ' << ss.size() << endl;

	return 0;
}

