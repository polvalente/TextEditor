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

Interface::Interface(string fn) : filename(fn) { // construtor que recebe nome do arquivo
}

void Interface::init(){
	// Inicializando a janela principal e
	// Desabilitando echo de teclas, buffer de linha
	// habilitando o teclado para teclas especiais
	initscr();
	noecho();
	cbreak();
	curs_set(1);
	keypad(stdscr, true);
}

int Interface::mainLoop(){
	//loop principal da interface grafica
	bool exit = false;
	int retval = 0; // valor de retorno para caso queira retornar algum erro
	while(!exit){
		editor->updateStatus(); // atualizar mensagem de status
		
		string status = editor->getStatus(); // ler status e imprimir na tela
		printStatus(status);
		
		vector<string> bufferLines = editor->getBuffer(); // ler buffer e imprimir na tela
		printBuffer(bufferLines);

		move(editor->getY(), editor->getX()); // posicionar o cursor

		int ch = getch(); // recebe um caracter de entrada

		editor->input(ch); // envia o caracter recebido para o editor

		if(!(editor->bufferEmpty())){ // se o buffer nao estiver vazio, executa a funcao de balanceamento de caracteres
			matchCharacters();
		}

		refresh(); // atualiza a tela
		if (editor->getMode() == 'x'){ // se estiver em modo 'x', sair
			exit = true;
		}
	}
	endwin(); // saiu do loop, entao fechar a janela principal
	return retval; // retornar o codigo de erro da funcao
}

void Interface::printBuffer(vector<string> &lines){ // funcao de impressao do buffer
	for(int i = 0; i < LINES-1; i++){ // percorre o vetor de linhas
		if (i >= (int)lines.size()){ // se estiver apos o final do buffer, move o cursor para a o inicio da linha sem fazer nada a mais
			move(i, 0);
		}
		else{ // caso contrario, imprime o buffer na linha atual
			string line = lines[i];
			mvprintw(i, 0, "%s", line.c_str()); // "%s" para escapar chars como %
		}
		clrtoeol(); // limpa a tela apos cursor
	}
}

void Interface::printStatus(string &status){ // impressao da barra de status
	if (editor->getMode() != ':')
		attron(A_REVERSE); // nao inverte se for para receber comandos
	mvprintw(LINES-1, 0, "%s", status.c_str()); // %s para escapar chars como %
	clrtoeol(); // limpa a linha apos o status
	if (editor->getMode() != ':')
		attroff(A_REVERSE); // soh desliga o atributo se ligou anteriormente
}

void Interface::setEditor(Editor *e){ // setter para o editor da interface
	editor = e;
}

void Interface::moveTo(int x, int y){ // funcao para mover o cursor da interface
	move(x, y);
}

bool Interface::autocomplete(vector<string> wordList, string word){ // metodo de autocompletar
	bool exit = false;
	size_t i = 0;
	if (wordList.size() == 0){
		return false;
	}
	else if (wordList.size() == 1){ // se soh tem uma palavra na lista, completa com ela
		for(size_t j = word.length(); j < wordList[0].length(); j++)
			editor->input(wordList[0][j]); // envia os caracteres que ainda nao estao no buffer (ignora o prefixo completado) para o editor
		return true;
	}

	string prev_msg = editor->getMsg(); // guarda a mensagem atual 
	while(!exit){
		editor->setStatus(" -- "+wordList[i]); // configura o status com a palavra selecionada atualmente
		string status = editor->getStatus(); // recebe o status
		printStatus(status); // imprime o status recebido
		refresh(); // atualiza a tela
		int c = getch(); // recebe entrada do teclado
		switch(c){
			//teclas direcionais para cima e para baixo navegam na lista de palavras
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
				//tab ou enter seleciona a palavra para completar
				for(unsigned j = word.length(); j < wordList[i].length(); j++)
					editor->input(wordList[i][j]); // envia os caracteres restantes ao editor
				exit = true;
				break;
			case MY_KEY_ESC:
				exit = true; // ESC sai sem autocompletar
				break;
			default:
				break;
		}
	}
	
	editor->setStatus(prev_msg); // volta com o status anterior
	string status = editor->getStatus(); // recebe o status restaurado
	printStatus(status); // imprime o status restaurado
	refresh(); // atualiza a tela
	return true;
}

void Interface::matchCharacters(){ // invoca o balanceamento de caracteres do editor
	vector<int> idx = editor->matchCharacters(); // retorna a linha e a coluna problematicas
	string msg;
	if (idx[0] == -1){ // se retornou -1, nao houve erro
		msg = editor->getMsg();
	}
	else{
		//houve erro, acusar na mensagem complementar do status
		msg = "- DESBALANCEADO - COLUNA: "+editor->numToStr(idx[1])+" LINHA: "+editor->numToStr(idx[0]); 
	}
	//modificar a mensagem complementar de status
	editor->setStatus(msg);
}

void Interface::refreshscr(){
	refresh(); // atualiza a tela da interface grafica
}

