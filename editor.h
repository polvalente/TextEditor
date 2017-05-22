#ifndef _EDITOR_H_
#define _EDITOR_H_


#include <string>
#include <vector>
using std::string;
using std::vector;

#include "buffer.h"
#include "wrapper.h"

#define MY_KEY_ESC 27
#define MY_KEY_BACKSPC 127

class Interface;

class Editor {
	private:
		int x, y;
		char mode;
		string msg;

		Buffer *buffer;
		Interface *interface;
		Wrapper *wrapper;
		string status, filename;
		string numToStr(int);

		void up();
		void down();
		void left();
		void right();

		void deleteLine(); // current line
		void deleteLine(int); // specific line
		void del_key();

		void save(); // save buffer to file
		bool isWordCharacter(char);
		string getWordBeforeCursor();
		string commandBuffer;
		size_t bufferIndex;

		//funcoes com interface para o Perl
		void autocomplete(string);
		void capitalize();
		string textCount();
		bool findReplace();
		vector<int> convertIdxToRowCol(int, const string&);
		string getBufferTxt();

		vector<string> split(const string, const char='\n');

		template<class T>
		string join(vector<T>, string);

		size_t rowColToIdx();

		string findreplace_output;


	public:
		Editor();
		Editor(Interface*, Wrapper*);
		Editor(Interface*, Wrapper*, string filename);
		~Editor();

		char getMode();

		void input(int); // handle input
		const vector<string>& getBuffer(); // return buffer to print
		const string& getStatus(); // return status line to print
		void setStatus(string);
		void updateStatus(); // update status text
		vector<int> matchCharacters();
		string getMsg();

		int getX();
		int getY();
};

#endif
