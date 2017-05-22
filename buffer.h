#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

class Buffer {
	public:
		Buffer();

		vector<string> lines;

		void insertLine(string, int);
		void removeLine(int);
		void appendLine(string);
		void deleteContent();

	private:
		string clean(string); // substitutes tabs with 4 spaces
};

#endif
