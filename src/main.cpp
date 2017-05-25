#include <vector>
#include <string>
#include <iostream>

#include <EXTERN.h>
#include <perl.h>

#include "interface.h"
#include "wrapper.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

//static void xs_init(pTHX);

EXTERN_C void boot_DynaLoader(pTHX_ CV *cv);

EXTERN_C void xs_init(pTHX){
    string s = __FILE__;
    char *file = (char*) malloc(sizeof(char) * (s.size() + 1));
    strcpy(file, s.c_str());
    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}

PerlInterpreter *my_perl;

int main(int argc, char *argv[], char **env)
{

		string filename;
		if (argc == 2) {
			filename = string(argv[1]);
		}
		else if (argc == 1) {
			filename = "";
		}
		else {
			cout << "Usage: " << argv[0] << " [filename]" << endl;
			return 1;
		}
		
	
    //char *args[] = {NULL};
		char myArgv1[] = "./text.pl";
		char myArgv0[] = "";
    char *my_argv[] = {myArgv0, myArgv1};
    int my_argc = sizeof(my_argv)/sizeof(char*);

    PERL_SYS_INIT3(&argc, &argv, &env);
    my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;

    perl_parse(my_perl, xs_init, my_argc, my_argv, NULL);

    Wrapper *wrapper = new Wrapper;
		Interface interface(filename);

		Editor *editor = new Editor(&interface, wrapper, filename);
		interface.setEditor(editor);

		interface.init();
		interface.mainLoop();

		delete editor;
		delete wrapper;
		return 0;
}
