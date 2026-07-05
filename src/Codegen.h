#include <stdio.h>

#include "Lexxer.h"


#define MAX_VARIABLES 2048

typedef struct {
    const char* Name;
    const char* Type;
} Variable;

bool IsVariableDeclared(const char* name);
void RegisterVariable(const char* name, const char* type);

bool AdvanceTokenInterval(int Interval);
bool AdvanceToken(void);
Token PeekToken(int Offset);

void GenerateTranspile(Token* TokenStream, int TokenCount, FILE* OutputFile);