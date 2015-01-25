
/* Semantics.c
   Support and semantic action routines.
   
*/

#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include "CodeGen.h"
#include "Semantics.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "typeFile.h"

extern struct SymTab *table;
extern struct SymTab *argumentTable;
int nestedControl = 0;// for doprint function
int inmain = 1;
int localVariableNumber = 1; //this version can only have 1
char * localVariableList[10];
int pointerforthelist = 0;
int parameterpointer = 0;	//track on which parameter is now.
int argumentNumber = 0;		//keep track of argument number
int variables[10];
int variablesi = 0;

// corresponds to enum Operators
char * Ops[] = { "add", "sub", "mul", "div"};

/* Semantics support routines */

struct ExprRes *  doIntLit(char * digits)  { 

   struct ExprRes *res;
  
  //printf("doIntLit\n");
  res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
  res->Reg = AvailTmpReg();
  res->Instrs = GenInstr(NULL,"li",TmpRegName(res->Reg),digits,NULL);

  return res;
}


struct ExprRes *  doFloatLit(char * digits)  { 
	
	struct ExprRes *res;
	
	//printf("doFloatLit\n");
	res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
	res->Reg = AvailFTmpReg();
	res->Instrs = GenInstr(NULL,"li.s",TmpRegName(res->Reg),digits,NULL);
	
	return res;
}

struct ExprRes *  doBooleanLit(char * bl)  { 
	
	struct ExprRes *res;
	
	printf("doBLit\n");
	res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
	res->Reg = AvailBTmpReg();
	if (strcmp("true", bl) == 0) {
		res->Instrs = GenInstr(NULL,"li",TmpRegName(res->Reg),"1",NULL);
	}else{
		res->Instrs = GenInstr(NULL,"li",TmpRegName(res->Reg),"0",NULL);
	}
	return res;
}


struct ExprRes *  doRval(char * name)  { 

   struct ExprRes *res;
	struct SymEntry *temp;
	struct typeInfo *currentAttribute;
	printf("doRval\n");
   if (!(temp = FindName(table, name))) {
	   if (!(temp = FindName(argumentTable, name))) {
		   WriteIndicator(GetCurrentColumn());
		   WriteMessage("Undeclared variable");
	   }		
   }
	
	currentAttribute = GetAttr(temp);
	if ((currentAttribute -> type)==0) {
		printf("doRval1\n");
		res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
		res->Reg = AvailTmpReg();
		res->Instrs = GenInstr(NULL,"lw",TmpRegName(res->Reg),name,NULL);
	}else if ((currentAttribute -> type)==1) {
		printf("doRval2\n");
		res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
		res->Reg = AvailFTmpReg();
		res->Instrs = GenInstr(NULL,"lwc1",TmpRegName(res->Reg),name,NULL);
	}else if ((currentAttribute -> type)==2) {
		printf("doRval3\n");
		res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
		res->Reg = AvailBTmpReg();
		res->Instrs = GenInstr(NULL,"lw",TmpRegName(res->Reg),name,NULL);
	}else if ((currentAttribute -> type)==7) {
		printf("doRval6\n");
		res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
		res->Reg = AvailTmpReg();
		res->Instrs = GenInstr(NULL,"move",TmpRegName(res->Reg),currentAttribute -> Reg,NULL);
	}
  

  return res;
}

struct InstrSeq * doGeneralArgument(char * name){
	printf("doIntArgument\n");
	struct InstrSeq * instrs;
	int argumentReg = parameterpointer + 28;
	char *sppointer;
	struct typeInfo *argumentAttribute;
	struct SymEntry * argumentEntry;
	int arraypointer;
	
	sppointer = (char*) malloc (sizeof(char));
	argumentAttribute = (struct typeInfo*) malloc (sizeof(struct typeInfo));
	argumentEntry = (struct SymEntry*) malloc (sizeof(struct SymEntry));
	
	
	arraypointer = variablesi - 1;
	printf("::::::%d\n", variables[arraypointer]);
	sprintf(sppointer, "%d($sp)", (variables[arraypointer]-1) * 4);
	
	variables[arraypointer] = variables[arraypointer] - 1;
	parameterpointer ++;
	
	instrs = GenInstr(NULL, "lw", TmpRegName(argumentReg), sppointer, NULL);
	argumentAttribute -> Reg = TmpRegName(argumentReg);
	argumentAttribute -> type = 7;
	
	EnterName(argumentTable, name, &argumentEntry);
	SetAttr(argumentEntry, argumentAttribute);
	
	return instrs;
}

struct InstrSeq * doRefArgument(char * name){
	printf("doIntArgument\n");
	struct InstrSeq * instrs;
	int argumentReg = parameterpointer + 28;
	char *sppointer;
	struct typeInfo *argumentAttribute;
	struct SymEntry * argumentEntry;
	int arraypointer;
	
	sppointer = (char*) malloc (sizeof(char));
	argumentAttribute = (struct typeInfo*) malloc (sizeof(struct typeInfo));
	argumentEntry = (struct SymEntry*) malloc (sizeof(struct SymEntry));
	
	
	arraypointer = variablesi - 1;
	printf("::::::%d\n", variables[arraypointer]);
	sprintf(sppointer, "%d($sp)", (variables[arraypointer]-1) * 4);
	
	variables[arraypointer] = variables[arraypointer] - 1;
	parameterpointer ++;
	
	instrs = GenInstr(NULL, "lw", TmpRegName(argumentReg), sppointer, NULL);
	argumentAttribute -> Reg = TmpRegName(argumentReg);
	argumentAttribute -> type = 7;
	argumentAttribute -> localVariable = 2;
	
	EnterName(argumentTable, name, &argumentEntry);
	SetAttr(argumentEntry, argumentAttribute);
	
	return instrs;
}



struct InstrSeq * doLinkInstrs(struct InstrSeq *inst1, struct InstrSeq *inst2){
	struct InstrSeq * instrs;
	
	instrs = inst1;
	
	AppendSeq (instrs, inst2);
	
	return instrs;
}

struct ExprRes * doLinkExpr(struct ExprRes * Res1, struct ExprRes * Res2){
	struct ExprRes * res;
	
	res -> Instrs = Res1 -> Instrs;
	AppendSeq (res -> Instrs, Res2 -> Instrs);
	
	return res;
}

struct ExprRes *  doArrayRval(char * name, struct ExprRes * Res1)  {
	struct ExprRes *res;
	struct SymEntry *temp;
	struct typeInfo *currentAttribute;
	int addressReg;
	//int fourtimesindex = atoi(index);
	//fourtimesindex = 4 * fourtimesindex;
	//char * finalindex;
	//finalindex = (char *) malloc (sizeof(char));
	//sprintf (finalindex,"%d",fourtimesindex);
	addressReg = AvailTmpReg();
	//printf("doArrayRval%s::%s\n",name,index);
	if (!(temp = FindName(table, name))) {
		WriteIndicator(GetCurrentColumn());
		WriteMessage("Undeclared variable");
	}
	currentAttribute = GetAttr(temp);
	if ((currentAttribute -> type)==3 || (currentAttribute -> type)==5) {
		printf("doArrayRval1\n");
		res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
		res->Reg = AvailTmpReg();
		res->Instrs = GenInstr(NULL, "mul", TmpRegName(Res1->Reg),TmpRegName(Res1->Reg),"4");
		AppendSeq(res->Instrs, GenInstr(NULL,"la",TmpRegName(addressReg),name,NULL));
		AppendSeq(res->Instrs, GenInstr(NULL,"add",TmpRegName(addressReg),TmpRegName(addressReg),TmpRegName(Res1->Reg)));
		AppendSeq (res->Instrs, GenInstr(NULL, "lw", TmpRegName(res->Reg),indirectAddress(TmpRegName(addressReg)),NULL));
	}else if ((currentAttribute -> type)==4) {
		printf("doArrayRval1\n");
		res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
		res->Reg = AvailFTmpReg();
		res->Instrs = GenInstr(NULL, "mul", TmpRegName(Res1->Reg),TmpRegName(Res1->Reg),"4");
		AppendSeq(res->Instrs, GenInstr(NULL,"la",TmpRegName(addressReg),name,NULL));
		AppendSeq(res->Instrs, GenInstr(NULL,"add",TmpRegName(addressReg),TmpRegName(addressReg),TmpRegName(Res1->Reg)));
		AppendSeq (res->Instrs, GenInstr(NULL, "lwc1", TmpRegName(res->Reg),indirectAddress(TmpRegName(addressReg)),NULL));
	}
	
	ReleaseTmpReg(addressReg);
	return res;
	
}

