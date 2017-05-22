#ifndef _WRAPPER_H_
#define _WRAPPER_H_
#include <string>
#include <vector>

using std::vector;
using std::string;

class Wrapper {
	public:
        string capitalize(const string &txt);
        vector<int> textCount(const string &txt);
		//find and replace functions return empty vector if
		//failed
        string replace(const string &txt, const string &old_str,
                                                const string &new_str, int& count, const int &idx);
        vector<int> find   (const string & txt, const string &str, const int &idx);
        int matchCharacters(const string &txt, const char &c); // can keep track of the char type using the function call
        vector<string> autocomplete(const string &txt, const string &word);
};
#endif
