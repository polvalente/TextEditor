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

Editor::Editor(Interface *i, Wrapper *w) : 
	//Inicializando posicao do cursor
	x(0),
	y(0),
	//Nao foi recebido um nome de arquivo, abre em modo de ajuda
	mode('h'),
	//mensagem relacionada ao modo 'h'
	status("Ajuda - pressione q para sair"),
	//Nao foi recebido nome de arquivo
	filename(""),
	//Buffer de comando vazio
	commandBuffer(""),
	bufferIndex(0),
	//mensagem do modo auxiliar vazia
	z_mode_output(""),
	interface(i),
	wrapper(w){
	//Construtor nao-padrao
	buffer = new Buffer(); // Alocando um novo buffer no heap
	buffer->appendLine(""); // Linha vazia para evitar acessos indevidos em vector vazio
	setupHelp(); // leitura do arquivo de ajuda	
}

void Editor::setupHelp(){
	//Metodo que le o arquivo de ajuda e guarda seu conteudo no atributo "helpText"
	string helpFilename="./lib/help.txt"; // localizacao padrao do arquivo de ajuda
	std::ifstream arquivo(helpFilename.c_str()); // abrindo arquivo
	if(arquivo.is_open()){ // foi possivel abrir
		while(!arquivo.eof()){ // iterando ao longo do arquivo
			string line;
			getline(arquivo, line); // lendo a linha atual
			helpText.push_back(line); // guardando a linha no final do vector
		}
		arquivo.close(); // fechando arquivo
	}
}

Editor::Editor(Interface *i, Wrapper *w, string f) : 
	//Inicializando posicao do cursor
	x(0),
	y(0),
	//Foi recebido um nome de arquivo, abre em modo normal
	mode('n'),
	//mensagem relacionada ao modo 'n'
	status("Modo normal"),
	//nome do arquivo recebido
	filename(f),
	//buffer de comando vazio
	commandBuffer(""),
	bufferIndex(0),
	//mensagem do modo auxiliar vazia
	z_mode_output(""),
	interface(i),
	wrapper(w)
{
	//Construtor nao-padrao
	
	setupHelp(); // leitura do arquivo de ajuda

	buffer = new Buffer(); // alocacao de buffer

	//lendo conteudo do arquivo pro buffer
	std::ifstream arquivo(filename.c_str());
	if(arquivo.is_open()){
		while(!arquivo.eof()){ // iterando ao longo do arquivo
			string line;
			getline(arquivo, line); // lendo a linha atual
			buffer->appendLine(line); // guardando a linha lida no final do buffer
		}
		msg = "Arquivo aberto: "+filename; // mensagem complementar de status
		arquivo.close(); // fechando arquivo
	}
	else {
		//std::cerr << "Erro ao abrir arquivo: '" << filename << "'\n";
		buffer->appendLine(""); // linha vazia para evitar acessos indevidos ao buffer
		msg = "Novo buffer vazio"; // buffer de criacao de arquivo novo
		mode = 'h'; // modo de ajuda
	}
}

void Editor::updateStatus(){
	//Metodo para configuracao do status
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
			status+= commandBuffer; // configura a mensagem com o conteudo do buffer de comando para emular a digitacao da palavra
			msg = "";
			break;
		case 'd':
			status = "Modo de Remocao";
			break;
		case 'h':
			status = "Ajuda - pressione q para sair desta tela";
			break;
		default:
			break;
	}
	if (mode != ':' && mode != 'z' && mode != 'h'){
		status += "\tCOLUNA: " + numToStr(x) + "\tLINHA: " + numToStr(y) + " - " +  textCount() + " " + msg; // configura a mensagem complementar com informacoes do editor
	}
}

string Editor::numToStr(int num){
	//Conversao de int para string
	std::ostringstream ss;
	ss.clear();
	ss << num;
	return ss.str();
}

Editor::~Editor(){
	// apaga o buffer alocado dinamicamente
	delete buffer;
}

void Editor::del_key(){
	//encapsulamento da funcao da tecla del
	if (x == (int)buffer->lines->at(y).length() && y != (int)buffer->lines->size() - 1){
		// se estiver no final da linha, apaga o '\n' e faz a linha de baixo voltar para o final da atual
		buffer->lines->at(y) += buffer->lines->at(y+1);
		deleteLine(y+1);
	}
	else{
		//apaga o caracter abaixo do cursor
		buffer->lines->at(y).erase(x, 1);
	}
}

