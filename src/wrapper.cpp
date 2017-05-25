#include <iostream>
#include <string>
#include <vector>

#include "wrapper.h"

#include <EXTERN.h>
#include <perl.h>



/* Perl functions:
 * @word_list = autocomplete($txt, $word, $dict)
 *
 * @($char, $pos) = matchCharacters($txt, $char)
 * 
 * $index = findNext($txt, $word, $idx)
 * @index_list = findAll($txt, $word)
 * @($success_bool, $mod_txt) = replaceNext($txt, $old, $new, $idx)
 * @($success_bool, $mod_txt) = replaceAll($txt, $old, $new)
 *
 * @($chars, $words, $nwsp_chars, $lines) = textCount($txt)
 *
 * $mod_txt = capitalize($txt);
 */

/* Interface entre C/PERL:
 * dSP; 1 - inicializa a pilha
 * ENTER; 2 - tudo criado a partir daqui sao variaveis temporarias
 * SAVETMPS; 
 * PUSHMARK(SP); 3 - guarda o ponteiro da pilha 
 * 
 * 4 - Enviar variaveis na ordem em que sao chamadas pela funcao (esquerda pra direita)
 *     Executar as funcoes abaixo conforme o necessario
 *
 * XPUSHs(sv_2mortal(newSViv(inteiro)));
 * XPUSHs(sv_2mortal(newSVpv(char [])));
 * XPUSHs(sv_2mortal(newSVnv(float)));
 *
 * PUTBACK; 5 - transforma o ponteiro da pilha em global
 * 
 * 6 - chamada da funcao
 * call_pv("nome_da_funcao", G_SCALAR)                       : se ela retorna um valor escalar, chamar assim
 * int tamanho_do_array = call_pv("nome_da_funcao", G_ARRAY) : se ela retorna um array, chamar assim
 *
 * SPAGAIN; 7 - renova o ponteiro da pilha
 *
 * 8 - remover os valores retornados da pilha.
 *     se for um array, os valores são retornados da direita pra esquerda.
 *     Usar as seguintes macros:
 *     POPp; se for string
 *     POPi; se for inteiro
 *     POPn; se for double
 * 
 * PUTBACK; 9 - finalizando a chamada de funcao do interpretador 
 * FREETMPS;
 * LEAVE;
 */


using std::cout;
using std::endl;
using std::string;
using std::vector;

extern PerlInterpreter *my_perl;

string Wrapper::capitalize(const string &txt){
	// Inicializando a pilha e criando vars. temp.
	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(SP);

	//mod_txt = capitalize(txt)
	//chamando a funcao capitalize()
	
	//push txt
	XPUSHs(sv_2mortal(newSVpv(txt.c_str(), txt.size())));
	
	PUTBACK;
	call_pv("capitalize", G_SCALAR);
	SPAGAIN;

	string output = POPp;

	PUTBACK;
	FREETMPS;
	LEAVE;
	return output;
}

std::vector<int> Wrapper::textCount(const std::string &txt){
	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(SP);

	//@($chars, $words, $nwsp_chars, $lines) = textCount(txt)
	//chamando a funcao textCount()
	
	//push txt
	XPUSHs(sv_2mortal(newSVpv(txt.c_str(), txt.size())));
	
	PUTBACK;
	int count = call_pv("textCount", G_ARRAY);
	SPAGAIN;

	std::vector<int> output(4);

	int i = count;
	while(i > 0){
		output[--i] = POPi;
	}

	PUTBACK;
	FREETMPS;
	LEAVE;

	return output;
}

string Wrapper::replace(const string &txt, const string &old_str, const string &new_str, int& count, const int &idx){
	// Inicializando a pilha e criando vars. temp.
	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(SP);

	//mod_txt = capitalize(txt)
	//chamando a funcao capitalize()
	
	//push txt
	XPUSHs(sv_2mortal(newSVpv(txt.c_str(), txt.size())));
	//push old_str
	XPUSHs(sv_2mortal(newSVpv(old_str.c_str(), old_str.size())));
	//push new_str
	XPUSHs(sv_2mortal(newSVpv(new_str.c_str(), new_str.size())));
	string output;
	if (idx == -1){
		//replace all
		PUTBACK;
		call_pv("replaceAll", G_ARRAY);
		SPAGAIN;
		output = POPp;
		count = POPi; // 0 if not replaced
	}
	else{
		//replace next
		XPUSHs(sv_2mortal(newSViv(idx)));
		PUTBACK;
		call_pv("replaceNext", G_ARRAY);
		SPAGAIN;
		output = POPp;
		count = POPi; // 0 if ok, 1 if error
	}
	
	PUTBACK;
	FREETMPS;
	LEAVE;
	return output;
}

vector<int> Wrapper::find(const string &txt, const string &s, const int &idx){
	// Inicializando a pilha e criando vars. temp.
	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(SP);

	//mod_txt = capitalize(txt)
	//chamando a funcao capitalize()
	
	//push txt
	XPUSHs(sv_2mortal(newSVpv(txt.c_str(), txt.size())));
	//push s
	XPUSHs(sv_2mortal(newSVpv(s.c_str(), s.size())));

	vector<int> output;
	if(idx == -1){
		//push idx
		PUTBACK;
		int count = call_pv("findAll", G_ARRAY);
		SPAGAIN;
		output = vector<int>(count);
		while(count > 0){
			output[--count] = POPi;
		}
	}
	else{
		XPUSHs(sv_2mortal(newSViv(idx)));
		PUTBACK;
		call_pv("findNext", G_SCALAR);
		SPAGAIN;
		output.push_back(POPi);
	}

	PUTBACK;
	FREETMPS;
	LEAVE;
	return output;
}

int Wrapper::matchCharacters(const string &txt, const char &c){
	// Inicializando a pilha e criando vars. temp.
	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(SP);

	//mod_txt = capitalize(txt)
	//chamando a funcao capitalize()
	
	//push txt
	XPUSHs(sv_2mortal(newSVpv(txt.c_str(), txt.size())));

	//push c
    char str[] = " ";
	str[0] = c;
	XPUSHs(sv_2mortal(newSVpv(str, 1)));
	
	PUTBACK;
	call_pv("matchCharacters", G_ARRAY);
	SPAGAIN;

	int idx = POPi; 
    POPp; // matched char


	PUTBACK;
	FREETMPS;
	LEAVE;
	return idx;
}

vector<string> Wrapper::autocomplete(const string &txt, const string &word){
    // Inicializando a pilha e criando vars. temp.
    dSP;
    ENTER;
    SAVETMPS;
    PUSHMARK(SP);



    //@words = autocomplete($txt, $word, $dictionary)

    //push txt
    XPUSHs(sv_2mortal(newSVpv(txt.c_str(), txt.size())));
    //push word
    XPUSHs(sv_2mortal(newSVpv(word.c_str(), word.size())));
    char empty[] = "";
    //push empty_dict
    XPUSHs(sv_2mortal(newSVpv(empty, 0)));
    PUTBACK;
    int count = call_pv("autocomplete", G_ARRAY);
    SPAGAIN;

    vector<string> output(count);
    while(count > 0){
        output[--count] = POPp;
    }

    PUTBACK;
    FREETMPS;
    LEAVE;
    return output;
}

