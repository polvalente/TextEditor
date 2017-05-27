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
		vector<string> helpText;
		string status, filename;
		string commandBuffer;
		size_t bufferIndex;
		string z_mode_output;

		Buffer *buffer;
		Interface *interface;
		Wrapper *wrapper;

		void setupHelp();

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

		//funcoes com interface para o Perl
		bool autocomplete(string);
		void capitalize();
		string textCount();
		vector<int> convertIdxToRowCol(int, const string&);
		string getBufferTxt();

		vector<string> split(const string, const char=' ');

		template<class T>
		string join(vector<T>, string);

		size_t rowColToIdx();


		bool saveCommand();
		bool quitCommand();
		bool findReplace();
		bool openCommand();


	public:
		Editor();
		Editor(Interface*, Wrapper*);
		Editor(Interface*, Wrapper*, string filename);
		~Editor();

		char getMode();
		string numToStr(int);

		void input(int); // handle input
		const vector<string>& getBuffer(); // return buffer to print
		const string& getStatus(); // return status line to print
		void setStatus(string);
		void updateStatus(); // update status text
		vector<int> matchCharacters();
		string getMsg();

		int getX();
		int getY();

		bool bufferEmpty();
};

#endif
