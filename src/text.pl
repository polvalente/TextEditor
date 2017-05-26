#!/usr/bin/env perl
#
#Programa de testes para as funcoes em Perl
#
#1  - parenMatch        - balanceamento de caracteres      - aplicada a todo o arquivo
#2  - capitalize        - capitalizar o texto              - aplicada a todo o arquivo
#3  - autocomplete      - completar uma palavra            - aplicada a ultima palavra do texto (no caso do programa de testes)
#4a - find/replace next - busca e substituicao             - consideram apenas as primeiras ocorrencias da palavra (no caso do programa de testes)
#4b - find/replace all  - busca e substituicao             - aplicadas a todo o arquivo
#5  - textCount         - contagens de parametros no texto - aplicada a todo o arquivo

no warnings;
use TextModule qw/ unique autocomplete matchCharacters findNext findAll replaceNext replaceAll textCount capitalize /;
use IO::Null;

my $null = IO::Null;
my $oldfh = select($null);

push(@INC,".");


#print(join(" ", findAll($s, "a")), "\n");
