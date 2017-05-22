#include "ncurses.h"
#include "editor.h"
#include "interface.h"
#include "wrapper.h"

#include <fstream>
#include <iostream>

#include <string>
#include <vector>
#include <locale>
#include <sstream>
using std::string;
using std::vector;

Editor::Editor(Interface *i, Wrapper *w) : interface(i), wrapper(w){
	x = 0;
	y = 0;
	mode = 'n';
	status = "Modo Normal";
	filename = "untitled";

	buffer = new Buffer();
	buffer->appendLine("");
}

Editor::Editor(Interface *i, Wrapper *w, string f) : interface(i), wrapper(w), filename(f) {
	x = 0;
	y = 0;
	mode = 'n';
	status = "Modo normal";

	buffer = new Buffer();

	//lendo conteudo do arquivo pro buffer
	
	std::ifstream arquivo(filename.c_str());
	if(arquivo.is_open()){
		while(!arquivo.eof()){
			string line;
			getline(arquivo, line);
			buffer->appendLine(line);
		}
		msg = "Arquivo aberto: "+filename;
	}
	else {
		//std::cerr << "Erro ao abrir arquivo: '" << filename << "'\n";
		buffer->appendLine("");
		msg = "Novo arquivo aberto: untitled";
	}
}

void Editor::updateStatus(){
	switch(mode){
		case 'n':
			status = "Modo Normal";
			break;
		case 'i':
			status = "Modo de Insercao";
			break;
		case 'x':
			status = "Saindo";
			break;
		case 'z':
			status = findreplace_output;
			msg = "";
			break;
		case ':':
			status = ":";
			status+= commandBuffer;
			msg = "";
			break;
		case 'd':
			status = "Modo de Remocao";
			break;
		default:
			break;
	}
	if (mode != ':' && mode != 'z'){
		status += "\tCOLUNA: " + numToStr(x) + "\tLINHA: " + numToStr(y) + " " + msg + " - " + textCount();
	}
}

string Editor::numToStr(int num){
	return std::to_string(num);
}

Editor::~Editor(){
	delete buffer;
}

void Editor::del_key(){
	if (x == buffer->lines[y].length() && y != buffer->lines.size() - 1){
		buffer->lines[y] += buffer->lines[y+1];
		deleteLine(y+1);
	}
	else{
		buffer->lines[y].erase(x, 1);
	}
}

void Editor::input(int ch){
	string word;
	switch(ch){
		case KEY_LEFT:
			left();
			return;
		case KEY_RIGHT:
			right();
			return;
		case KEY_UP:
			up();
			return;
		case KEY_DOWN:
				down();
				return;
		default:
				break;
	}

	if (mode == 'n'){
			switch(ch){
			case 'x':
				mode = 'x';
				break;
			case 'd':
				mode = 'd';
				break;
			case 'i':
				mode = 'i';
				break;
			case 'w':
				save();
				break;
			case ':':
				mode = ':';
				commandBuffer = "";
				break;
			case 'c':
			case 'C':
				capitalize();
				break;
			case KEY_DC:
				del_key();
				break;
			default:
				break;
			}
	}
	else if (mode == 'd'){
		switch(ch){
			case 'd':
				buffer->removeLine(y--);
				if (y < 0){
					buffer->appendLine("");
					y = 0;
				}
				x = 0;
				mode = 'n';
				break;
			case MY_KEY_ESC:
				mode = 'n';		
				break;
			case KEY_UP:
				up();
				break;
			case KEY_DOWN:
				down();
				break;
			case KEY_LEFT:
				left();
				break;
			case KEY_RIGHT:
				right();
				break;
			default:
				break;
		}
	}
	else if (mode == 'i'){
		switch(ch){
			case MY_KEY_ESC:
				mode = 'n';
				break;
			
			case MY_KEY_BACKSPC:
			case KEY_BACKSPACE:
				if (x == 0 && y == 0){
				}
				else if (x == 0 && y > 0){
					// primeira coluna, apaga no final da linha
					// anterior
					x = buffer->lines[y-1].length();
					buffer->lines[y-1] += buffer->lines[y];
					deleteLine();
					up();
				}
				else{
					buffer->lines[y].erase(--x, 1);
				}
				break;
			
			case KEY_DC:
				//delete
				del_key();
				break;
			
			case KEY_ENTER:
			case '\n':
				if (x < buffer->lines[y].length()){
					buffer->insertLine(buffer->lines[y].substr(x, buffer->lines[y].length() - x), y + 1);
					buffer->lines[y].erase(x, buffer->lines[y].length() - x);
				}
				else{
					buffer->insertLine("", y+1);
				}
				x = 0;
				down();
				break;

			case KEY_BTAB:
			case KEY_CTAB:
			case KEY_STAB:
			case 9:
				//tab
				word = getWordBeforeCursor();
				if (x == 0 || word == ""){
					buffer->lines[y].insert(x, 4, ' ');
					x += 4;
				}
				else {
					autocomplete(word);
				}
				break;

			default:
				//outros caracteres
				buffer->lines[y].insert(x, 1, static_cast<char>(ch));
				x++;
				break;
		}
	}
	else if (mode == 'z'){
		//modo auxiliar para mostrar resultado do findReplace
		setStatus(findreplace_output);
		switch(ch){
			case KEY_ENTER:
			case '\n':
			case KEY_UP:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
			case MY_KEY_BACKSPC:
			case KEY_BACKSPACE:
			case MY_KEY_ESC:
				mode = 'n';
				setStatus("");
				break;
			case 'i':
				mode = 'i';
				setStatus("");
				break;
			case ':':
				mode = ':';
				break;
			default:
				break;
		}
		
	}
	else if (mode == ':'){
	/*modo de comando*
	 *
	 *    find next:  /palavra
	 *     find all:  /palavra/g
	 * replace next: s/old/new
	 *  replace all: s/old/new/g
	 */
		switch(ch){
			case KEY_LEFT:
				bufferIndex = (bufferIndex > 0) ? bufferIndex - 1 : 0;
				break;
			case KEY_RIGHT:
				bufferIndex = (bufferIndex < commandBuffer.length()) ? bufferIndex + 1 : commandBuffer.length();
				break;

			case MY_KEY_BACKSPC:
			case KEY_BACKSPACE:
				if (commandBuffer.size() > 0)
					commandBuffer.erase(--bufferIndex, 1);
				break;

			case KEY_DC:
				if (bufferIndex < commandBuffer.length())
					commandBuffer.erase(bufferIndex, 1);
				break;

			case KEY_ENTER:
			case '\n':
				if(findReplace()){
					mode = 'z';
					commandBuffer = "";
					bufferIndex = 0;
				}
				return;
				break;

			case MY_KEY_ESC:
				mode = 'n';
				commandBuffer = "";
				bufferIndex = 0;
				return;
				break;

			default:
				if (bufferIndex > commandBuffer.length())
					bufferIndex = commandBuffer.length();
				commandBuffer.insert(bufferIndex++, 1, static_cast<char>(ch));
				msg = commandBuffer;
				break;
		}//end switch
	}
}

