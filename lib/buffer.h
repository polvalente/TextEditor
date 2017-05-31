#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

class Buffer {
	public:
		// Construtor e destrutor padrao
		Buffer();
		~Buffer();

		vector<string> *lines; // Ponteiro para o vector que guarda o conteudo do Buffer


		// Funcoes 'helper' para manipulacao do conteudo do buffer
		void insertLine(string, int); // inserir uma linha em posicao especifica
		void removeLine(int); // remover linha
		void appendLine(string); // adicionar linha no final do buffer
		void deleteContent(); // apagar o conteudo do buffer

	private:
		// substitui tabs por 4 espacos para facilitar manipulacao de conteudo 
		// e correspondencia entre a interface grafica e o conteudo
		string clean(string); 
};

#endif