void Editor::input(int ch){
	//lida com entrada do teclado
	string word;
	switch(ch){
		//navega na tela de acordo com as 4 teclas direcionais
		case KEY_LEFT:
			left();
			if (mode != ':' && mode != 'z')
				return;
		case KEY_RIGHT:
			right();
			if (mode != ':' && mode != 'z')
				return;
		case KEY_UP:
			up();
			if (mode != ':' && mode != 'z')
				return;
		case KEY_DOWN:
			down();
			if (mode != ':' && mode != 'z')
				return;
		default:
				break;
	}

	if (mode == 'n'){
		//modo normal
			switch(ch){
			case 'x': // comando para sair do editor
				mode = 'x';
				break;
			case 'd': // modo de deletar linha para emular o comando 'dd' do Vim
				mode = 'd';
				break;
			case 'o': // insere linha abaixo do cursor e entra em modo de insercao com o cursor na nova linha
				mode = 'i';
				buffer->insertLine("",y+1); 
				y++;
				x = 0;
				break;
			case 'O': // insere linha acima da linha atual e entra em modo de insercao com o cursor na nova linha
				mode = 'i';
				buffer->insertLine("",y);
				x = 0;
				break;
			case 'i': // entrar no modo de insercao
				mode = 'i';
				break;
			case 'w': // salvar buffer atual
				save();
				break;
			case ':':
				mode = ':'; // entrar no modo comandos do tipo ":comando"
				commandBuffer = "";
				break;
			case 'h': // exibir tela de ajuda
				mode = 'h';
				break;
			case 'c':
			case 'C': // capitalizar frases no buffer atual
				capitalize();
				break;
			case KEY_DC: // tecla DEL
				del_key();
				break;
			default:
				break;
			}
	}
	else if (mode == 'h'){
		// modo de ajuda
		switch(ch){
			case 'q':
			case 'Q':
			case MY_KEY_ESC: // sair do modo de ajuda
				mode = 'n';
				break;
			default:
				break;
		}
	}
	else if (mode == 'd'){
		//modo para emular do comando 'dd'
		switch(ch){
			case 'd': // tecla 'd', apaga a linha atual e corrige a posicao do cursor de acordo
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
				mode = 'n'; // tecla invalida, sai do modo intermediario
				break;
		}
	}
	else if (mode == 'i'){
		//modo de insercao de conteudo
		switch(ch){
			case MY_KEY_ESC: // voltar para o modo normal
				mode = 'n';
				break;
			
			case MY_KEY_BACKSPC:
			case KEY_BACKSPACE: // apagar o caracter anterior ao cursor
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
			
			case KEY_DC: // tecla DEL
				del_key();
				break;
			
			case KEY_ENTER:
			case '\n': // pular linha e deslocar tudo apos o cursor pra linha seguinte
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
				if (x == 0 || word == "" || !autocomplete(word)){ // se nao estiver na primeira coluna e tiver uma palavra antes do cursor, tenta autocompletar
					//Nao autocompletou ou esta na primeira coluna. Inserir tab
					buffer->lines->at(y).insert(x, 4, ' ');
					x += 4;
				}
				break;

			default:
				//outros caracteres
				buffer->lines->at(y).insert(x, 1, static_cast<char>(ch)); // inserir caracter no buffer
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
				setStatus(""); // volta para o modo normal
				break;
			case 'i': // entra no modo de insercao
				mode = 'i';
				setStatus("");
				break;
			case ':': // entra no modo de comando
				mode = ':';
				break;
			default:
				break;
		}
		
	}
	else if (mode == ':'){
		// modo de comando
		switch(ch){
			case KEY_LEFT:
				bufferIndex = (bufferIndex > 0) ? bufferIndex - 1 : 0;
				break;
			case KEY_RIGHT:
				bufferIndex = (bufferIndex < commandBuffer.length()) ? bufferIndex + 1 : commandBuffer.length();
				break;

			case MY_KEY_BACKSPC:
			case KEY_BACKSPACE: // backspace
				if (commandBuffer.size() > 0)
					commandBuffer.erase(--bufferIndex, 1);
				break;

			case KEY_DC: // del
				if (bufferIndex < commandBuffer.length())
					commandBuffer.erase(bufferIndex, 1);
				break;

			case KEY_ENTER:
			case '\n':
				//quando recebe um "enter", tenta parsear o comando para ver se eh um comando valido
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

			case MY_KEY_ESC: // ESC - voltar para o modo normal
				mode = 'n';
				commandBuffer = "";
				bufferIndex = 0;
				return;
				break;
			default: // inserir caracter no buffer de comando
				if (bufferIndex > commandBuffer.length())
					bufferIndex = commandBuffer.length();
				commandBuffer.insert(bufferIndex++, 1, static_cast<char>(ch));
				msg = commandBuffer;
				break;
		}//end switch
	}
}