char * doIdent(char * name) {
	printf("doIdent\n");
	if (!FindName(table, name)) {
		WriteIndicator(GetCurrentColumn());
		WriteMessage("Undeclared variable");
	}

	return strdup(name);
}

struct ExprResList *  doLink(struct ExprRes * Res1, struct ExprResList * Resls1)  {
	printf("doLink\n");
	struct ExprResList * ReslsLinked;
	
	ReslsLinked = (struct ExprResList *)malloc(sizeof(struct ExprResList));
	ReslsLinked -> Expr = Res1;
	ReslsLinked -> Next = Resls1;
	
	return ReslsLinked;
}

struct ExprRes *  doAdd(struct ExprRes * Res1, struct ExprRes * Res2)  { 
 printf("doAdd\n");
   int reg;
  if (Res1->Reg < 10 && Res2 -> Reg <10) {
	  printf("doAdd\n");
	  
	  reg = AvailTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"add",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doAdd2\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doAdd3\n");
  }else if (Res1->Reg >= 10 && Res2 -> Reg >=10) {
	  printf("doAdd4\n");
	  
	  reg = AvailFTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"add.s",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doAdd5\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doAdd6\n");
  }else if (Res1->Reg >= 10 && Res2 -> Reg < 10) {
	  int convertReg = AvailFTmpReg();
	  printf("doAdd7\n");
	  AppendSeq(Res2 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res2->Reg),TmpRegName(convertReg),NULL));
	  AppendSeq(Res2 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
	  ReleaseTmpReg(Res2 -> Reg);
	  Res2 -> Reg = convertReg;	  
	  reg = AvailFTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"add.s",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doAdd8\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doAdd9\n");
  }else {
	  int convertReg = AvailFTmpReg();
	  printf("doAdd10\n");
	  AppendSeq(Res1 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res1->Reg),TmpRegName(convertReg),NULL));
	  AppendSeq(Res1 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
	  ReleaseTmpReg(Res1 -> Reg);
	  Res1 -> Reg = convertReg;	  
	  reg = AvailFTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"add.s",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doAdd11\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doAdd12\n");
  }

  
  return Res1;
}

struct ExprRes *  doMinus(struct ExprRes * Res1, struct ExprRes * Res2)  { 
	
	int reg;
	if (Res1->Reg < 10 && Res2 -> Reg <10) {
	printf("doMinus\n");
	
	reg = AvailTmpReg();
	AppendSeq(Res1->Instrs,Res2->Instrs);
	AppendSeq(Res1->Instrs,GenInstr(NULL,"sub",
									TmpRegName(reg),
									TmpRegName(Res1->Reg),
									TmpRegName(Res2->Reg)));
	printf("doMinus2\n");
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	Res1->Reg = reg;
	free(Res2);
	printf("doMinus3\n");
	}else if (Res1->Reg >= 10 && Res2 -> Reg >=10) {
		printf("doMinus4\n");
		
		reg = AvailFTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"sub.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doMinus5\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doMinus6\n");
	}else if (Res1->Reg >= 10 && Res2 -> Reg < 10) {
		int convertReg = AvailFTmpReg();
		printf("doMinus7\n");
		AppendSeq(Res2 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res2->Reg),TmpRegName(convertReg),NULL));
		AppendSeq(Res2 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
		ReleaseTmpReg(Res2 -> Reg);
		Res2 -> Reg = convertReg;	  
		reg = AvailFTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"sub.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doMinus8\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doMinus9\n");
	}else {
		int convertReg = AvailFTmpReg();
		printf("doMinus10\n");
		AppendSeq(Res1 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res1->Reg),TmpRegName(convertReg),NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
		ReleaseTmpReg(Res1 -> Reg);
		Res1 -> Reg = convertReg;	  
		reg = AvailFTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"sub.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doMinus11\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doMinus12\n");
	}
	return Res1;
}

struct ExprRes *  doMult(struct ExprRes * Res1, struct ExprRes * Res2)  { 

   int reg;
  if (Res1->Reg < 10 && Res2 -> Reg <10) {
  printf("doMult\n");
 
  reg = AvailTmpReg();
  AppendSeq(Res1->Instrs,Res2->Instrs);
  AppendSeq(Res1->Instrs,GenInstr(NULL,"mul",
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg),
                                       TmpRegName(Res2->Reg)));
  printf("doMult2\n");
  ReleaseTmpReg(Res1->Reg);
  ReleaseTmpReg(Res2->Reg);
  Res1->Reg = reg;
  free(Res2);
  printf("doMult3\n");
  }else if (Res1->Reg >= 10 && Res2 -> Reg >=10) {
	  printf("doMult4\n");
	  
	  reg = AvailFTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"mul.s",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doMult5\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doMult6\n");
  }else if (Res1->Reg >= 10 && Res2 -> Reg < 10) {
	  int convertReg = AvailFTmpReg();
	  printf("doMult7\n");
	  AppendSeq(Res2 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res2->Reg),TmpRegName(convertReg),NULL));
	  AppendSeq(Res2 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
	  ReleaseTmpReg(Res2 -> Reg);
	  Res2 -> Reg = convertReg;	  
	  reg = AvailFTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"mul.s",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doMult8\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doMult9\n");
  }else {
	  int convertReg = AvailFTmpReg();
	  printf("doMult10\n");
	  AppendSeq(Res1 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res1->Reg),TmpRegName(convertReg),NULL));
	  AppendSeq(Res1 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
	  ReleaseTmpReg(Res1 -> Reg);
	  Res1 -> Reg = convertReg;	  
	  reg = AvailFTmpReg();
	  AppendSeq(Res1->Instrs,Res2->Instrs);
	  AppendSeq(Res1->Instrs,GenInstr(NULL,"mul.s",
									  TmpRegName(reg),
									  TmpRegName(Res1->Reg),
									  TmpRegName(Res2->Reg)));
	  printf("doMult11\n");
	  ReleaseTmpReg(Res1->Reg);
	  ReleaseTmpReg(Res2->Reg);
	  Res1->Reg = reg;
	  free(Res2);
	  printf("doMult12\n");
  }
  return Res1;
}

struct ExprRes *  doDiv(struct ExprRes * Res1, struct ExprRes * Res2)  { 
	
	int reg;
	if (Res1->Reg < 10 && Res2 -> Reg <10) {
		printf("doDiv\n");
	
	reg = AvailTmpReg();
	AppendSeq(Res1->Instrs,Res2->Instrs);
	AppendSeq(Res1->Instrs,GenInstr(NULL,"div",
									TmpRegName(reg),
									TmpRegName(Res1->Reg),
									TmpRegName(Res2->Reg)));
	printf("doDiv2\n");
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	Res1->Reg = reg;
	free(Res2);
	printf("doDiv3\n");
	}else if (Res1->Reg >= 10 && Res2 -> Reg >=10) {
		printf("doDiv4\n");
		
		reg = AvailFTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"div.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doDiv5\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doDiv6\n");
	}else if (Res1->Reg >= 10 && Res2 -> Reg < 10) {
		int convertReg = AvailFTmpReg();
		printf("doDiv7\n");
		AppendSeq(Res2 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res2->Reg),TmpRegName(convertReg),NULL));
		AppendSeq(Res2 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
		ReleaseTmpReg(Res2 -> Reg);
		Res2 -> Reg = convertReg;	  
		reg = AvailFTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"div.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doDiv8\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doDiv9\n");
	}else {
		int convertReg = AvailFTmpReg();
		printf("doDiv10\n");
		AppendSeq(Res1 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res1->Reg),TmpRegName(convertReg),NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
		ReleaseTmpReg(Res1 -> Reg);
		Res1 -> Reg = convertReg;	  
		reg = AvailFTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"div.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doDiv11\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doDiv12\n");
	}
	
	
	return Res1;
}

