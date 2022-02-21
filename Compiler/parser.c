//Jarod Gomberg
//COP3402 HW3 parser.c
//Dec. 2, 2021

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

typedef struct { //create an additional struct for tokens
	char type[3];
	char val[12];
} Token;

Token* token; //for the token program is currently on
instruction* code;
int cIndex = 0;
symbol* table;
int tIndex = 0;
int level = 0, tIdx = 0; //lexilevel and tokenList index

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();
void program(lexeme* tokenList);
void block(lexeme* tokenList);
int var_declaration(lexeme* tokenList);
void const_declaration(lexeme* tokenList);
void proc_declaration(lexeme* tokenList);
void statement(lexeme* tokenList);
void condition(lexeme* tokenList);
void expression(lexeme* tokenList);
void term(lexeme* tokenList);
void factor(lexeme* tokenList);
int findsymbol(lexeme name, int kind);
int multipledeclarationcheck(lexeme* tokenList);
void mark();
void getNextToken();

instruction* parse(lexeme* list, int printTable, int printCode)
{
	code = NULL;
	table = malloc(100*sizeof(symbol));  //declare and allocate memory for both code and symbol table
	code = malloc(1000*sizeof(instruction));

	program(list); //program call to begin and proceed recursivly and
	if (printTable == 1)
		printsymboltable();		//print necessary output based on input variables
	if (printCode == 1)
		printassemblycode();

	code[cIndex].opcode = -1; //remove comment for this line to signal VM end of code
	if (table != NULL) 	//check for table free if not yet done
		free(table);

	return code; //once finished it returns loaded code array for VM input
}


void emit(int opname, int level, int mvalue) //provided emit function
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m) //provided symbol table insert function
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void printparseerror(int err_code) //provided error function. Added exit(0) statement at end to terminate upon error
{
	switch (err_code)
	{
	case 1:
		printf("Parser Error: Program must be closed by a period\n");
		break;
	case 2:
		printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
		break;
	case 3:
		printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
		break;
	case 4:
		printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
		break;
	case 5:
		printf("Parser Error: Variables must be assigned using :=\n");
		break;
	case 6:
		printf("Parser Error: Only variables may be assigned to or read\n");
		break;
	case 7:
		printf("Parser Error: call must be followed by a procedure identifier\n");
		break;
	case 8:
		printf("Parser Error: if must be followed by then\n");
		break;
	case 9:
		printf("Parser Error: while must be followed by do\n");
		break;
	case 10:
		printf("Parser Error: Relational operator missing from condition\n");
		break;
	case 11:
		printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
		break;
	case 12:
		printf("Parser Error: ( must be followed by )\n");
		break;
	case 13:
		printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
		break;
	case 14:
		printf("Parser Error: Symbol declarations should close with a semicolon\n");
		break;
	case 15:
		printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
		break;
	case 16:
		printf("Parser Error: begin must be followed by end\n");
		break;
	case 17:
		printf("Parser Error: Bad arithmetic\n");
		break;
	case 18:
		printf("Parser Error: Confliciting symbol declarations\n");
		break;
	case 19:
		printf("Parser Error: Undeclared identifier\n");
		break;
	default:
		printf("Implementation Error: unrecognized error code\n");
		break;
	}

	free(code);
	free(table);
	exit(0); //added for termination
}


void printsymboltable() //outputs symbol table
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode() //outputs assembly code
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}

void program(lexeme* tokenList) {

	emit(7, 0, 0); //emit JMP
	addToSymbolTable(3, "main", 0, level, 0, 0); //add symbol "main" to table
	level = -1; //set intial level index

	block(tokenList);
	if (tokenList[tIdx].type != periodsym) { //here we check if token is period and if not print error
		printparseerror(1);
	}

	emit(9, 0, 3); //emit HALT

	for (int i = 0; i < cIndex; i++) {
		if (code[cIndex].opcode == 5) { //check if current code index has OP CAL
			code[cIndex].m = table[code[cIndex].m].addr;
		}
	}
	code[0].m = table[0].addr;
}

void block(lexeme* tokenList) {
	level++; //increment level

	int proc_declaration_idx = tIndex - 1;
	const_declaration(tokenList); //calls const function to get if token is constant

	int x = var_declaration(tokenList); //does the same for variable
	proc_declaration(tokenList); //and same for procedure

	table[proc_declaration_idx].addr = cIndex * 3;

	if (level == 0) {
		emit(6, 0, x); //emit INC
	}
	else {
		emit(6, 0, x + 3); //emit INC
	}

	statement(tokenList); //calls statement for statement check
	mark(); //calls to mark
	level--; //decrement level
}


