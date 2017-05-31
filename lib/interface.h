#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "wrapper.h"
#include "editor.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class Editor;

class Interface {
	//Interface grafica do editor
	public:
		Interface(string); // construtor nao-padrao para a interface grafica
		void init(); // inicializa a ncurses
		int mainLoop(); // loop principal do programa
		void setEditor(Editor*); // setter para o ponteiro do editor
		void moveTo(int, int); // move o cursor para as coordenadas especificadas

		bool autocomplete(vector<string>, string); // chama a funcao de autocompletar palavra
		void refreshscr(); // atualiza a tela

	private:
		string filename; // nome do arquivo atual (usado no construtor)
		Editor *editor; // ponteiro para o 'backend' do editor

		void printBuffer(vector<string> &); // imprime o buffer na tela
		void printStatus(string &); // imprime a barra de status

		void matchCharacters(); // chama a funcao de balancear caracteres
};

#endif
