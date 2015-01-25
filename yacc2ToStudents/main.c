#include <stdio.h>
#include "Semantics.h"
#include "CodeGen.h"
#include "SymTab.h"
#include "IOMngr.h"

extern int yyparse();

struct SymTab *table;
struct SymTab *argumentTable;
struct SymEntry *entry;
//int * variables;
FILE *aFile;

int main(int argc, char * argv[]) {
	table = CreateSymTab(33);
	argumentTable = CreateSymTab(33);
//	variables = (int*)malloc(10*sizeof(int));
	OpenFiles(argv[1], argv[2]);
	if (argc == 4) 
		aFile = fopen(argv[3], "w");
	else
		aFile = stdout;

	yyparse();
}