struct ExprRes *  doMod(struct ExprRes * Res1, struct ExprRes * Res2)  { 
	//warning: This function will not support negtive numbers, since that is MATH ERROR.
	int reg;
	if (Res1->Reg < 10 && Res2 -> Reg <10) {
		printf("doMod\n");
		
		reg = AvailTmpReg();
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"rem",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		printf("doMod2\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		Res1->Reg = reg;
		free(Res2);
		printf("doMod3\n");
	}else if (Res1->Reg >= 10 && Res2 -> Reg >=10) {
		printf("doMod4\n");
		int reg2;
		int reg3;
		int ingReg;
		
		printf("doMod5\n");
		
		reg = AvailFTmpReg();
		reg2 = AvailFTmpReg();
		reg3 = AvailFTmpReg();
		ingReg = AvailTmpReg();
		
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"div.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		
		//clear the number after point
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.w.s",
										TmpRegName(reg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mfc1",
										TmpRegName(ingReg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(ingReg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg),
										TmpRegName(reg),
										NULL));
		//End ingnore
		
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mul.s",
										TmpRegName(reg2),
										TmpRegName(reg),
										TmpRegName(Res2->Reg)));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"sub.s",
										TmpRegName(reg3),
										TmpRegName(Res1->Reg),
										TmpRegName(reg2)));
		printf("doMod6\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		ReleaseTmpReg(reg);
		ReleaseTmpReg(reg2);
		Res1->Reg = reg3;
		free(Res2);
		printf("doMod7\n");
	}else if (Res1->Reg >= 10 && Res2 -> Reg < 10) {
		int convertReg = AvailFTmpReg();
		printf("doMod8\n");
		AppendSeq(Res2 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res2->Reg),TmpRegName(convertReg),NULL));
		AppendSeq(Res2 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
		ReleaseTmpReg(Res2 -> Reg);
		Res2 -> Reg = convertReg;	  
		int reg2;
		int reg3;
		int ingReg;
		
		printf("doMod9\n");
		
		reg = AvailFTmpReg();
		reg2 = AvailFTmpReg();
		reg3 = AvailFTmpReg();
		ingReg = AvailTmpReg();
		
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"div.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		
		//clear the number after point
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.w.s",
										TmpRegName(reg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mfc1",
										TmpRegName(ingReg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(ingReg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg),
										TmpRegName(reg),
										NULL));
		//End ingnore
		
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mul.s",
										TmpRegName(reg2),
										TmpRegName(reg),
										TmpRegName(Res2->Reg)));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"sub.s",
										TmpRegName(reg3),
										TmpRegName(Res1->Reg),
										TmpRegName(reg2)));
		printf("doMod10\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		ReleaseTmpReg(reg);
		ReleaseTmpReg(reg2);
		Res1->Reg = reg3;
		free(Res2);
		printf("doMod11\n");
	}else {
		int convertReg = AvailFTmpReg();
		printf("doMod10\n");
		AppendSeq(Res1 -> Instrs, GenInstr(NULL,"mtc1",TmpRegName(Res1->Reg),TmpRegName(convertReg),NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL,"cvt.s.w",TmpRegName(convertReg),TmpRegName(convertReg),NULL));
		ReleaseTmpReg(Res1 -> Reg);
		Res1 -> Reg = convertReg;	  
		int reg2;
		int reg3;
		int ingReg;
		
		printf("doMod11\n");
		
		reg = AvailFTmpReg();
		reg2 = AvailFTmpReg();
		reg3 = AvailFTmpReg();
		ingReg = AvailTmpReg();
		
		AppendSeq(Res1->Instrs,Res2->Instrs);
		AppendSeq(Res1->Instrs,GenInstr(NULL,"div.s",
										TmpRegName(reg),
										TmpRegName(Res1->Reg),
										TmpRegName(Res2->Reg)));
		
		//clear the number after point
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.w.s",
										TmpRegName(reg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mfc1",
										TmpRegName(ingReg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(ingReg),
										TmpRegName(reg),
										NULL));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg),
										TmpRegName(reg),
										NULL));
		//End ingnore
		
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mul.s",
										TmpRegName(reg2),
										TmpRegName(reg),
										TmpRegName(Res2->Reg)));
		AppendSeq(Res1->Instrs,GenInstr(NULL,"sub.s",
										TmpRegName(reg3),
										TmpRegName(Res1->Reg),
										TmpRegName(reg2)));
		printf("doMod12\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		ReleaseTmpReg(reg);
		ReleaseTmpReg(reg2);
		Res1->Reg = reg3;
		free(Res2);
		printf("doMod13\n");
	}
	
	
	return Res1;
}

struct InstrSeq * doRead(char * name) {
	struct InstrSeq *code;
	struct SymEntry *temp;
	struct typeInfo *currentAttribute;
	
	printf("doRead\n");
	if (!(temp = FindName(table, name))) {
		WriteIndicator(GetCurrentColumn());
		WriteMessage("Undeclared variable");
	}
	
	code = (struct InstrSeq *)malloc(sizeof(struct InstrSeq));
	currentAttribute = GetAttr(temp);
	if ((currentAttribute -> type)==0 ) {
		printf("doRead1\n");
		AppendSeq (code, GenInstr(NULL,"li","$v0","4", NULL));
		AppendSeq (code, GenInstr(NULL,"la","$a0","IS", NULL));
		AppendSeq (code, GenInstr(NULL,"syscall",NULL,NULL, NULL));
		AppendSeq (code, GenInstr(NULL,"li","$v0","5", NULL));
		AppendSeq (code, GenInstr(NULL,"syscall",NULL,NULL, NULL));
		AppendSeq (code, GenInstr(NULL, "sw", "$v0", name,NULL));
	} else if ((currentAttribute -> type)==1) {
		printf("doRead2\n");
		AppendSeq (code, GenInstr(NULL,"li","$v0","4", NULL));
		AppendSeq (code, GenInstr(NULL,"la","$a0","FS", NULL));
		AppendSeq (code, GenInstr(NULL,"syscall",NULL,NULL, NULL));
		AppendSeq (code, GenInstr(NULL,"li","$v0","6", NULL));
		AppendSeq (code, GenInstr(NULL,"syscall",NULL,NULL, NULL));
		AppendSeq (code, GenInstr(NULL, "swc1", "$f0", name,NULL));
	} else if ((currentAttribute -> type)==2) {
		AppendSeq (code, GenInstr(NULL,"li","$v0","4", NULL));
		AppendSeq (code, GenInstr(NULL,"la","$a0","BS", NULL));
		AppendSeq (code, GenInstr(NULL,"syscall",NULL,NULL, NULL));
		AppendSeq (code, GenInstr(NULL,"li","$v0","5", NULL));
		AppendSeq (code, GenInstr(NULL,"syscall",NULL,NULL, NULL));
		AppendSeq (code, GenInstr(NULL, "sw", "$v0", name,NULL));
	}
	
	return code;
}

struct InstrSeq * doVoidFuctions(char * functionName, struct InstrSeq* argu, struct InstrSeq * instr1){
	printf("doVoidFunctions\n");
	struct InstrSeq *result;
	struct InstrSeq *returnresult;
	struct SymEntry * entry;
	int pointer = localVariableNumber * 4;
	char * sppointer;
	char * localVariableName;
	char * stringpointer;
	char * popup;
	int arraypointer;
	
	
	int saveReg = AvailSTmpReg();
	
	arraypointer = variablesi - 1;
	
	char * functionExitLabel;
	functionExitLabel = (char *)malloc (sizeof(char));
	popup = (char *)malloc (sizeof(char));
	
	sprintf(functionExitLabel, "%sExit", functionName);
	sprintf(popup,"%d",parameterpointer*4);
	
	result = GenInstr(functionName,NULL,NULL,NULL,NULL);
	AppendSeq(result, argu);
	AppendSeq(result,GenInstr(NULL,"addi","$sp","$sp","-4"));
	AppendSeq(result,GenInstr(NULL,"sw","$ra","0($sp)",NULL));
	returnresult = (struct InstrSeq *) malloc (sizeof(struct InstrSeq));
	
	AppendSeq (result, instr1);
	AppendSeq (result, GenInstr(functionExitLabel,NULL,NULL,NULL,NULL));
	
	localVariableNumber = pointerforthelist;
	
	printf("<#message#>%d",localVariableNumber);
	while (localVariableNumber > 0) {
		
		
		sppointer = (char *)malloc(sizeof(char));
		stringpointer =  (char *)malloc(sizeof(char));
		localVariableName = (char *)malloc(sizeof(char));
		
		sprintf(sppointer,"%d($sp)", (pointer-4));
		sprintf(stringpointer,"%d", pointer);
		
		AppendSeq (result, GenInstr(NULL,"lw",TmpRegName(saveReg),sppointer,NULL));
		AppendSeq (result, GenInstr(NULL,"sw",TmpRegName(saveReg),localVariableList[localVariableNumber-1],NULL));
		AppendSeq (result, GenInstr(NULL,"addi","$sp","$sp",stringpointer));
		
		localVariableNumber --;
	}
	
	AppendSeq (result, GenInstr(NULL,"lw","$ra","0($sp)",NULL));
	AppendSeq (result, GenInstr(NULL,"addi","$sp","$sp","4"));
	AppendSeq (result, GenInstr(NULL,"addi","$sp","$sp",popup));
	variablesi --;
	AppendSeq (result, GenInstr(NULL,"jr","$ra",NULL,NULL));
	
	struct typeInfo *attribute;
	EnterName(table, functionName, &entry); 
	attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
	attribute->type = 6;
	attribute -> instrs = result;
	SetAttr(entry, attribute); 
	
