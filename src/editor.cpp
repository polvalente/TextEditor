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
	commandBuffer = "";
	bufferIndex = 0;
	z_mode_output = "";

	buffer = new Buffer();
	buffer->appendLine("");

	setupHelp();	
}

void Editor::setupHelp(){
	string helpFilename="./lib/help.txt";
	std::ifstream arquivo(helpFilename.c_str());
	if(arquivo.is_open()){
		while(!arquivo.eof()){
			string line;
			getline(arquivo, line);
			helpText.push_back(line);
		}
		arquivo.close();
	}
}

Editor::Editor(Interface *i, Wrapper *w, string f) : 
	x(0),
	y(0),
	mode('n'),
	status("Modo normal"),
	filename(f),
	commandBuffer(""),
	bufferIndex(0),
	z_mode_output(""),
	interface(i),
	wrapper(w)
{
	
	//x = 0;
	//y = 0;
	//mode = 'n';
	//status = "Modo normal";
	//commandBuffer = "";
	//bufferIndex = 0;
	//z_mode_output = "";

	setupHelp();

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
		arquivo.close();
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
			status = z_mode_output;
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
		case 'h':
			status = "Ajuda";
			break;
		default:
			break;
	}
	if (mode != ':' && mode != 'z' && mode != 'h'){
		status += "\tCOLUNA: " + numToStr(x) + "\tLINHA: " + numToStr(y) + " - " +  textCount() + " " + msg;
	}
}

string Editor::numToStr(int num){
	std::ostringstream ss;
	ss.clear();
	ss << num;
	return ss.str();
}

Editor::~Editor(){
	delete buffer;
}