void Editor::left(){ // andar para a esquerda
	msg = "";
	if (x-1 >= 0){
		interface->moveTo(y, --x);
	}
}

void Editor::right(){ // andar para a direita
	msg = "";
	if (x + 1 < COLS && x + 1 <= (int)buffer->lines->at(y).length()){
		interface->moveTo(y, ++x);
	}
}

void Editor::up(){ // andar para cima
	msg = "";
	if (y-1 >= 0){
		y--;
	}
	if(x >= (int)buffer->lines->at(y).length()){
		x = buffer->lines->at(y).length();
	}
	interface->moveTo(y, x);
}

void Editor::down(){ // andar para baixo
	msg = "";
	if(y+1 < (int)LINES-1 && y+1 < (int) buffer->lines->size()){
		y++;
	}
	if((unsigned)x >= buffer->lines->at(y).length()){
		x = buffer->lines->at(y).length();
	}
	interface->moveTo(y, x);
}

const vector<string>& Editor::getBuffer(){ // retornar referencia para o buffer a imprimir na tela
	if (mode == 'h')
		return helpText;
	return *(buffer->lines);
}

const string& Editor::getStatus(){ // retornar referencia para a barra de status
	return status; 
}

void Editor::deleteLine(){ // apagar linha atual
	buffer->removeLine(y);
}

void Editor::deleteLine(int i){ // apagar linha especifica
	buffer->removeLine(i);
}

void Editor::save(){ // salvar arquivo atual
	if(filename == ""){ // se nao tiver nome de arquivo, mostra mensagem de erro
		msg = "Escolha um nome de arquivo com ':w <nome>'";
		return;
	}

	std::ofstream arquivo(filename.c_str()); // abrir arquivo para escrita
	if(arquivo.is_open()){ // se abriu, editar
		for(unsigned i=0; i < buffer->lines->size(); i++){ // imprimir cada linha do buffer
				arquivo << buffer->lines->at(i);
				if(i != buffer->lines->size()-1) // nao imprime endl se for a ultima linha
					arquivo << std::endl;
		}
		msg = "Arquivo '"+filename+"' salvo!";
	}
	else{
		msg = "Erro ao abrir o arquivo '"+filename+"'.";
	}
	arquivo.close();
}

int Editor::getX(){
	if (mode == ':'){ // no modo :, retorna a posicao do cursor no buffer de comando
		return bufferIndex+1;
	}
	return x; // retorna a posicao do cursor na tela
}

int Editor::getY(){
	if (mode == ':'){
		return LINES-1; // retorna a linha do buffer de comando
	}
	return y; // retorna a posicao do cursor na tela
}

char Editor::getMode(){ // retorna o modo de operacao atual
	return mode;
}

string Editor::getWordBeforeCursor(){ // retorna a palavra que precede o cursor
	string line = buffer->lines->at(y); // analisa a linha atual
	string word = ""; // palavra a principio nao existe
	if(line == ""){
		word = ""; // nao ha palavra se a linha esta vazia
	}
	else{
		std::stringstream ss(line.substr(0,x)); // string stream com substring que precede o cursor, funciona para dar split em espacos
		if (x == 0 || x > (int) line.length() || !isWordCharacter(line[x-1])){
			word = "";
		}
		else{
			while(ss >> word); // percorre o stringstream, para colocar a ultima palavra do stream em word
		}
	}
	return word;
}

bool Editor::isWordCharacter(char c){ // characters valid for a word
	return std::isalnum(static_cast<int>(c)) || (c == '_') || (c == '-');
}