void const_declaration(lexeme* tokenList) { //function is designed to use provided grammar to store constants
	if (tokenList[tIdx].type == constsym) { //searches through tokenList and checks conditions
		do {
			getNextToken();
			if (tokenList[tIdx].type != identsym) {
				printparseerror(2);
			}

			int symidx = multipledeclarationcheck(tokenList); //for additional declarations
			if (symidx != -1) {
				printparseerror(19);
			}

			char ident[12];
			strcpy(ident, tokenList[tIdx].name);

			getNextToken();

			if (tokenList[tIdx].type != assignsym) {
				printparseerror(2);
			}

			getNextToken();

			if (tokenList[tIdx].type != numbersym) {
				printparseerror(2);
			}

			addToSymbolTable(1, ident, tokenList[tIdx].value, level, 0, 0);

			getNextToken();
		} while (tokenList[tIdx].type == commasym);

		if (tokenList[tIdx].type != semicolonsym) {
			if (tokenList[tIdx].type == identsym) {
				printparseerror(13);
			}
			else {
				printparseerror(14);
			}
		}
		getNextToken();
	}
}

int var_declaration(lexeme* tokenList) { //passes through tokens to get and store any variables, while checking grammar rules accordingly
	int numVars = 0;
	if (tokenList[tIdx].type == varsym) {

		do {
			numVars++;
			getNextToken();
			if (tokenList[tIdx].type != identsym) {
				printparseerror(3);
			}
			int symidx = multipledeclarationcheck(tokenList); //additonal declarations
			if (symidx != -1) {
				printparseerror(18);
			}
			if (level == 0) {
				addToSymbolTable(2, tokenList[tIdx].name, 0, level, numVars - 1, 0);
			}
			else {
				addToSymbolTable(2, tokenList[tIdx].name, 0, level, numVars + 2, 0);
			}
			getNextToken();
		} while (tokenList[tIdx].type == commasym);

		if (tokenList[tIdx].type != semicolonsym) {
			if (tokenList[tIdx].type == identsym) {
				printparseerror(13);
			}
			else {
				printparseerror(14);
			}
		}
		getNextToken();
	}
	return numVars;
}

void proc_declaration(lexeme* tokenList) { //passes through tokens to get and store any procedures, while checking grammar rules accordingly

	while (tokenList[tIdx].type == procsym) {
		getNextToken();
		if (tokenList[tIdx].type != identsym) {
			printparseerror(4);
		}

		int symidx = multipledeclarationcheck(tokenList); //additonal declarations
		if (symidx != -1) {
			printparseerror(19);
		}

		addToSymbolTable(3, tokenList[tIdx].name, 0, level, 0, 0);
		getNextToken();
		if (tokenList[tIdx].type != semicolonsym) {
			printparseerror(4);
		}

		getNextToken();
		block(tokenList);
		if (tokenList[tIdx].type != semicolonsym) {
			printparseerror(14);
		}

		getNextToken();
		emit(2, 0, 0);
	}
}