void Editor::left(){
	msg = "";
	if (x-1 >= 0){
		interface->moveTo(y, --x);
	}
}

void Editor::right(){
	msg = "";
	if (x + 1 < COLS && x + 1 <= buffer->lines[y].length()){
		interface->moveTo(y, ++x);
	}
}

void Editor::up(){
	msg = "";
	if (y-1 >= 0){
		y--;
	}
	if(x >= buffer->lines[y].length()){
		x = buffer->lines[y].length();
	}
	interface->moveTo(y, x);
}

void Editor::down(){
	msg = "";
	if(y+1 < LINES-1 && y+1 < buffer->lines.size()){
		y++;
	}
	if(x >= buffer->lines[y].length()){
		x = buffer->lines[y].length();
	}
	interface->moveTo(y, x);
}

const vector<string>& Editor::getBuffer(){
	return buffer->lines;
}

const string& Editor::getStatus(){
	return status; 
}

void Editor::deleteLine(){
	buffer->removeLine(y);
}

void Editor::deleteLine(int i){
	buffer->removeLine(i);
}

void Editor::save(){
	bool std_name = false;
	if(filename == ""){
		filename = "untitled"; 
		std_name = true;
	}

	if(!std_name){
		std::ofstream arquivo(filename.c_str());
		if(arquivo.is_open()){
			for(int i=0; i < buffer->lines.size(); i++){
				if (buffer->lines[i] != "")
					arquivo << buffer->lines[i] << std::endl;
			}
			msg = "Arquivo '"+filename+"' salvo!";
		}
		else{
			msg = "Erro ao abrir o arquivo '"+filename+"'.";
		}
		arquivo.close();
	}
}

int Editor::getX(){
	if (mode == ':'){
		return bufferIndex;
	}
	return x;
}

int Editor::getY(){
	if (mode == ':'){
		return LINES-1; 
	}
	return y;
}

char Editor::getMode(){
	return mode;
}

string Editor::getWordBeforeCursor(){
	string line = buffer->lines[y];
	string word = "";
	if(line == ""){
		word = "";
	}
	else{
		std::stringstream ss(line.substr(0,x));
		if (x == 0 || x > line.length() || !isWordCharacter(line[x-1])){
			word = "";
		}
		else{
			while(ss >> word);
		}
	}
	return word;
}

bool Editor::isWordCharacter(char c){
	return std::isalnum(static_cast<int>(c)) || (c == '_') || (c == '-');
}

void Editor::autocomplete(string word){
	string txt = getBufferTxt();
	vector<string> wordList = wrapper->autocomplete(txt, word);
	interface->autocomplete(wordList, word);
}

