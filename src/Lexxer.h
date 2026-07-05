#include <stdbool.h>

#define MAX_LINES 256
#define MAX_LINE_LENGTH 512

typedef enum {
    TOKEN_SYMBOL = 0,
    TOKEN_STRING_LITERAL,
    TOKEN_IDENTIFIER,
    TOKEN_SYNTAX,
    TOKEN_DATATYPE,
    TOKEN_INTEGER_LITERAL,
    TOKEN_EMPTY
} TokenType;

typedef struct {
    TokenType Type;
    const char* Value;
    int Line;
} Token;


void PushToken(Token token);
bool AdvanceLine();
bool AdvanceCharacter(char Lines[MAX_LINES][MAX_LINE_LENGTH]);

void FreeTokenStream(void);
void InitTokenStream(void);

void DisplayTokenStream(void);
Token* GenerateTokenStream(char Lines[MAX_LINES][MAX_LINE_LENGTH], int TotalLines, int* TokenCount);


