#include <stdbool.h>

#define MAX_LINES 256
#define MAX_LINE_LENGTH 512

typedef enum {
    SYNTAX,
    IDENTIFIER,
    NUMBER,
    END_OF_FILE,
} TokenType;

typedef struct {
    TokenType Type;
    const char* Value;
} Token;


void PushToken(Token token);
bool AdvanceLine(char Lines[MAX_LINES][MAX_LINE_LENGTH]);
bool AdvanceCharacter(char Lines[MAX_LINES][MAX_LINE_LENGTH]);

void FreeTokenStream(void);
void InitTokenStream(void);

Token* GenerateTokenStream(char Lines[MAX_LINES][MAX_LINE_LENGTH], int TotalLines);


