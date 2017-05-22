#include "buffer.h"
#include <string>

using std::string;

Buffer::Buffer(){
}

string Buffer::clean(string line){
	int tab;
	while ((tab = line.find("\t")) != line.npos){
		line.replace(tab, 1, "    ");
	}
	return line;
}

void Buffer::insertLine(string line, int i){
	line = clean(line);
	lines.insert(lines.begin()+i, line);
}

void Buffer::appendLine(string line){
	line = clean(line);
	lines.push_back(line);
}

void Buffer::removeLine(int i){
	lines.erase(lines.begin()+i);
}

void Buffer::deleteContent(){
	lines.erase(lines.begin(), lines.end());
}
