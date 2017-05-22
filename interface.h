
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
	public:
		Interface(string);
		void init();
		int mainLoop();
		void setEditor(Editor*);
		void moveTo(int, int);

		void autocomplete(vector<string>, string);
		void refreshscr();


	private:
		string filename;
		Editor *editor;

		void printBuffer(vector<string> &);
		void printStatus(string &);

		void matchCharacters();
};

#endif
