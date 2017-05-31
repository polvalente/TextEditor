#ifndef _EDITOR_H_
#define _EDITOR_H_


#include <string>
#include <vector>
using std::string;
using std::vector;

#include "buffer.h"
#include "wrapper.h"

//Constantes para codigos ascii/unicode de teclas
#define MY_KEY_ESC 27
#define MY_KEY_BACKSPC 127

class Interface; // Declaracao de nome para evitar loop de "include"

class Editor {
	//Backend do editor
	private:
		int x, y; // posicao do cursor no buffer
		char mode; // modo de operacao do editor
		string msg; // mensagem complementar da barra de status
		vector<string> helpText; // conteudo do arquivo de ajuda 'help.txt'
		string status, filename; // mensagem da barra de status e nome do arquivo atual
		string commandBuffer; // comando recebido ate o momento (usado no modo ':')
		size_t bufferIndex; // posicao atual do cursor no comando atual
		string z_mode_output; // mensagem da barra de status no modo auxiliar 'z'

		Buffer *buffer; // ponteiro para o buffer de trabalho
		Interface *interface; // ponteiro para a interface grafica
		Wrapper *wrapper; // ponteiro para o 'wrapper' que encapsula as chamadas para o Perl

		void setupHelp(); // funcao para ler o arquivo 'help.txt'

		// funcoes para deslocar o cursor na tela
		void up(); 
		void down();
		void left();
		void right();

		// funcoes relacionadas a deletar conteudo
		void deleteLine(); // deleta linha atual
		void deleteLine(int); // deleta uma linha especifica
		void del_key(); // encapsula o comportamento da tecla DEL

		void save(); // salva buffer para o arquivo 'filename'
		bool isWordCharacter(char); // retorna verdadeiro se o caracter de entrada pode ser parte de uma palavra
		string getWordBeforeCursor(); // retorna a palavra antes do cursor

		//funcoes com interface para o Perl
		bool autocomplete(string); // autocompletar uma palavra
		void capitalize(); // capitalizar conteudo do buffer
		string textCount(); // analisar o conteudo do buffer atual
		vector<int> convertIdxToRowCol(int, const string&); // converte indices lineares na string de conteudo para coordenadas (linha, coluna)
		string getBufferTxt(); // retorna uma string com o conteudo do buffer 

		vector<string> split(const string, const char=' '); // divide uma string em um vector<string> no caracter especificado como separador. Por padrao, divide nos espacos

		template<class T>
		string join(vector<T>, string); // junta os elementos de um vector em uma string, unindo atraves da string especificada

		size_t rowColToIdx(); // converte coordenadas do tipo (linha, coluna) para indices lineares da string de conteudo


		// funcoes para parsear o 'commandBuffer', retornam true se era o comando esperado
		bool saveCommand(); // ':w' ou ':wq'
		bool quitCommand(); // ':q'
		bool findReplace(); // comando de find ou replace
		bool openCommand(); // ':e'


	public:
		Editor(); // Construtor padrao
		Editor(Interface*, Wrapper*); // Construtor alternativo 1
		Editor(Interface*, Wrapper*, string filename); // Construtor alternativo 2
		~Editor(); // Destrutor

		char getMode(); // retorna o modo atual de operacao
		string numToStr(int); // converte numero para string

		void input(int); // lida com entrada do teclado no editor
		const vector<string>& getBuffer(); // getter para o conteudo do buffer
		const string& getStatus(); // getter para o status atual
		void setStatus(string); // setter para a mensagem complementar do status
		void updateStatus(); // atualiza o status baseado no modo de operacao e na mensagem complementar
		vector<int> matchCharacters(); // balanceamento de caracteres - ponte para o perl
		string getMsg(); // retorna a mensagem complementar de status 

		int getX(); // getter para a coluna atual do cursor
		int getY(); // getter para a linha atual do cursor

		bool bufferEmpty(); // define se o buffer esta vazio
};

#endif