	return returnresult;
}

struct InstrSeq * doMainFuctions(struct InstrSeq * instr1){
	
	
	return instr1;
}
	

struct InstrSeq * callfc(char * functionName, struct ExprResList *exprls){
	struct InstrSeq *result;
	int argument1;
	struct ExprRes *Expression;
	char * sppointer;
	int pointer = 4;

    result = (struct InstrSeq *) malloc (sizeof(struct InstrSeq));	
	sppointer = (char *)malloc(sizeof(char));
	
	sprintf(sppointer,"%d($sp)", (pointer-4));
	while (exprls != NULL) {
		argumentNumber ++;
		argument1 = AvailPTmpReg();
		Expression = exprls -> Expr;
		AppendSeq(result,Expression -> Instrs);
		if (Expression -> Reg >= 10 && Expression -> Reg < 20) {
			AppendSeq(result, GenInstr(NULL, "mfc1", TmpRegName(argument1), TmpRegName(Expression -> Reg),NULL));
			AppendSeq(result, GenInstr(NULL, "addi", "$sp", "$sp","-4"));
			AppendSeq(result, GenInstr(NULL, "sw", TmpRegName(argument1), sppointer,NULL));
		}else {
			AppendSeq(result, GenInstr(NULL, "move", TmpRegName(argument1), TmpRegName(Expression -> Reg),NULL));
			AppendSeq(result, GenInstr(NULL, "addi", "$sp", "$sp","-4"));
			AppendSeq(result, GenInstr(NULL, "sw", TmpRegName(argument1), sppointer,NULL));
		}		
		exprls = exprls -> Next;
	}

	AppendSeq(result, GenInstr(NULL,"jal",functionName,NULL,NULL));
	//printf("theargumentnumber%d\n",argumentNumber);
	variables[variablesi] = argumentNumber;
	//printf("::::::::::::::::::::list%d\n",variables[1]);
	argumentNumber = 0;
	variablesi ++;
	printf("::::::::::::::::::::%d\n",variablesi);
	return result;
}

struct ExprRes *  doExpon(struct ExprRes * Res1, struct ExprRes * Res2)  {
	//This function can accept the negtive exponent, and also that is why integer^0 = 1.0 (try to improve!)
	
	if (Res2->Reg >= 10) {
		WriteIndicator(GetCurrentColumn());
		WriteMessage("Exponents can't be a float number. Math Error");
		return;//force the program to stop
	}
	
	int reg;
	int reg2;
	int reg3;
	int reg4;
	int reg5;
	int reg6;
	
	printf("doExpon\n");
	
	if (Res1 -> Reg < 10) {	
	
	reg = AvailTmpReg();//$t2
	reg2 = AvailTmpReg();//$t3
	reg3 = AvailTmpReg();//$t4
	reg4 = AvailFTmpReg();//$f5
	reg5 = AvailFTmpReg();//$f6
		reg6 = AvailTmpReg();//$t5
	
	AppendSeq(Res1->Instrs,Res2->Instrs);
	//li		$t4, 1	
	AppendSeq(Res1->Instrs,GenInstr(NULL,"li",
									TmpRegName(reg3),
									"1",
									NULL));
	//li		$t5, -1
		AppendSeq(Res1->Instrs,GenInstr(NULL,"li",
										TmpRegName(reg6),
										"-1",
										NULL));
		//move	$t3, $t0
		AppendSeq(Res1->Instrs, GenInstr(NULL, "move",
										 TmpRegName(reg2),
										 TmpRegName(Res1->Reg),
										 NULL));
		//slt     $t2, $t1, $0
		AppendSeq(Res1->Instrs,GenInstr(NULL,"slt",
										TmpRegName(reg),
										TmpRegName(Res2->Reg),
										"$0"));
		
	//beq     $t1, $0, ExpIsZero
	AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",
									 TmpRegName(Res2->Reg),
									 "$0",
									 "ExpIsZero"));
	
	
		//beq     $t1, $t4, ExpIsOneOrNegOne
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",
										 TmpRegName(Res2->Reg),
										 TmpRegName(reg3),
										 "ExpIsOneOrNegOne"));
		//beq     $t1, $t5, ExpIsOneOrNegOne
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",
										 TmpRegName(Res2->Reg),
										 TmpRegName(reg6),
										 "ExpIsOneOrNegOne"));
	//bne     $t2, $0, ReverseExpo
	AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",
									TmpRegName(reg),
									"$0",
									"ReverseExpo"));	
	//ExpLoopStart:
	AppendSeq(Res1->Instrs,GenInstr("ExpLoopStart",NULL,
									NULL,
									NULL,
									NULL));
    //mul     $t3, $t3, $t0
	AppendSeq(Res1->Instrs, GenInstr(NULL, "mul",
									TmpRegName(reg2),
									TmpRegName(reg2),
									TmpRegName(Res1->Reg)));
	//addi    $t1, $t1, -1
	AppendSeq(Res1->Instrs, GenInstr(NULL, "addi",
									TmpRegName(Res2->Reg),
									TmpRegName(Res2->Reg),
									"-1"));
	//bne     $t1, $t4, ExpLoopStart	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",
									TmpRegName(Res2->Reg),
									TmpRegName(reg3),
									"ExpLoopStart"));
		//ExpIsNeg:
		AppendSeq(Res1->Instrs,GenInstr("ExpIsOneOrNegOne",NULL,
										NULL,
										NULL,
										NULL));
		//mtc1    $t3, $f6
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(reg2),
										TmpRegName(reg5),
										NULL));
		//cvt.s.w $f6, $f6
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg5),
										TmpRegName(reg5),
										NULL));
	//bne     $t2, $0, ExpIsNeg	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",
									TmpRegName(reg),
									"$0",
									"ExpIsNeg"));
	//j		ExpLoopEnd
	AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
									"ExpLoopEnd",
									NULL,
									NULL));
	//ExpIsNeg:
	AppendSeq(Res1->Instrs,GenInstr("ExpIsNeg",NULL,
									NULL,
									NULL,
									NULL));
		//mtc1    $t4, $f5
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(reg3),
										TmpRegName(reg4),
										NULL));
		//cvt.s.w $f5, $f5
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg4),
										TmpRegName(reg4),
										NULL));
		//mtc1    $t3, $f6
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(reg2),
										TmpRegName(reg5),
										NULL));
		//cvt.s.w $f6, $f6
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg5),
										TmpRegName(reg5),
										NULL));
	//div.s     $f6, $f5, $f6
	AppendSeq(Res1->Instrs, GenInstr(NULL, "div.s",
									TmpRegName(reg5),
									TmpRegName(reg4),
									TmpRegName(reg5)));
	//j		ExpLoopEnd
	AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
									"ExpLoopEnd",
									NULL,
									NULL));
	//ExpIsZero:
	AppendSeq(Res1->Instrs,GenInstr("ExpIsZero",NULL,
									NULL,
									NULL,
									NULL));
		//li.s      $f3, 1.0
		AppendSeq(Res1->Instrs,GenInstr(NULL,"li.s",
										TmpRegName(reg5),
										"1.0",
										NULL));
	//j		ExpLoopEnd
	AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
									"ExpLoopEnd",
									NULL,
									NULL));
	//ReverseExpo:
	AppendSeq(Res1->Instrs,GenInstr("ReverseExpo",NULL,
									NULL,
									NULL,
									NULL));
	//sub		$t1, $0, $t1
	AppendSeq(Res1->Instrs, GenInstr(NULL, "sub",
									TmpRegName(Res2->Reg),
									"$0",
									TmpRegName(Res2->Reg)));
	//j		ExpLoopStart
	AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
									"ExpLoopStart",
									NULL,
									NULL));
	//ExpLoopEnd:
	AppendSeq(Res1->Instrs,GenInstr("ExpLoopEnd",NULL,
									NULL,
									NULL,
									NULL));
	printf("doExpon2\n");
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	ReleaseTmpReg(reg);
	ReleaseTmpReg(reg3);
	ReleaseTmpReg(reg4);
	ReleaseTmpReg(reg5);
	Res1->Reg = reg5;
	free(Res2);
	printf("doExpon3\n");
	}else {
		int reg4;
		int reg5;
		reg = AvailTmpReg();//$t2
		reg2 = AvailFTmpReg();//$f3
		reg3 = AvailTmpReg();//$t4
		reg4 = AvailFTmpReg();//$f5
		reg5 = AvailTmpReg();//$t6
		
		AppendSeq(Res1->Instrs,Res2->Instrs);
		//li		$t4, 1	
		AppendSeq(Res1->Instrs,GenInstr(NULL,"li",
										TmpRegName(reg3),
										"1",
										NULL));
		//li		$t6, -1	
		AppendSeq(Res1->Instrs,GenInstr(NULL,"li",
										TmpRegName(reg5),
										"-1",
										NULL));
		//beq     $t1, $0, ExpIsZero
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",
										 TmpRegName(Res2->Reg),
										 "$0",
										 "ExpIsZero"));
		//beq		$t4, $t0, ExpIsOne
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",
										 TmpRegName(reg3),
										 TmpRegName(Res2->Reg),
										 "ExpIsOne"));
		//beq		$t6, $t0, ExpIsOne
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",
										 TmpRegName(reg5),
										 TmpRegName(Res2->Reg),
										 "ExpIsNegOne"));
		//slt     $t2, $t1, $0
		AppendSeq(Res1->Instrs,GenInstr(NULL,"slt",
										TmpRegName(reg),
										TmpRegName(Res2->Reg),
										"$0"));
		//mov.s	$f3, $f0
		AppendSeq(Res1->Instrs, GenInstr(NULL, "mov.s",
										 TmpRegName(reg2),
										 TmpRegName(Res1->Reg),
										 NULL));
		//bne     $t2, $0, ReverseExpo
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",
										 TmpRegName(reg),
										 "$0",
										 "ReverseExpo"));	
		//ExpLoopStart:
		AppendSeq(Res1->Instrs,GenInstr("ExpLoopStart",NULL,
										NULL,
										NULL,
										NULL));
		//mul.s     $f3, $f3, $f0
		AppendSeq(Res1->Instrs, GenInstr(NULL, "mul.s",
										 TmpRegName(reg2),
										 TmpRegName(reg2),
										 TmpRegName(Res1->Reg)));
		//addi    $t1, $t1, -1
		AppendSeq(Res1->Instrs, GenInstr(NULL, "addi",
										 TmpRegName(Res2->Reg),
										 TmpRegName(Res2->Reg),
										 "-1"));
		//bne     $t1, $t4, ExpLoopStart	
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",
										 TmpRegName(Res2->Reg),
										 TmpRegName(reg3),
										 "ExpLoopStart"));
		//bne     $t2, $0, ExpIsNeg	
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",
										 TmpRegName(reg),
										 "$0",
										 "ExpIsNeg"));
		//j		ExpLoopEnd
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
										 "ExpLoopEnd",
										 NULL,
										 NULL));
		//ExpIsNeg:
		AppendSeq(Res1->Instrs,GenInstr("ExpIsNeg",NULL,
										NULL,
										NULL,
										NULL));
		//mtc1    $t4, $f5
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(reg3),
										TmpRegName(reg4),
										NULL));
		//cvt.s.w $f5, $f5
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg4),
										TmpRegName(reg4),
										NULL));
		//div     $f3, $f5, $f3
		AppendSeq(Res1->Instrs, GenInstr(NULL, "div.s",
										 TmpRegName(reg2),
										 TmpRegName(reg4),
										 TmpRegName(reg2)));
		//j		ExpLoopEnd
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
										 "ExpLoopEnd",
										 NULL,
										 NULL));
		//ExpIsOne:
		AppendSeq(Res1->Instrs,GenInstr("ExpIsOne",NULL,
										NULL,
										NULL,
										NULL));
		
		//mov.s		$f1,$f0
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mov.s",
										TmpRegName(reg2),
										TmpRegName(Res1->Reg),
										NULL));
		//j		ExpLoopEnd
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
										 "ExpLoopEnd",
										 NULL,
										 NULL));
		
		//ExpIsNegOne:
		AppendSeq(Res1->Instrs,GenInstr("ExpIsNegOne",NULL,
										NULL,
										NULL,
										NULL));
		//mov.s		$f1,$f0
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mov.s",
										TmpRegName(reg2),
										TmpRegName(Res1->Reg),
										NULL));
		//mtc1    $t4, $f5
		AppendSeq(Res1->Instrs,GenInstr(NULL,"mtc1",
										TmpRegName(reg3),
										TmpRegName(reg4),
										NULL));
		//cvt.s.w $f5, $f5
		AppendSeq(Res1->Instrs,GenInstr(NULL,"cvt.s.w",
										TmpRegName(reg4),
										TmpRegName(reg4),
										NULL));
		//div     $f3, $f5, $f3
		AppendSeq(Res1->Instrs, GenInstr(NULL, "div.s",
										 TmpRegName(reg2),
										 TmpRegName(reg4),
										 TmpRegName(reg2)));
		//j		ExpLoopEnd
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
										 "ExpLoopEnd",
										 NULL,
										 NULL));
		//ExpIsZero:
		AppendSeq(Res1->Instrs,GenInstr("ExpIsZero",NULL,
										NULL,
										NULL,
										NULL));
		//li.s      $f3, 1.0
		AppendSeq(Res1->Instrs,GenInstr(NULL,"li.s",
										TmpRegName(reg2),
										"1.0",
										NULL));
		//j		ExpLoopEnd
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
										 "ExpLoopEnd",
										 NULL,
										 NULL));
		//ReverseExpo:
		AppendSeq(Res1->Instrs,GenInstr("ReverseExpo",NULL,
										NULL,
										NULL,
										NULL));
		//sub		$t1, $0, $t1
		AppendSeq(Res1->Instrs, GenInstr(NULL, "sub",
										 TmpRegName(Res2->Reg),
										 "$0",
										 TmpRegName(Res2->Reg)));
		//j		ExpLoopStart
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j",
										 "ExpLoopStart",
										 NULL,
										 NULL));
		//ExpLoopEnd:
		AppendSeq(Res1->Instrs,GenInstr("ExpLoopEnd",NULL,
										NULL,
										NULL,
										NULL));
		printf("doExpon2\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
		ReleaseTmpReg(reg);
		ReleaseTmpReg(reg3);
		ReleaseTmpReg(reg4);
		ReleaseTmpReg(reg5);
		Res1->Reg = reg2;
		free(Res2);
		printf("doExpon3\n");
	}

	return Res1;
}

