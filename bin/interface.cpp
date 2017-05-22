#include "interface.h"
#include "editor.h"
#include "ncurses.h"

#include <string>
#include <vector>
#include <iostream>
using std::string;
using std::vector;
using std::cout;
using std::endl;

#define MY_KEY_ESC 27
#define MY_KEY_BACKSPC 127

Interface::Interface(string fn) : filename(fn) {
}

void Interface::init(){
	// Inicializando a janela principal e
	// Desabilitando echo de teclas, buffer de linha
	// habilitando o teclado para teclas especiais
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, true);
}

int Interface::mainLoop(){
	bool exit = false;
	int retval = 0;
	while(!exit){
		editor->updateStatus();
		
		string status = editor->getStatus();
		printStatus(status);
		
		vector<string> bufferLines = editor->getBuffer();
		printBuffer(bufferLines);

		move(editor->getY(), editor->getX());

		int ch = getch();

		editor->input(ch);

		matchCharacters();

		refresh();
		if (editor->getMode() == 'x'){
			exit = true;
		}
	}
	endwin();
	return retval;
}

void Interface::printBuffer(vector<string> &lines){
	for(int i = 0; i < LINES-1; i++){
		if (i >= lines.size()){
			move(i, 0);
		}
		else{
			mvprintw(i, 0, lines[i].c_str());
		}
		clrtoeol();
	}
}

void Interface::printStatus(string &status){
	attron(A_REVERSE);
	mvprintw(LINES-1, 0, status.c_str());
	clrtoeol();
	attroff(A_REVERSE);
}

void Interface::setEditor(Editor *e){
	editor = e;
}

void Interface::moveTo(int x, int y){
	move(x, y);
}

void Interface::autocomplete(vector<string> wordList, string word){
	bool exit = false;
	int i = 0;
	string prev_msg = editor->getMsg();
	refresh();
	while(!exit){
		editor->setStatus(" -- "+wordList[i]);
		string status = editor->getStatus();
		printStatus(status);
		refresh();
		int c = getch();
		switch(c){
			case KEY_UP:
				i = (i > 0) ? i - 1 : 0;
				break;
			case KEY_DOWN:
				i = (i < wordList.size() - 1) ? i + 1 : wordList.size() - 1;
				break;
			
			case KEY_BTAB:
			case KEY_CTAB:
			case KEY_STAB:
			case 9:
			case KEY_ENTER:
			case '\n':
				for(int j = word.length(); j < wordList[i].length(); j++)
					editor->input(wordList[i][j]);
				exit = true;
				break;
			case MY_KEY_ESC:
				exit = true;
				break;
			default:
				break;
		}
	}
	
	editor->setStatus(prev_msg);
	string status = editor->getStatus();
	printStatus(status);
	refresh();
}

void Interface::matchCharacters(){
	vector<int> idx = editor->matchCharacters(); // returns row, col
	/*if (idx[0] == -1){
		return;
	}
	attron(A_REVERSE);
	char s[] = " ";
	char c = editor->getBuffer()[idx[0]][idx[1]];
	s[0] = c;
	mvprintw(idx[0], idx[1], s); 
	attroff(A_REVERSE);
	refresh();*/
	string msg;
	if (idx[0] == -1){
		msg = editor->getMsg();
	}
	else{
		msg = "- DESBALANCEADO - COLUNA: "+editor->numToStr(idx[1])+" LINHA: "+editor->numToStr(idx[0]);
	}
	editor->setStatus(msg);
}

void Interface::refreshscr(){
	refresh();
}