bool Editor::autocomplete(string word){
	string txt = getBufferTxt();
	//invoca a interface com o perl para obter uma lista de palavras para autocompletar a palavra recebida
	vector<string> wordList = wrapper->autocomplete(txt, word);
	//invoca a interface grafica para mostrar a lista de palavras recebida e completar a palavra se necessario
	return interface->autocomplete(wordList, word);
}

void Editor::setStatus(string msg){
	//setter para modificar a mensagem complementar de status e atualizar o status atual
	this->msg = msg;
	updateStatus(); 
}


vector<int> Editor::matchCharacters(){
	//balanceamento de parentesis, colchetes e chaves
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
		if (idx != -1) // se deu problema, para de verificar
			break;
	}

	if (idx == -1){ // nao deu problema
		coords = vector<int>(2,-1);
		return coords;
	}
	
	coords = convertIdxToRowCol(idx, txt); // deu problema, converte indice linear pra (linha, coluna) e retorna a posicao
	return coords;
}

vector<int> Editor::convertIdxToRowCol(int idx, const string& txt){
	//converte indice linear para indice (linha, coluna)
	vector<int> out(2);
	int row = 0;
	int col = 0;
	for (int i = 1; i <= idx; i++){ // comeca no 1 para caso o indice seja 0, ele retorne (0,0)
		if (txt[i-1] == '\n'){ // cada \n encontrado incrementa uma linha e zera a coluna
			col = 0;
			row++;
		}
		else{ // outro caracter qualquer incrementa a coluna
			col++;
		}
	}
	out[0] = row;
	out[1] = col;
	return out;
}

string Editor::getBufferTxt(){ // retorna uma string contendo o conteudo do buffer
	vector<string> &lines = *(buffer->lines);
	return join(lines,"\n");
}