struct ExprRes *  doref(char* idname)  {
	struct ExprRes * result;
	result = (struct ExprRes *) malloc (sizeof(struct ExprRes));
	int augumentReg;
	
	augumentReg = AvailPTmpReg();
	result -> Instrs = GenInstr(NULL,"la",TmpRegName(augumentReg),idname,NULL);
	AppendSeq(result -> Instrs, GenInstr(NULL,"addi","$sp","$sp","-4"));
	AppendSeq(result -> Instrs, GenInstr(NULL,"sw",TmpRegName(augumentReg),"0($sp)",NULL));
	
	ReleaseTmpReg(augumentReg);
	
	return result;
}

struct ExprRes * doSimpleExpon(struct ExprRes * Res1, struct ExprRes * Res2) {
	int reg1;
	int reg2;
	
	struct CondRes *cres1;
	cres1 = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres1 -> Label = GenLabel();
	
	struct CondRes *cres2;
	cres2 = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres2 -> Label = GenLabel();
	
	printf("doSimpleExp1\n");
	
	if (Res1->Reg >= 10) {
		reg1 = AvailFTmpReg();
		reg2 = AvailTmpReg();
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li.s", TmpRegName(reg1), "1.0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "ori", TmpRegName(reg2), "$zero", "0"));
		AppendSeq(Res1->Instrs, GenInstr(cres1 -> Label, NULL, NULL, NULL, NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", TmpRegName(Res2->Reg), TmpRegName(reg2), cres2 -> Label));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "mul.s", TmpRegName(reg1), TmpRegName(reg1), TmpRegName(Res1->Reg)));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "addi", TmpRegName(reg2), TmpRegName(reg2), "1"));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", "$0", "$0", cres1 -> Label));
		AppendSeq(Res1->Instrs, GenInstr(cres2 -> Label, NULL, NULL, NULL, NULL));
		printf("doEasyExp2\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
	} else {
		reg1 = AvailTmpReg();
		reg2 = AvailTmpReg();
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg1), "1", NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "ori", TmpRegName(reg2), "$0", "0"));
		AppendSeq(Res1->Instrs, GenInstr(cres1 -> Label, NULL, NULL, NULL, NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", TmpRegName(Res2->Reg), TmpRegName(reg2), cres2 -> Label));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "mul", TmpRegName(reg1), TmpRegName(reg1), TmpRegName(Res1->Reg)));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "addi", TmpRegName(reg2), TmpRegName(reg2), "1"));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", "$0", "$0", cres1 -> Label));
		AppendSeq(Res1->Instrs, GenInstr(cres2 -> Label, NULL, NULL, NULL, NULL));
		printf("doEasyExp2\n");
		ReleaseTmpReg(Res1->Reg);
		ReleaseTmpReg(Res2->Reg);
	}
	
	Res1->Reg = reg1;
	free(Res2);
	printf("doEasyExp3\n");
	return Res1;
}