void statement(lexeme* tokenList) { //passes through tokens to get and store any statements, while checking grammar rules accordingly
	if (tokenList[tIdx].type == identsym) {
		int symidx = findsymbol(tokenList[tIdx], 2);
		if (symidx == -1) {
			if (findsymbol(tokenList[tIdx], 1) != findsymbol(tokenList[tIdx], 3)) { //get symbols
				printparseerror(6);
			}
			else {
				printparseerror(19);
			}
		}
		getNextToken();
		if (tokenList[tIdx].type != assignsym) {
			printparseerror(5);
		}

		getNextToken();
		expression(tokenList);
		emit(4, level - table[symidx].level, table[symidx].addr);
		return;
	}
	if (tokenList[tIdx].type == beginsym) {
		do {
			getNextToken();
			statement(tokenList);
		} while (tokenList[tIdx].type == semicolonsym);
		if (tokenList[tIdx].type != endsym) {
			if (tokenList[tIdx].type == identsym || tokenList[tIdx].type == beginsym || tokenList[tIdx].type == ifsym || tokenList[tIdx].type == whilesym || tokenList[tIdx].type == readsym || tokenList[tIdx].type == writesym || tokenList[tIdx].type == callsym) {
				printparseerror(15);
			}
			else {
				printparseerror(16);
			}
		}
		getNextToken();
		return;
	}
	if (tokenList[tIdx].type == ifsym) {
		getNextToken();
		condition(tokenList);
		int jpcidx = cIndex;
		emit(8, 0, 0);

		if (tokenList[tIdx].type != thensym) {
			printparseerror(8);
		}

		getNextToken();
		statement(tokenList);
		if (tokenList[tIdx].type == elsesym) {
			int jmpidx = cIndex;
			emit(7, 0, 0);
			code[jpcidx].m = cIndex * 3;
			getNextToken();
			statement(tokenList);
			code[jmpidx].m = cIndex * 3;
		}
		else {
			code[jpcidx].m = cIndex * 3;
		}
		return;
	}

	if (tokenList[tIdx].type == whilesym) {
		getNextToken();
		int loopidx = cIndex;
		condition(tokenList);

		if (tokenList[tIdx].type != dosym) {
			printparseerror(9);
		}

		getNextToken();
		int jpcidx = cIndex;
		emit(8, 0, 0);
		statement(tokenList);
		emit(7, 0, loopidx * 3);
		code[jpcidx].m = cIndex * 3;
		return;
	}
	if (tokenList[tIdx].type == readsym) {
		getNextToken();
		if (tokenList[tIdx].type != identsym) {
			printparseerror(6);
		}

		int symIdx = findsymbol(tokenList[tIdx], 2);
		if (symIdx == -1) {
			if (findsymbol(tokenList[tIdx], 1) != findsymbol(tokenList[tIdx], 3)) { //get symbols
				printparseerror(18);
			}
			else {
				printparseerror(19);
			}
		}

		getNextToken();
		emit(9, 0, 2);
		emit(4, level - table[symIdx].level, table[symIdx].addr);

		return;
	}
	if (tokenList[tIdx].type == writesym) {
		getNextToken();
		expression(tokenList);
		emit(9, 0, 1);
		return;
	}
	if (tokenList[tIdx].type == callsym) {
		getNextToken();
		int symIdx = findsymbol(tokenList[tIdx], 3);
		if (symIdx == -1) {
			if (findsymbol(tokenList[tIdx], 1) != findsymbol(tokenList[tIdx], 2)) {
				printparseerror(7);
			}
			else {
				printparseerror(19);
			}
		}
		getNextToken();
		emit(5, level - table[symIdx].level, table[symIdx].addr);
	}
}

void condition(lexeme* tokenList) { //based on grammar provided, check conditions and store them accordingly
	if (tokenList[tIdx].type == oddsym) {
		getNextToken();
		expression(tokenList);
		emit(2, 0, 6);
	}
	else {
		expression(tokenList);
		if (tokenList[tIdx].type == eqlsym) {
			getNextToken();
			expression(tokenList);
			emit(2, 0, 8);
		}
		else if (tokenList[tIdx].type == neqsym) {
			getNextToken();
			expression(tokenList);
			emit(2, 0, 9);
		}
		else if (tokenList[tIdx].type == lsssym) {
			getNextToken();
			expression(tokenList);
			emit(2, 0, 10);
		}
		else if (tokenList[tIdx].type == leqsym) {
			getNextToken();
			expression(tokenList);
			emit(2, 0, 11);
		}
		else if (tokenList[tIdx].type == gtrsym) {
			getNextToken();
			expression(tokenList);
			emit(2, 0, 12);
		}
		else if (tokenList[tIdx].type == geqsym) {
			getNextToken();
			expression(tokenList);
			emit(2, 0, 13);
		}
		else {
			printparseerror(10);
		}
	}
}

