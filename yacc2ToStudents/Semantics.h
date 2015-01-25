/* Semantics.h
   The action and supporting routines for performing semantics processing.
*/




/* Semantic Records */
struct IdList {
  struct SymEntry * TheEntry;
  struct IdList * Next;
};

struct ExprRes {
  int Reg;
  struct InstrSeq * Instrs;
};

struct ExprResList {
	struct ExprRes *Expr;
	struct ExprResList * Next;
};

struct CondRes {
  char * Label;
  struct InstrSeq * Instrs;
};


/* Semantics Actions */
extern struct ExprRes *  doIntLit(char * digits);
extern struct ExprRes *  doFloatLit(char * digits);
extern struct ExprRes *  doBooleanLit(char * bl);
char *  doIdent(char * name);
char * indirectAddress (char *regName);
char * getArraySize (char *size);
extern struct ExprRes *  doRval(char * name);
extern struct ExprRes *  doArrayRval(char * name, struct ExprRes * Res1);
extern struct InstrSeq *  doAssign(char * name,  struct ExprRes * Res1);
extern struct InstrSeq * doArrayAssign(char * name, struct ExprRes * Expr1, struct ExprRes * Expr2); 
extern struct ExprResList *  doLink(struct ExprRes * Res1, struct ExprResList * Resls1);
extern struct ExprRes *  doAdd(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doMinus(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doMult(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doDiv(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doMod(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doNEQ(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doEQ(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doLT(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doGT(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doLTE(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doGTE(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doNOT(struct ExprRes * Res1);
extern struct ExprRes *  doOR(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doAND(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doExpon(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doSimpleExpon(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct InstrSeq *  doPrint(struct ExprResList * Exprls);
extern struct InstrSeq * doRead(char * name);
extern struct InstrSeq *  doPrintLn();
extern struct InstrSeq *  doPrintSpace();
extern struct CondRes * doEqual (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct CondRes * doNotEqual (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct CondRes * doLessThan (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct CondRes * doGreaterThan (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct CondRes * doLessThanEqual (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct CondRes * doGreaterThanEqual (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct InstrSeq * doIf(struct CondRes * cres1, struct InstrSeq * seq);
extern struct InstrSeq * doElse(struct CondRes * cres, struct InstrSeq * seq, struct InstrSeq * seq1);
extern struct InstrSeq * doWhile(struct CondRes * cres, struct InstrSeq * seq);
extern struct InstrSeq * doVoidFuctions(char * functionName, struct InstrSeq* argu, struct InstrSeq * instr1);
extern struct InstrSeq * doMainFuctions(struct InstrSeq * instr1);
extern struct InstrSeq * callfc(char * functionName,struct ExprResList * exprls);
extern struct InstrSeq * doLinkInstrs(struct InstrSeq *inst1, struct InstrSeq *inst2);
extern struct ExprRes * doLinkExpr(struct ExprRes * Res1, struct ExprRes * Res2);
extern struct InstrSeq * doGeneralArgument(char * name);
extern struct InstrSeq * doRefArgument(char * name);
extern struct ExprRes *  doref(char * name); 
extern void	Finish(struct InstrSeq *Code);