void Editor::setStatus(string msg){
	this->msg = msg;
	updateStatus(); 
}


vector<int> Editor::matchCharacters(){
	vector<int> coords;
	string txt = getBufferTxt();
	int idx;
	char list[] = "([{";
	for (int i = 0; i < 3; i++){
		idx = wrapper->matchCharacters(txt, list[i]);
		if (idx != -1)
			break;
	}

	if (idx == -1){
		coords.push_back(-1);
		coords.push_back(-1);
		return coords;
	}
	
	coords = convertIdxToRowCol(idx, txt);
	return coords;
}

vector<int> Editor::convertIdxToRowCol(int idx, const string& txt){
	vector<int> out(2);
	int row = 0;
	int col = 0;
	for (int i = 1; i <= idx; i++){
		if (txt[i-1] == '\n'){
			col = 0;
			row++;
		}
		else{
			col++;
		}
	}
	out[0] = row;
	out[1] = col;
	return out;
}

string Editor::getBufferTxt(){
	string txt = "";
	for(int i = 0; i < buffer->lines.size(); i++){
		txt += buffer->lines[i] + "\n";
	}
	txt.pop_back();
	return txt;
}

void Editor::capitalize(){
	string txt = wrapper->capitalize(getBufferTxt());
	std::istringstream ss(txt);
	string line;
	if (txt.length() < 1)
		return;
	buffer->deleteContent();
	while(!ss.eof()){
		getline(ss,line);
		buffer->appendLine(line);	
	}
}

bool Editor::findReplace(){
	/*find next: /word
	 *find all: /word/g
	 *replace next: s/old/new
	 *replace all: s/old/new/g
	 */
	bool all = false;

	std::istringstream stream(commandBuffer);
	string token;
	vector<string> comando = split(commandBuffer, '/');
	
	if(comando.size() < 2 || comando.size() > 4 || (comando[0] != "" && comando[0] != "s")){
		return false;
	}

	if (comando[comando.size()-1] == "g")
		all = true;

	string txt = getBufferTxt();
	string &word = comando[1];
	vector<int> findResult;
	if (comando[0] == ""){
		//find
		if (all){
			//find all
			findResult = wrapper->find(txt, word, -1);
		}
		else{
			//find next
			findResult = wrapper->find(txt, word, rowColToIdx());
		}
		
		if (findResult.size() < 1 || findResult[0] == -1){
			findreplace_output = word+" nao encontrada";
			return true;
		}
		findreplace_output = word+" encontrada em (LINHA, COLUNA): ";
		vector<int> coord;
		size_t i;
		for (i = 0; i < findResult.size(); i++){
			coord = convertIdxToRowCol(findResult[i], txt);	
			findreplace_output += "("+numToStr(coord[0])+", "+numToStr(coord[1])+"), ";
		}
		if(i > 0){
			findreplace_output.pop_back();
			findreplace_output.pop_back();
		}
		
		return true;
	}
	else if (comando[0] == "s"){
		//replace
		string &new_word = comando[2];
		if (comando.size() == 3 && comando[2] == "g"){
			all = false;
		}
		int count = 0;
		string tmp;

		if (all){
			//replace all
			tmp = wrapper->replace(txt, word, new_word, count, -1);
		}
		else{
			//replace next
			tmp = wrapper->replace(txt, word, new_word, count, rowColToIdx());
		}
		vector<string> replaceResult = split(tmp);
		buffer->deleteContent();
		for (size_t i = 0; i < replaceResult.size(); i++){
		//for (size_t i = 0; i < comando.size(); i++){
			//buffer->appendLine(comando[i]);
			buffer->appendLine(replaceResult[i]);
		}
		return true;
	}
	else{
		return false; //nao deve acontecer
	}
	interface->refreshscr();
	return true;
}

size_t Editor::rowColToIdx(){
	size_t out = 0;
	for(size_t r = 0; r < y; r++){
		out += buffer->lines[r].length()+1;
	}
	out += x + 1;
	return out;
}

template <class T>
string Editor::join(vector<T> v, string sep){
	if (v.size() < 1)
		return "";
	std::ostringstream oss("");
	size_t i = 0;
	for(i = 0; i < v.size() - 1; i++){
		oss << v[i] << sep;
	}
	oss << v[i];
	return oss.str();
}

vector<string> Editor::split(const string s, const char sep){
	vector<string> output;
	std::istringstream ss(s);
	string tok;
	while(std::getline(ss, tok, sep)){
		output.push_back(tok);
	}
	return output;
}

string Editor::getMsg(){
	return msg;
}

string Editor::textCount(){
	std::ostringstream ss;
	string txt = getBufferTxt();
	vector<int> result(4);
  result = wrapper->textCount(getBufferTxt());
	ss << "chrs: " << result[0] << " words: " << result[1] << " nwspc chrs: " << result[2] << " lines: " << result[3];
	return ss.str();
}
