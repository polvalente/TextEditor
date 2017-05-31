#ifndef _WRAPPER_H_
#define _WRAPPER_H_
#include <string>
#include <vector>

using std::vector;
using std::string;

class Wrapper {
	// Classe de interface para o Perl
	// Todos os metodos contem chamadas de manipulacao de pilha e chamadas do perl
	public:
        string capitalize(const string &txt); // capitalizar o conteudo de txt
        vector<int> textCount(const string &txt); // analisar contagens no conteudo de txt
		//find and replace functions return empty vector if
		//failed
		    //substituir old_str por new_str em txt a partir do indice idx
        string replace(const string &txt, const string &old_str,
                                                const string &new_str, int& count, const int &idx);
				//buscar str em txt a partir do indice idx
        vector<int> find   (const string & txt, const string &str, const int &idx);
				//balanceamento de caracteres
        int matchCharacters(const string &txt, const char &c); // can keep track of the char type using the function call
				//autocompletar word baseado no conteudo de txt
        vector<string> autocomplete(const string &txt, const string &word);
};
#endif
