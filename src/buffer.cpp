#include "buffer.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

Buffer::Buffer(){
	lines = new vector<string>(0);
}

Buffer::~Buffer(){
	delete lines;
}


string Buffer::clean(string line){
	size_t tab;
	while ((tab = line.find("\t")) != line.npos){
		line.replace(tab, 1, "    ");
	}
	return line;
}

void Buffer::insertLine(string line, int i){
	line = clean(line);
	lines->insert(lines->begin()+i, line);
}

void Buffer::appendLine(string line){
	line = clean(line);
	lines->push_back(line);
}

void Buffer::removeLine(int i){
	if (i >= 0 && lines->size() >= 1)
		lines->erase(lines->begin()+i);
}

void Buffer::deleteContent(){
	lines->erase(lines->begin(), lines->end());
}