struct ExprRes *  doNEQ(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doNEQ\n");
	int reg;
	reg = AvailBTmpReg();
	
	struct CondRes *cres;
	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres -> Label = GenLabel();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL,"li",TmpRegName(reg),"0",NULL));
	
	if (Res1->Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1 -> Instrs, Res2-> Instrs);
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "c.eq.s", TmpRegName(Res2 -> Reg), TmpRegName(Res1 -> Reg),NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "bc1t", cres->Label ,NULL,NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(cres -> Label, NULL, NULL ,NULL,NULL));
	}else if (Res1 -> Reg < 6 && Res2 ->Reg < 6) {
		AppendSeq(Res1 -> Instrs, Res2-> Instrs);
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "beq", TmpRegName(Res2 -> Reg), TmpRegName(Res1 -> Reg),cres->Label));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(cres -> Label, NULL, NULL ,NULL,NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL,"li",TmpRegName(reg),"1",NULL));
	}

	
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	
	Res1 -> Reg = reg;
	
	free (Res2);
	
	
	printf("reg number %d \n", Res1 -> Reg);
	return Res1;
	
	
}
struct ExprRes *  doEQ(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doEQ\n");
	
	int reg;
	reg = AvailBTmpReg();
	
	struct CondRes *cres;
	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres -> Label = GenLabel();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL,"li",TmpRegName(reg),"0",NULL));
	
	if (Res1->Reg >= 10 && Res2 -> Reg >= 10) {
		printf("doEQ1\n");
		AppendSeq(Res1 -> Instrs, Res2-> Instrs);
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "c.eq.s", TmpRegName(Res2 -> Reg), TmpRegName(Res1 -> Reg),NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "bc1f", cres->Label ,NULL,NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(cres -> Label, NULL, NULL ,NULL,NULL));
	}else if (Res1 -> Reg < 6 && Res2 ->Reg < 6) {
		printf("doEQ2\n");
		AppendSeq(Res1 -> Instrs, Res2-> Instrs);
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "bne", TmpRegName(Res2 -> Reg), TmpRegName(Res1 -> Reg),cres->Label));
		AppendSeq(Res1 -> Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",NULL));
		AppendSeq(Res1 -> Instrs, GenInstr(cres -> Label, NULL, NULL ,NULL,NULL));
	}else {
		printf("doEQ3\n");
		AppendSeq(Res1->Instrs, GenInstr(NULL,"li",TmpRegName(reg),"0",NULL));
	}
	
	
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	
	Res1 -> Reg = reg;
	
	free (Res2);
	
	return Res1;
}
struct ExprRes *  doLT(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doLT\n");
	int reg;
	reg = AvailBTmpReg();
	
	struct CondRes *cres;
	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres -> Label = GenLabel();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1", NULL));
	
	if (Res1->Reg >= 10 && Res2->Reg >= 10) {
		printf("doLT1\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.lt.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else if (Res1->Reg < 6 && Res2->Reg < 6) {
		printf("doLT2\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "blt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else {
		printf("doLT3\n");
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
	}
	
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	
	Res1->Reg = reg;
	free(Res2);
	return Res1;
}
struct ExprRes *  doGT(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doGT\n");
	int reg;
	reg = AvailBTmpReg();
	
	struct CondRes *cres;
	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres -> Label = GenLabel();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1", NULL));
	
	if (Res1->Reg >= 10 && Res2->Reg >= 10) {
		printf("doGT1\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.le.s", TmpRegName(Res2->Reg), TmpRegName(Res1->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else if (Res1->Reg < 6 && Res2->Reg < 6) {
		printf("doGT2\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bgt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else {
		printf("doGT3\n");
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
	}
	
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	
	Res1->Reg = reg;
	free(Res2);
	return Res1;
}
struct ExprRes *  doLTE(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doLTE\n");
	int reg;
	reg = AvailBTmpReg();
	
	struct CondRes *cres;
	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres -> Label = GenLabel();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1", NULL));
	
	if (Res1->Reg >= 10 && Res2->Reg >= 10) {
		printf("doLTE1\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.le.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else if (Res1->Reg < 6 && Res2->Reg < 6) {
		printf("doLTE2\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "ble", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else {
		printf("doLTE3\n");
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
	}
	
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	
	Res1->Reg = reg;
	free(Res2);
	return Res1;
}
struct ExprRes *  doGTE(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doGTE\n");
	int reg;
	reg = AvailBTmpReg();
	
	struct CondRes *cres;
	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres -> Label = GenLabel();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1", NULL));
	
	if (Res1->Reg >= 10 && Res2->Reg >= 10) {
		printf("doGTE1\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.le.s", TmpRegName(Res2->Reg), TmpRegName(Res1->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else if (Res1->Reg < 6 && Res2->Reg < 6) {
		printf("doGTE2\n");
		AppendSeq(Res1->Instrs, Res2->Instrs);
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bge", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
		AppendSeq(Res1->Instrs, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	} else {
		printf("doGTE3\n");
		AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "0", NULL));
	}
	
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);
	
	Res1->Reg = reg;
	free(Res2);
	return Res1;
}
struct ExprRes *  doNOT(struct ExprRes * Res1){
	printf("doNOT\n");
	int reg;
	reg = AvailBTmpReg();
	
	AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1", NULL));
	AppendSeq(Res1->Instrs, GenInstr(NULL, "xor", TmpRegName(reg), TmpRegName(Res1->Reg), TmpRegName(reg)));
	ReleaseTmpReg(Res1->Reg);	
	Res1->Reg = reg;
	return Res1;
}
struct ExprRes *  doOR(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doOR\n");
	int reg;
	reg = AvailBTmpReg();
	
	AppendSeq(Res1->Instrs, Res2->Instrs);
	AppendSeq(Res1->Instrs, GenInstr(NULL, "or", TmpRegName(reg), TmpRegName(Res1->Reg), TmpRegName(Res2->Reg)));
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);	
	Res1->Reg = reg;
	free(Res2);
	return Res1;
}
struct ExprRes *  doAND(struct ExprRes * Res1,  struct ExprRes * Res2){
	printf("doAND\n");
	int reg;
	reg = AvailBTmpReg();
	
	AppendSeq(Res1->Instrs, Res2->Instrs);
	AppendSeq(Res1->Instrs, GenInstr(NULL, "and", TmpRegName(reg), TmpRegName(Res1->Reg), TmpRegName(Res2->Reg)));
	ReleaseTmpReg(Res1->Reg);
	ReleaseTmpReg(Res2->Reg);	
	Res1->Reg = reg;
	free(Res2);
	return Res1;
}

//if do the multiple Print, we assume the number of variable is not bigger than 3, since register available for boolean is 3. 
struct InstrSeq * doPrint(struct ExprResList * Exprls) { 

  struct InstrSeq *code;
	printf("doPrint\n");
  
	
		struct ExprRes * Expra;
		Expra = (struct ExprRes*)malloc(sizeof(struct ExprRes));
		Expra = Exprls -> Expr;
   
	code = (Exprls->Expr)->Instrs;
	if (nestedControl == 1) {
		AppendSeq(code,doPrintSpace());
	}
	
  if (Expra -> Reg < 6) {
	  printf("doPrint1\n");
	  AppendSeq(code,GenInstr(NULL,"li","$v0","1",NULL));
	  AppendSeq(code,GenInstr(NULL,"move","$a0",TmpRegName(Expra->Reg),NULL));
	  AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
	  /*
	  AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
	  AppendSeq(code,GenInstr(NULL,"la","$a0","_nl",NULL));
	  AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
	  */
	  ReleaseTmpReg(Expra->Reg);
	  free(Expra);
	  free(Exprls);
  }else if(Expra -> Reg >= 10){
	  printf("doPrint2\n");
	  AppendSeq(code,GenInstr(NULL,"li","$v0","2",NULL));
	  AppendSeq(code,GenInstr(NULL,"mov.s","$f12",TmpRegName(Expra->Reg),NULL));
	  AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
	  /*
	  AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
	  AppendSeq(code,GenInstr(NULL,"la","$a0","_nl",NULL));
	  AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
	  */
	   ReleaseTmpReg(Expra->Reg);
	  free(Expra);
	  free(Exprls);
  }else{
	  printf("doPrint3\n");
	  struct CondRes* cres;
	  cres = (struct CondRes *) malloc(sizeof (struct CondRes));
	  cres -> Label = GenLabel();
	  
	  AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
	  AppendSeq(code,GenInstr(NULL,"la","$a0","False",NULL));
	  AppendSeq(code,GenInstr(NULL,"beq","$0",TmpRegName(Expra -> Reg),cres -> Label));	  
	  AppendSeq(code,GenInstr(NULL,"la","$a0","True",NULL));
	  AppendSeq(code,GenInstr(cres -> Label,NULL,NULL,NULL,NULL));
	  AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
	  
	  ReleaseTmpReg(Expra->Reg);
	  free(Expra);
	  free(Exprls);
  }
	if ((Exprls-> Next) != NULL) {
		struct ExprRes *pointtoExpr;
		printf("nested\n");
		nestedControl = 1;
		pointtoExpr = (Exprls -> Expr);
		ReleaseTmpReg (pointtoExpr -> Reg);
		AppendSeq(code,doPrint(Exprls = Exprls -> Next));		
	}

	nestedControl = 0;
  // WriteSeq(code);
    return code;
}

struct InstrSeq * doPrintLn() {
	printf("doPrintLn\n");
	
	struct InstrSeq *code;	
	
	code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
	AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));
	AppendSeq(code, GenInstr(NULL, "la", "$a0", "_nl", NULL));
	AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));
	
	return code;
}

struct InstrSeq * doPrintSpace() {
	printf("doPrintSpace\n");
	
	struct InstrSeq *code;	
	
	code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
	AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));
	AppendSeq(code, GenInstr(NULL, "la", "$a0", "_space", NULL));
	AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));
	
	return code;
}


struct InstrSeq * doAssign(char * name, struct ExprRes * Expr) { 

  struct InstrSeq *code;
  struct SymEntry *temp;
  struct typeInfo *currentAttribute;
	

  
  printf("doAssignt\n");
	if (!(temp = FindName(table, name))) {
		if (!(temp = FindName(argumentTable, name))) {
			WriteIndicator(GetCurrentColumn());
			WriteMessage("Undeclared variable");
		}		
	}
  
  code = Expr->Instrs;
  currentAttribute = GetAttr(temp);
  if ((currentAttribute -> type)==0 && (currentAttribute -> localVariable)==0) {
  printf("doAssign0::%s\n",name);
  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));

  ReleaseTmpReg(Expr->Reg);
  free(Expr);
  }else if ((currentAttribute -> type)==1 && (currentAttribute -> localVariable)==0) {
	  printf("doAssign1::%s\n",name);
	  AppendSeq(code,GenInstr(NULL,"swc1",TmpRegName(Expr->Reg),name,NULL));	  
	  ReleaseTmpReg(Expr->Reg);
	  free(Expr);
  }else if ((currentAttribute -> type)==2 && (currentAttribute -> localVariable)==0) {
	  printf("doAssign2\n");
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));	  
	  ReleaseTmpReg(Expr->Reg);
	  free(Expr);
  }else if ((currentAttribute -> type)==0 && (currentAttribute -> localVariable)==1) {
	  printf("doAssign3\n");
	  int saveReg = AvailSTmpReg();
	  int reg = AvailTmpReg();
	  localVariableList[pointerforthelist] = (char *) malloc (sizeof(char));
	  localVariableList[pointerforthelist] = name;
	  AppendSeq(code,GenInstr(NULL,"lw",TmpRegName(reg),name,NULL));
	  AppendSeq(code,GenInstr(NULL,"move",TmpRegName(saveReg),TmpRegName(reg),NULL));
	  AppendSeq(code,GenInstr(NULL,"addi","$sp","$sp","-4"));
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(saveReg),"0($sp)",NULL));
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));	  
	  ReleaseTmpReg(Expr->Reg);
	  ReleaseTmpReg(saveReg);
	  ReleaseTmpReg(reg);
	  pointerforthelist ++;
	  free(Expr);
	  (currentAttribute -> localVariable) = 0;
  }else if ((currentAttribute -> type)==1 && (currentAttribute -> localVariable)==1) {
	  printf("doAssign4\n");
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));	  
	  ReleaseTmpReg(Expr->Reg);
	  free(Expr);
  }else if ((currentAttribute -> type)==2 && (currentAttribute -> localVariable)==1) {
	  printf("doAssign5\n");
	  int saveReg = AvailSTmpReg();
	  int reg = AvailTmpReg();
	  localVariableList[pointerforthelist] = (char *) malloc (sizeof(char));
	  localVariableList[pointerforthelist] = name;
	  AppendSeq(code,GenInstr(NULL,"lw",TmpRegName(reg),name,NULL));
	  AppendSeq(code,GenInstr(NULL,"move",TmpRegName(saveReg),TmpRegName(reg),NULL));
	  AppendSeq(code,GenInstr(NULL,"addi","$sp","$sp","-4"));
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(saveReg),"0($sp)",NULL));
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));	  
	  ReleaseTmpReg(Expr->Reg);
	  ReleaseTmpReg(saveReg);
	  ReleaseTmpReg(reg);
	  pointerforthelist ++;
	  free(Expr);
	  (currentAttribute -> localVariable) = 0;
  }else if ((currentAttribute -> type)==7 && (currentAttribute -> localVariable)==2){
	  printf("doAssign6\n");
	  int saveReg = AvailSTmpReg();
	  int reg = AvailTmpReg();
	  localVariableList[pointerforthelist] = (char *) malloc (sizeof(char));
	  localVariableList[pointerforthelist] = name;
	  AppendSeq(code,GenInstr("here","lw",TmpRegName(reg),name,NULL));
	  AppendSeq(code,GenInstr(NULL,"move",TmpRegName(saveReg),TmpRegName(reg),NULL));
	  AppendSeq(code,GenInstr(NULL,"addi","$sp","$sp","-4"));
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(saveReg),"0($sp)",NULL));
	  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));	  
	  ReleaseTmpReg(Expr->Reg);
	  ReleaseTmpReg(saveReg);
	  ReleaseTmpReg(reg);
	  pointerforthelist ++;
	  free(Expr);
	  (currentAttribute -> localVariable) = 0;
  }
	
  return code;
}