void Editor::del_key(){
	if (x == (int)buffer->lines->at(y).length() && y != (int)buffer->lines->size() - 1){
		buffer->lines->at(y) += buffer->lines->at(y+1);
		deleteLine(y+1);
	}
	else{
		buffer->lines->at(y).erase(x, 1);
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
			case 'o':
				mode = 'i';
				buffer->insertLine("",y+1); 
				y++;
				x = 0;
				break;
			case 'O':
				mode = 'i';
				buffer->insertLine("",y);
				x = 0;
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
			case 'h':
				mode = 'h';
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
	else if (mode == 'h'){
		switch(ch){
			case 'q':
			case 'Q':
			case MY_KEY_ESC:
				mode = 'n';
				break;
			default:
				break;
		}
	}
	else if (mode == 'd'){
		switch(ch){
			case 'd':
				buffer->removeLine(y);
				if (y > 0){
					y--;
				}
				if (buffer->lines->size() < 1){
					buffer->appendLine("");
					y = 0;
				}
				x = 0;
				mode = 'n';
				break;
			default:
				mode = 'n';
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
					x = buffer->lines->at(y-1).length();
					buffer->lines->at(y-1) += buffer->lines->at(y);
					deleteLine();
					up();
				}
				else{
					buffer->lines->at(y).erase(--x, 1);
				}
				break;
			
			case KEY_DC:
				//delete
				del_key();
				break;
			
			case KEY_ENTER:
			case '\n':
				if (x < (int)buffer->lines->at(y).length()){
					buffer->insertLine(buffer->lines->at(y).substr(x, buffer->lines->at(y).length() - x), y + 1);
					buffer->lines->at(y).erase(x, buffer->lines->at(y).length() - x);
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
					buffer->lines->at(y).insert(x, 4, ' ');
					x += 4;
				}
				else {
					autocomplete(word);
				}
				break;

			default:
				//outros caracteres
				buffer->lines->at(y).insert(x, 1, static_cast<char>(ch));
				x++;
				break;
		}
	}
	else if (mode == 'z'){
		//modo auxiliar para mostrar resultado do findReplace
		setStatus(z_mode_output);
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
				else if (saveCommand()){
					mode = 'n';
					commandBuffer = "";
					bufferIndex = 0;
				}
				else if (quitCommand()){
					mode = 'x';
				}
				else if (openCommand()){
					mode = 'n';
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
	if (x + 1 < COLS && x + 1 <= (int)buffer->lines->at(y).length()){
		interface->moveTo(y, ++x);
	}
}

void Editor::up(){
	msg = "";
	if (y-1 >= 0){
		y--;
	}
	if(x >= (int)buffer->lines->at(y).length()){
		x = buffer->lines->at(y).length();
	}
	interface->moveTo(y, x);
}

void Editor::down(){
	msg = "";
	if(y+1 < (int)LINES-1 && y+1 < (int) buffer->lines->size()){
		y++;
	}
	if((unsigned)x >= buffer->lines->at(y).length()){
		x = buffer->lines->at(y).length();
	}
	interface->moveTo(y, x);
}

const vector<string>& Editor::getBuffer(){
	if (mode == 'h')
		return helpText;
	return *(buffer->lines);
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
			for(unsigned i=0; i < buffer->lines->size(); i++){
				if (buffer->lines->at(i) != "")
					arquivo << buffer->lines->at(i) << std::endl;
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
	string line = buffer->lines->at(y);
	string word = "";
	if(line == ""){
		word = "";
	}
	else{
		std::stringstream ss(line.substr(0,x));
		if (x == 0 || x > (int) line.length() || !isWordCharacter(line[x-1])){
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
	if(txt.length() < 1){
		coords = vector<int>(2,-1);
		return coords;
	}
	int idx;
	char list[] = "([{";
	for (int i = 0; i < 3; i++){
		idx = wrapper->matchCharacters(txt, list[i]);
		if (idx != -1)
			break;
	}

	if (idx == -1){
		coords = vector<int>(2,-1);
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
	for(size_t i = 0; i < buffer->lines->size(); i++){
		txt += buffer->lines->at(i) + "\n";
	}
	if (txt.length() >= 1)
		txt = txt.substr(0, txt.size()-1);
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
	
	if(comando.size() < 2 || comando.size() > 4 || (comando[0] != "" && comando[0] != "s" && comando[0] != "ss")){
		return false;
	}

	if (comando[comando.size()-1] == "g")
		all = true;

	string txt; 
	if (comando[0] == "s"){
		txt = buffer->lines->at(y);
	}
	else {
		txt = getBufferTxt();	
	}
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
			z_mode_output = word+" nao encontrada";
			return true;
		}
		z_mode_output = word+" encontrada em (LINHA, COLUNA): ";
		vector<int> coord;
		size_t i;
		for (i = 0; i < findResult.size(); i++){
			coord = convertIdxToRowCol(findResult[i], txt);	
			z_mode_output += "("+numToStr(coord[0])+", "+numToStr(coord[1])+"), ";
		}
		if(i > 0){
			z_mode_output = z_mode_output.substr(0, z_mode_output.size()-2);
		}
		
		return true;
	}
	else if (comando[0] == "s" || comando[0] == "ss"){
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
		if (comando[0] == "ss"){
			buffer->deleteContent();
			for (size_t i = 0; i < replaceResult.size(); i++){
			//for (size_t i = 0; i < comando.size(); i++){
				//buffer->appendLine(comando[i]);
				buffer->appendLine(replaceResult[i]);
			}
		}
		else {
			buffer->removeLine(y);
			buffer->insertLine(replaceResult[0], y);
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
	for(int r = 0; r < y; r++){
		out += buffer->lines->at(r).length()+1;
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
	if (txt.length() > 0){
		result = wrapper->textCount(getBufferTxt());
	}
	else{
		result = vector<int>(4,0);
	}
	ss << "chrs: " << result[0] << " words: " << result[1] << " nwspc chrs: " << result[2] << " lines: " << result[3];
	return ss.str();
}

bool Editor::saveCommand(){
	vector<string> comando = split(commandBuffer, ' ');
	if (comando.size() > 2 || comando.size() < 1|| comando[0] != "w"){
		return false;
	}
	else if (comando.size() == 1) {
		comando.push_back(filename);
	}

	string old_filename = filename;
	filename = comando[1];
	save();
	filename = old_filename;

	return true;
}

bool Editor::quitCommand(){
	string char0 = "";
	string char1 = "";

	if (commandBuffer.length() < 1){
		return false;
	}
	else if (commandBuffer.length() == 2){
		char1 = commandBuffer[1];		
	}
	else if (commandBuffer.length() > 2){
		return false;
	}

	char0 = commandBuffer[0];

	if (char0 == "q" && commandBuffer.length() == 1){
		//quit without save
		return true;
	}
	else if (char0 == "x" || (char0 == "w" && char1 == "q")){
		//save and quit
		save();
		return true;
	}
	return false;
}

bool Editor::openCommand(){
	vector<string> comando = split(commandBuffer, ' ');
	if (comando[0] != "e" || comando.size() != 2)
		return false;
	
	string fname = comando[1];

	std::ifstream arquivo(fname.c_str());
	bool opened = false;
	if((opened = arquivo.is_open())){
		x = 0;
		y = 0;
		opened = true;
		buffer->deleteContent();
		while(!arquivo.eof()){
			string line;
			getline(arquivo, line);
			buffer->appendLine(line);
		}

		filename = fname;
		msg = "Arquivo aberto: "+filename;
		arquivo.close();
	}
	else {
		x = 0;
		y = 0;
		filename = fname;
		buffer->deleteContent();
		buffer->appendLine("");
		msg = "Novo arquivo aberto: "+filename;
	}

	return true;
}

bool Editor::bufferEmpty(){
	return (buffer->lines->size() == 1 && buffer->lines->at(0) == "");
}	