void Editor::capitalize(){
	//Capitaliza todas as frases do texto
	string txt = wrapper->capitalize(getBufferTxt());
	std::istringstream ss(txt);
	string line;
	if (txt.length() < 1)
		return;
	buffer->deleteContent(); // apaga o conteudo do buffer e substitui pelas linhas do string stream com o conteudo modificado
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

	string token;
	vector<string> comando = split(commandBuffer, '/'); // quebra a string do commandBuffer nos argumentos para parsear 
	
	if(comando.size() < 2 || comando.size() > 4 || (comando[0] != "" && comando[0] != "s" && comando[0] != "\%s")){
		//caso nao esteja no formato correto de comando, retorna false
		return false;
	}

	if (comando[comando.size()-1] == "g") // verifica se quer todas as ocorrencias (na linha, no caso do replace, ou no arquivo, no caso do find)
		all = true;

	string &word = comando[1];
	vector<int> findResult;
	if (comando[0] == ""){ // nada antes da primeira barra -> find
		string txt = getBufferTxt();	
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
	else if (comando[0] == "s" || comando[0] == "\%s"){ // s ou %s antes da primeira barra -> replace
		//replace
		string &new_word = comando[2];
		if (comando.size() == 3 && comando[2] == "g"){
			all = false;
		}
		int count = 0;
		string tmp;
		size_t lower, upper;
		if (comando[0] == "s"){
			//replace one line
			lower = y;
			upper = y + 1;
		}
		else{
			//replace all lines
			lower = 0;
			upper = buffer->lines->size();
		}
		int pos;
		if(all){
			//replace all occurences in line
			pos = -1;
		}
		else{
			//replace first occurence in line
			pos = 0;
		}
		for(size_t index = lower; index < upper; index++){ // percorre as linhas e executa o replace em cada uma
			string current_line = buffer->lines->at(index);
			if (current_line.length() == 0)
				continue;
			tmp = wrapper->replace(current_line, word, new_word, count, pos);
			if (count == 1)
				continue;
			vector<string> replaceResult = split(tmp, '\n');
			buffer->removeLine(index);
			buffer->insertLine(replaceResult[0], index);
		}
		return true;
	}
	else{
		return false; //nao deve acontecer
	}
	interface->refreshscr(); // atualiza a tela da interface grafica
	return true;
}

size_t Editor::rowColToIdx(){ // converte indices (linha, coluna) para indices lineares
	size_t out = 0;
	for(int r = 0; r < y; r++){
		out += buffer->lines->at(r).length()+1; // para cada linha anterior a atual, soma seu comprimento + 1 para o '\n'
	}
	out += x + 1; // adiciona o numero de caracteres que antecede o cursor + 1
	return out;
}

template <class T>
string Editor::join(vector<T> v, string sep){ // une os elementos do vector recebido em uma string utilizando a string separadadora ('sep') especificada
	if (v.size() < 1)
		return "";
	std::ostringstream oss("");
	size_t i = 0;
	for(i = 0; i < v.size() - 1; i++){
		oss << v[i] << sep; // para cada elemento ate o penultimo, append no oss do elemento e do separador
	}
	oss << v[i]; // append do ultimo elemento sem separador
	return oss.str(); // conversao de oss para string
}

vector<string> Editor::split(const string s, const char sep){ // separa uma string em um vector de strings de acordo com o char separador recebido
	vector<string> output;
	std::istringstream ss(s);
	string tok;
	while(std::getline(ss, tok, sep)){ // faz uso do getline com separador especificado, em cima do input string stream que contem o texto recebido
		output.push_back(tok); // append no vector com o trecho recebido
	}
	return output; // retorna o vector de split
}

string Editor::getMsg(){ // getter da mensagem complementar de status atual
	return msg;
}

string Editor::textCount(){ // invoca a analise de contagem de texto do perl
	std::ostringstream ss;
	string txt = getBufferTxt();
	vector<int> result(4);
	if (txt.length() > 0){
		result = wrapper->textCount(getBufferTxt());
	}
	else{
		result = vector<int>(4,0); // se o texto esta vazio, a contagem e nula
	}
	ss << "chrs: " << result[0] << " words: " << result[1] << " nwspc chrs: " << result[2] << " lines: " << result[3]; // construindo a string formatada em um stringstream
	return ss.str(); // convertendo stringstream para string
}

bool Editor::saveCommand(){ // parser de comando para salvar :w
	vector<string> comando = split(commandBuffer);
	if (comando.size() > 2 || comando.size() < 1|| comando[0] != "w"){
		return false;
	}
	else if (comando.size() == 1) {
		comando.push_back(filename); // se nao recebeu nome de arquivo, usa o nome de arquivo atual
	}

	string old_filename = filename;
	filename = comando[1]; // modifica temporariamente o filename com o segundo argumento em comando
	save(); // invoca a funcao save padrao
	filename = old_filename; // desfaz a modificacao no nome do arquivo

	return true;
}

bool Editor::quitCommand(){ // parser para o comando de saida :q, :x ou :wq
	string char0 = "";
	string char1 = "";

	if (commandBuffer.length() < 1){ // nao eh um comando valido
		return false;
	}
	else if (commandBuffer.length() == 2){ // se tiver dois caracteres, char1 recebe o segundo caracter
		char1 = commandBuffer[1];		
	}
	else if (commandBuffer.length() > 2){ // se tiver mais que dois caracteres, nao eh um comando valido
		return false;
	}

	char0 = commandBuffer[0]; // char0 recebe o primeiro caracter do comando

	if (char0 == "q" && commandBuffer.length() == 1){ // se char0 for q e nao tiver mais nenhum caracter, sai sem salvar
		//quit without save
		return true;
	}
	else if (char0 == "x" || (char0 == "w" && char1 == "q")){ // se char0 for x ou char0 e char1 forem w e q, salva e sai
		//save and quit
		save();
		return true;
	}
	return false;
}

bool Editor::openCommand(){ // parser do comando :e <filename>
	vector<string> comando = split(commandBuffer);
	if (comando[0] != "e" || comando.size() != 2) // nao segue formato valido
		return false;
	
	string fname = comando[1]; // nome do arquivo para abrir eh o segundo argumento

	std::ifstream arquivo(fname.c_str()); // abre o arquivo do nome especificado
	bool opened = false;
	if((opened = arquivo.is_open())){ // se abriu, reconstroi o buffer e a mensagem de status
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
	else { // nao abriu, entao eh um novo arquivo. reconstroi o buffer e o status de acordo
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
	//retorna verdadeiro se o buffer esta vazio
	return (buffer->lines->size() == 1 && buffer->lines->at(0) == "");
}	
