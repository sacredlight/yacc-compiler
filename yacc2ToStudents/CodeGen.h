/* CodeGen.h
   Routines to support the generation of assembly code.
   
*/

#include <stdio.h>

extern FILE *AssmFile;

struct InstrSeq {
  char *Label;
  char *OpCode;
  char *Oprnd1;
  char *Oprnd2;
  char *Oprnd3;
  struct InstrSeq *Next;
};

extern void							 InitCodeGen(char *AFilename);

extern struct InstrSeq * GenInstr(char *Label, char *OpCode, 
                                  char *Oprnd1, char *Oprnd2, char *Oprnd3);
extern struct InstrSeq * AppendSeq(struct InstrSeq *Seq1, 
                                   struct InstrSeq *Seq2);
extern void 						 WriteSeq(struct InstrSeq *ASeq);
extern void							PrintoutSeq(struct InstrSeq *ASeq);
extern char *						 GenLabel();

extern int  						 AvailTmpReg();
extern int  						 AvailFTmpReg();
extern int  						 AvailBTmpReg();
extern int  						 AvailPTmpReg();
extern int  						 AvailSTmpReg();
extern int  						 AvailRTmpReg();

extern char *						 TmpRegName(int RegNum);
extern void							 ReleaseTmpReg(int ANum);
extern void 	           ResetAllTmpReg();
extern struct InstrSeq * SaveSeq();
extern struct InstrSeq * RestoreSeq();

extern char *						 Imm(int Val);
extern char *						 RegOff(int Offset, char * Reg);