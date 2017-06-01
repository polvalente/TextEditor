#include "buffer.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

Buffer::Buffer(){
	lines = new vector<string>(0); // Alocar vector vazio para linhas do buffer. Alocacao dinamica para evitar estouro de pilha
}

Buffer::~Buffer(){
	delete lines; // apaga o vector alocado
}


string Buffer::clean(string line){
	//Metodo que substitui tabs por 4 espacos na string recebida e retorna uma versao modificada dela
	size_t tab;
	while ((tab = line.find("\t")) != line.npos){
		line.replace(tab, 1, "    ");
	}
	return line;
}

void Buffer::insertLine(string line, int i){
	//Metodo que insere a string 'line' na posicao i do vector de linhas (lines)
	line = clean(line);
	lines->insert(lines->begin()+i, line);
}

void Buffer::appendLine(string line){
	//Metodo que insere a string 'line' no final do vector de linhas (lines)
	line = clean(line);
	lines->push_back(line);
}

void Buffer::removeLine(int i){
	//Metodo que remove a string 'line' na posicao i do vector de linhas (lines)
	if (i >= 0 && lines->size() >= 1)
		lines->erase(lines->begin()+i);
}

void Buffer::deleteContent(){
	//Metodo que apaga todo  conteudo do vector de linhas (lines)
	lines->erase(lines->begin(), lines->end());
}
