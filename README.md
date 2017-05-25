# TextEditor
Vim-like text editor made with ncurses

Install:
- Clone dir to local machine and run the command 'make' (or 'make all')
- Run the executable TextEditor; press h for help

Este editor foi escrito por Paulo Valente

Instalação:
- Clonar o diretório para a máquina local e executar o comando 'make' (ou 'make all')
- Executar o arquivo TextEditor; pressione h para ajuda

Parte da estrutura da interface grafica foi inspirada no tutorial em:
http://cheukyin699.github.io/tutorial/c++/2015/02/01/ncurses-editor-tutorial-01.html

Funcionalidades:
O editor, como o Vim, possui um modo 'normal' para insercao de comandos, e outros modos que executam suas funcionalidades.
A todo instante, o editor verifica balanceamento de caracteres (), [] e {}.

O programa aceita um nome de arquivo como unico argumento de linha de comando

Dependencias:
O programa foi feito com a versao 5.18 do Perl para a interface de processamento de texto

Comandos:
- *x* Sair (nao salva alteracoes no arquivo)
- *d* Modo de remocao de linhas, basta apertar d novamente para remover uma dada linha
- *i* Modo de insercao de texto
- *w* Salvar alteracoes no arquivo
- *:* Modo de insercao de comandos, atualmente contem:
	- */word*                    encontrar proximo
	- */word/g*                  encontrar todos
	- *s/old/new*                substituir proximo na linha
	- *s/old/new/g*              substituir todos na linha
	- *ss/old/new*							 substituir proximo
	- *ss/old/new/g*						 substituir todos
  - *w [filename]*             salvar arquivo, se nao for fornecido o nome, salva o arquivo atual
  - *q*                        sair
  - *wq*                       salvar e sair
  - *x*                        salvar e sair
- *h* Ajuda
- *c* Autocapitalizar frases
