#include <stdlib.h>
#include <stdio.h>

#include "Lexxer.h"


#define INITIAL_TOKEN_STREAM_CAPACITY 128

static Token* TokenStream = NULL;
static int TokenCount = 0;
static int TokenCapacity = 0;

static int CurrentLineIndex = -1;
static int CurrentCharacterIndex = -1;

static int TotalFileLines = 0;


void PushToken(Token token) {
    if (TokenCount >= TokenCapacity) {
        TokenCapacity = TokenCount * 2;

        Token* temp = realloc(TokenStream,TokenCapacity * sizeof(Token));

        if (temp == NULL) { printf("Failed to re-allocate memory to Token Stream!\n"); exit(1); }
        TokenStream = temp;

    }
    TokenStream[TokenCount] = token;
    TokenCount++;
}

bool AdvanceLine(char Lines[MAX_LINES][MAX_LINE_LENGTH]) {
    CurrentLineIndex++;
    CurrentCharacterIndex = 0;

    if (CurrentLineIndex >= TotalFileLines) {
        CurrentLineIndex = TotalFileLines;
        CurrentCharacterIndex = -1;
        return false;
    }
    return true;
}

bool AdvanceCharacter(char Lines[MAX_LINES][MAX_LINE_LENGTH]) {
    if (CurrentLineIndex < 0 || CurrentLineIndex >= TotalFileLines) { 
        return false; 
    }

    CurrentCharacterIndex++;

    if (Lines[CurrentLineIndex][CurrentCharacterIndex] == '\0') { 
        return AdvanceLine(Lines); 
    }
    
    return true;
}

void FreeTokenStream(void) {
    if (TokenStream != NULL) { free(TokenStream); TokenStream = NULL; }
    TokenCount = 0;
    TokenCapacity = 0;
}

void InitTokenStream(void) {
    TokenCount = 0;
    TokenCapacity = INITIAL_TOKEN_STREAM_CAPACITY;

    if (TokenStream != NULL) { free(TokenStream); }

    TokenStream = malloc(TokenCapacity * sizeof(Token));

    if (TokenStream == NULL) { printf("Failed to allocate memory to Token Stream!\n"); exit(1); }
}



Token* GenerateTokenStream(char Lines[MAX_LINES][MAX_LINE_LENGTH], int TotalLines) {

    InitTokenStream();
    TotalFileLines = TotalLines;

    CurrentLineIndex = 0; CurrentCharacterIndex = 0;

    if (TotalFileLines == 0) { CurrentLineIndex = 0; CurrentCharacterIndex = -1; return TokenStream; }

    bool CanAdvanceCharacter = true;

    while (CanAdvanceCharacter) {
        char CurrentCharacter = Lines[CurrentLineIndex][CurrentCharacterIndex];
        printf("%c\n", CurrentCharacter);

        CanAdvanceCharacter = AdvanceCharacter(Lines);
    }
    return TokenStream;
}