struct InstrSeq * doArrayAssign(char * name, struct ExprRes * Expr1, struct ExprRes * Expr2) {
	struct InstrSeq *code;
	struct SymEntry *temp;
	struct typeInfo *currentAttribute;
	
	
	int addressReg;
	int newoffsetReg;
	addressReg = AvailTmpReg();
	newoffsetReg = AvailTmpReg();
	
	printf("doArrayAssignt\n");
	if (!(temp = FindName(table, name))) {
		WriteIndicator(GetCurrentColumn());
		WriteMessage("Undeclared variable");
	}
	
	//currentAttribute = GetAttr(temp);
	
	if (Expr2 -> Reg < 10) {
		code = Expr2->Instrs;
		AppendSeq(code, Expr1 -> Instrs);
		AppendSeq(code,GenInstr(NULL,"mul",TmpRegName(Expr1 -> Reg),TmpRegName(Expr1 -> Reg),"4"));
		AppendSeq(code,GenInstr(NULL,"la",TmpRegName(addressReg),name,NULL));
		AppendSeq(code,GenInstr(NULL,"add",TmpRegName(newoffsetReg),TmpRegName(addressReg),TmpRegName(Expr1 -> Reg)));
		AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr2->Reg),indirectAddress(TmpRegName(newoffsetReg)),NULL));	
	}else if (Expr2 -> Reg >= 10 && Expr2 -> Reg < 20) {
		code = Expr2->Instrs;
		AppendSeq(code, Expr1 -> Instrs);
		AppendSeq(code,GenInstr(NULL,"mul",TmpRegName(Expr1 -> Reg),TmpRegName(Expr1 -> Reg),"4"));
		AppendSeq(code,GenInstr(NULL,"la",TmpRegName(addressReg),name,NULL));
		AppendSeq(code,GenInstr(NULL,"add",TmpRegName(newoffsetReg),TmpRegName(addressReg),TmpRegName(Expr1 -> Reg)));
		AppendSeq(code,GenInstr(NULL,"swc1",TmpRegName(Expr2->Reg),indirectAddress(TmpRegName(newoffsetReg)),NULL));
	}
	
	
	
	ReleaseTmpReg(addressReg);
	ReleaseTmpReg(newoffsetReg);
	ReleaseTmpReg(Expr1 -> Reg);
	ReleaseTmpReg(Expr2 -> Reg);
	free(Expr1);
	free(Expr2);
	
	return code;
}

char * indirectAddress (char *regName){
	printf("indirectAddress%s\n",regName);
	char * result;
	result = (char *) malloc (sizeof(char));
	sprintf(result, "(%s)", regName);
	printf("indirectAddressEnd\n");
	return result;
}