void expression(lexeme* tokenList) { //uses provided grammar to check current token is a valid expression like plus, minus, etc. saves and stores accordingly
	if (tokenList[tIdx].type == subsym) {
		getNextToken();
		term(tokenList);
		emit(2, 0, 1);
		while (tokenList[tIdx].type == addsym || tokenList[tIdx].type == subsym) {
			if (tokenList[tIdx].type == addsym) {
				getNextToken();
				term(tokenList);
				emit(2, 0, 2);
			}
			else {
				getNextToken();
				term(tokenList);
				emit(2, 0, 3);
			}
		}
	}
	else {
		if (tokenList[tIdx].type == addsym) {
			getNextToken();
		}
		term(tokenList);
		while (tokenList[tIdx].type == addsym || tokenList[tIdx].type == subsym) {
			if (tokenList[tIdx].type == addsym) {
				getNextToken();
				term(tokenList);
				emit(2, 0, 2);
			}
			else {
				getNextToken();
				term(tokenList);
				emit(2, 0, 3);
			}
		}
	}
	if (tokenList[tIdx].type == lparensym || tokenList[tIdx].type == identsym || tokenList[tIdx].type == numbersym || tokenList[tIdx].type == oddsym) {
		printparseerror(17);
	}
}

void term(lexeme* tokenList) { //uses provided grammar to check term multiplication, division, or mod operator
	factor(tokenList);
	while (tokenList[tIdx].type == multsym || tokenList[tIdx].type == divsym || tokenList[tIdx].type == modsym) {
		if (tokenList[tIdx].type == multsym) {
			getNextToken();
			factor(tokenList);
			emit(2, 0, 4);
		}
		else if (tokenList[tIdx].type == divsym) {
			getNextToken();
			factor(tokenList);
			emit(2, 0, 5);
		}
		else {
			getNextToken();
			factor(tokenList);
			emit(2, 0, 7);
		}
	}
}

void factor(lexeme* tokenList) { //checks and processes factor
	if (tokenList[tIdx].type == identsym)
	{
		int symIdx_var = findsymbol(tokenList[tIdx], 2); //gets symbols from table
		int symIdx_const = findsymbol(tokenList[tIdx], 1);

		if (symIdx_var == -1 && symIdx_const == -1) {
			if (findsymbol(tokenList[tIdx], 3) != -1) {
				printparseerror(11);
			}
			else {
				printparseerror(19);
			}
		}

		if (symIdx_var == -1) {
			emit(1, 0, table[symIdx_const].val);
		}
		else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level) {
			emit(3, level - table[symIdx_var].level, table[symIdx_var].addr);
		}
		else {
			emit(1, 0, table[symIdx_const].val);
		}
		getNextToken();
	}
	else if (tokenList[tIdx].type == numbersym) {
		emit(1, 0, tokenList[tIdx].value);
		getNextToken();
	}
	else if (tokenList[tIdx].type == lparensym) {
		getNextToken();
		expression(tokenList);
		if (tokenList[tIdx].type != rparensym) {
			printparseerror(12);
		}
		getNextToken();
	}
	else {
		printparseerror(17);
	}
}

void getNextToken() //increases index on current token
{
	tIdx++;
}

/*This function does a linear search for the given name.
	An entry only matches if it has the correct name AND kind value AND is unmarked.
	Then it tries to maximize the level value */
int findsymbol(lexeme name, int kind)
{
		int i, max = -1;
		for(i = 0; i <= tIdx; i++) {
			if(strcmp(name.name, table[i].name)==0) { //compare names
					if(table[i].kind == kind && table[i].mark == 0) { //check kind and if unmarked
						max = i;
					}
			}
		}
		return max;
}

/*This function starts at the end of the table and works backward. It ignores marked entries.
	It looks at an entry’s level and if it is equal to the current level it marks that entry.
	It stops when it finds an unmarked entry whose level is less than the current level */
void mark()
{
	int i;

  // Traverse the symbols in reverse
  for (i = tIndex - 1; i >= 0; i--) {
		if(table[i].mark == 0 && table[i].level == level) { //check mark and level
				table[i].mark = 1; //mark if conditions met
			}
			else if(table[i].mark == 1 && table[i].level < level) {
				break;
			}
		}
}

/*This function should do a linear pass through the symbol table looking for the symbol name given.
	If it finds that name, it checks to see if it’s unmarked (no? keep searching).
	If it finds an unmarked instance, it checks the level.
	If the level is equal to the current level, it returns that index.
	Otherwise it keeps searching until it gets to the end of the table */
int multipledeclarationcheck(lexeme* tokenList)
{
	int i;
	for(i = 0; i <= tIndex; i++) {
		if(strcmp(table[i].name, tokenList[tIdx].name)==0) { //check names
			if(table[i].mark == 1) { //check mark
				if(table[i].level == level) { //if level is equal
					return i; //return that index
				}
			}
		}
	}
	return -1; //if nothing is found return -1
}
