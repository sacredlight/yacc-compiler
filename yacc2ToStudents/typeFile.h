#define INT 0
#define FLOAT 1
#define BOOLEAN 2
#define INTARRAY 3
#define FLOATARRAY 4
#define BOOLEANARRAY 5
#define FUNCTIONS 6
#define PRESERVEFORRVAL 7

struct typeInfo
{
	int type;
	char * arraysize;
	char * Reg;
	struct InstrSeq * instrs;
	int localVariable; //0 is not local
};