char * getArraySize (char *sizename){
	char * result;
	result = (char *) malloc (sizeof(char));
	strcpy (result, sizename);
	return result;
}

extern struct CondRes * doEqual(struct ExprRes * Res1,  struct ExprRes * Res2) {
	printf("doEqual\n");
	struct CondRes * cres;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres->Label = GenLabel();
	if (Res1 -> Reg < 10 && Res2 -> Reg < 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bne", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
	}else if (Res1 -> Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.eq.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1f", cres->Label, NULL, NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j", cres->Label, NULL, NULL));
	}
	
	cres->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return cres;
}

extern struct CondRes * doGreaterThan(struct ExprRes * Res1,  struct ExprRes * Res2) {
	printf("doGreaterThan\n");
	struct CondRes * cres;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres->Label = GenLabel();
	if (Res1 -> Reg < 10 && Res2 -> Reg < 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "ble", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
	}else if (Res1 -> Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.le.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j", cres->Label, NULL, NULL));
	}
	cres->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return cres;
}

extern struct CondRes * doNotEqual(struct ExprRes * Res1,  struct ExprRes * Res2) {
	printf("doNotEqual\n");
	struct CondRes * cres;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres->Label = GenLabel();
	if (Res1 -> Reg < 10 && Res2 -> Reg < 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
	}else if (Res1 -> Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.eq.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j", cres->Label, NULL, NULL));
	}
	cres->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return cres;
}

extern struct CondRes * doLessThan(struct ExprRes * Res1,  struct ExprRes * Res2) {
	printf("doLessThan\n");
	struct CondRes * cres;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres->Label = GenLabel();
	if (Res1 -> Reg < 10 && Res2 -> Reg < 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bge", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
	}else if (Res1 -> Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.lt.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1f", cres->Label, NULL, NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j", cres->Label, NULL, NULL));
	}
	cres->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return cres;
}

extern struct CondRes * doLessThanEqual(struct ExprRes * Res1,  struct ExprRes * Res2) {
	printf("doLessThanEqual\n");
	struct CondRes * cres;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres->Label = GenLabel();
	if (Res1 -> Reg < 10 && Res2 -> Reg < 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bgt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
	}else if (Res1 -> Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.le.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1f", cres->Label, NULL, NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j", cres->Label, NULL, NULL));
	}
	cres->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return cres;
}

extern struct CondRes * doGreaterThanEqual(struct ExprRes * Res1,  struct ExprRes * Res2) {
	printf("doGreaterThanEqual\n");
	struct CondRes * cres;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	cres = (struct CondRes *) malloc(sizeof(struct CondRes));
	cres->Label = GenLabel();
	if (Res1 -> Reg < 10 && Res2 -> Reg < 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "blt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), cres->Label));
	}else if (Res1 -> Reg >= 10 && Res2 -> Reg >= 10) {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "c.lt.s", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), NULL));
		AppendSeq(Res1->Instrs, GenInstr(NULL, "bc1t", cres->Label, NULL, NULL));
	}else {
		AppendSeq(Res1->Instrs, GenInstr(NULL, "j", cres->Label, NULL, NULL));
	}
	cres->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return cres;
}

extern struct InstrSeq * doIf(struct CondRes * cres, struct InstrSeq * seq) {
	printf("doif\n");
	struct InstrSeq * seq2;
	seq2 = AppendSeq(cres->Instrs, seq);
	AppendSeq(seq2, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	free(cres);
	return seq2;
}

extern struct InstrSeq * doElse(struct CondRes * cres, struct InstrSeq * seq, struct InstrSeq * seq1) {
	printf("doElse\n");
	struct InstrSeq * seq2;
	seq2 = AppendSeq(cres->Instrs, seq);
	AppendSeq(seq2, GenInstr(NULL, "j", "finishpart", NULL, NULL));
	AppendSeq(seq2, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	AppendSeq(seq2,seq1);
	free(cres);
	return seq2;
}

extern struct InstrSeq * doWhile(struct CondRes * cres, struct InstrSeq * seq) {
	printf("doWhile\n");
	struct InstrSeq * seq2;
	
	struct CondRes * condlabel;
	condlabel = (struct CondRes *) malloc (sizeof(struct CondRes));
	condlabel -> Label = GenLabel();
	
	seq2 = GenInstr(condlabel->Label,NULL,NULL,NULL,NULL);
	AppendSeq(seq2, cres -> Instrs);
	AppendSeq(seq2, seq);
	AppendSeq(seq2, GenInstr(NULL, "j", condlabel -> Label, NULL,NULL));
	AppendSeq(seq2, GenInstr(cres->Label, NULL, NULL, NULL, NULL));
	free(cres);
	free(condlabel);
	
	return seq2;
}

void							 
Finish(struct InstrSeq *Code)
{ struct InstrSeq *code;
  struct SymEntry *entry;
	struct SymEntry *fsEntry;
  struct Attr * attr;

  
  printf("Finish\n");
// if (!Code) printf("ERROR\n");
// WriteSeq(Code);
	code = (struct InstrSeq*) malloc (sizeof (struct InstrSeq));
  code = GenInstr(NULL,".text",NULL,NULL,NULL);
  AppendSeq(code,GenInstr(NULL,".align","2",NULL,NULL));
  AppendSeq(code,GenInstr(NULL,".globl","main",NULL,NULL));
  AppendSeq(code, GenInstr("main",NULL,NULL,NULL,NULL));
   printf("Finish2\n");
	AppendSeq(code,Code);  
   printf("Finish3\n");
	AppendSeq(code, GenInstr("finishpart",NULL,NULL,NULL,NULL));
  AppendSeq(code, GenInstr(NULL, "li", "$v0", "10", NULL)); 
  AppendSeq(code, GenInstr(NULL,"syscall",NULL,NULL,NULL));
	//AppendSeq(code, GenInstr("FUNCTIONS",NULL,NULL,NULL,NULL));
	printf("Finish8\n");
		fsEntry = FirstEntry(table);
	 while (fsEntry) {
	 struct typeInfo * attribute;
	 struct InstrSeq * functionInstrs;
		 attribute = GetAttr(fsEntry);
		 
    // functionInstrs = (struct InstrSeq *) malloc (sizeof (struct InstrSeq));
		 
	 //PrintoutSeq(code);
	//	 PrintoutSeq(functionInstrs);
	 if ((attribute -> type) ==  6) {
		 functionInstrs = attribute -> instrs;
		 AppendSeq(code,GenInstr(NULL,NULL,NULL,NULL,NULL));
		 AppendSeq(code,functionInstrs);
		 AppendSeq(code,GenInstr(NULL,NULL,NULL,NULL,NULL));
		 //PrintoutSeq(code);
	 printf("Finish7\n");
		 
	 }	 
	 fsEntry = NextEntry(table, fsEntry);	 
	 }
//	AppendSeq(code, GenInstr("FUNCTIONSEND",NULL,NULL,NULL,NULL)); 	
  AppendSeq(code,GenInstr(NULL,".data",NULL,NULL,NULL));
  AppendSeq(code,GenInstr(NULL,".align","4",NULL,NULL));
  AppendSeq(code,GenInstr("_nl",".asciiz","\"\\n\"",NULL,NULL));
  AppendSeq(code,GenInstr("_space",".asciiz","\" \"",NULL,NULL));
  AppendSeq(code,GenInstr("True",".asciiz","\"true\"",NULL,NULL));
  AppendSeq(code,GenInstr("False",".asciiz","\"false\"",NULL,NULL));
  AppendSeq(code,GenInstr("IS",".asciiz","\"Enter an integer\\n\"",NULL,NULL));
	AppendSeq(code,GenInstr("FS",".asciiz","\"Enter an float\\n\"",NULL,NULL));
	AppendSeq(code,GenInstr("BS",".asciiz","\"Enter an boolean, 0 for false, 1 for true\\n\"",NULL,NULL));

 printf("Finish4\n");

 entry = FirstEntry(table);
 while (entry) {
	 struct typeInfo * attribute;
	 char * combine;
	 combine = (char *) malloc (sizeof(char));
	 attribute = GetAttr(entry);
	 if ((attribute -> type) >=  3 &&(attribute -> type) !=  6) {
		 sprintf(combine, "0:%s", attribute -> arraysize);
		 AppendSeq(code,GenInstr((char *) GetName(entry),".word",combine,NULL,NULL));
	 }else if ((attribute -> type) ==  6) {
	 }
	 else{
		 AppendSeq(code,GenInstr((char *) GetName(entry),".word","0",NULL,NULL));
		 }
        entry = NextEntry(table, entry);
	 
 }
  
  printf("writing code\n");
  WriteSeq(code);
  
  return;
}




