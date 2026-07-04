#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Lexxer.h"


#define INITIAL_TOKEN_STREAM_CAPACITY 128

static Token* TokenStream = NULL;
static int TokenCount = 0;
static int TokenCapacity = 0;

static int CurrentLineIndex = -1;
static int CurrentCharacterIndex = -1;

static int TotalFileLines = 0;

static const char* RecognisedSyntax[] = {
    "var",
    "print"
};

static const int RecognisedSyntaxCount = sizeof(RecognisedSyntax) / sizeof(char*);




static const char* RecognisedDataTypes[] = {
    "const"
    "string",
    "int"
};
static const int RecognisedDataTypesCount = sizeof(RecognisedDataTypes) / sizeof(char*);




static const char RecognisedChars[] = {
    '(', ')',
    '=',
    ';',
    '+', '-',
    '/', '*'
};
static const int RecognisedCharsCount = sizeof(RecognisedChars) / sizeof(char);


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

void DisplayTokenStream(void) {
    if (TokenStream == NULL || TokenCount == 0) { printf("Token Stream is empty.\n"); return; }

    const char* TokenTypeStrings[] = {
        "SYMBOL",
        "STRING_LITERAL",
        "IDENTIFIER",
        "SYNTAX",
        "DATATYPE",
        "INTEGER_LITERAL"
    };

    printf("\n------------- Token Stream (%d total) -------------\n", TokenCount);
    printf("%-20s %-20s %-10s\n", "TOKEN TYPE", "VALUE", "LINE");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < TokenCount; i++) {
        Token t = TokenStream[i];
        const char* TypeString = TokenTypeStrings[t.Type];

        printf("%-20s \"%-18s\" Line %-10d\n", TypeString, t.Value, t.Line);
    }
    printf("--------------------------------------------------\n\n");
}

Token* GenerateTokenStream(char Lines[MAX_LINES][MAX_LINE_LENGTH], int TotalLines) {
    InitTokenStream();
    TotalFileLines = TotalLines;

    CurrentLineIndex = 0; CurrentCharacterIndex = 0;

    if (TotalFileLines == 0) { CurrentLineIndex = 0; CurrentCharacterIndex = -1; return TokenStream; }

    bool CanAdvanceCharacter = true;

    while (CanAdvanceCharacter) {
        char CurrentCharacter = Lines[CurrentLineIndex][CurrentCharacterIndex];

        if (CurrentCharacter == ' ' || CurrentCharacter == '\t' || CurrentCharacter == '\r') { 
            CanAdvanceCharacter = AdvanceCharacter(Lines); 
            continue; 
        }
        
        bool IsSymbolMatched = false;
        for (int i = 0; i < RecognisedCharsCount; i++) {
            if (CurrentCharacter == RecognisedChars[i]) {
                Token NewToken; 
                NewToken.Type = TOKEN_SYMBOL; 
                NewToken.Line = CurrentLineIndex + 1;
                
                char* CharacterString = malloc(2 * sizeof(char));
                CharacterString[0] = CurrentCharacter; 
                CharacterString[1] = '\0';
                NewToken.Value = CharacterString;
                
                PushToken(NewToken);
                CanAdvanceCharacter = AdvanceCharacter(Lines);
                IsSymbolMatched = true;
                break;
            }
        }
        if (IsSymbolMatched) { continue; }

        if (CurrentCharacter == '"') {
            char StringLiteralBuffer[MAX_LINE_LENGTH];
            int BufferIndex = 0;

            CanAdvanceCharacter = AdvanceCharacter(Lines);
            
            while (CanAdvanceCharacter && Lines[CurrentLineIndex][CurrentCharacterIndex] != '"') {
                StringLiteralBuffer[BufferIndex++] = Lines[CurrentLineIndex][CurrentCharacterIndex];
                CanAdvanceCharacter = AdvanceCharacter(Lines);
            }
            StringLiteralBuffer[BufferIndex] = '\0';

            Token NewToken; 
            NewToken.Line = CurrentLineIndex + 1; 
            NewToken.Type = TOKEN_STRING_LITERAL; 
            NewToken.Value = strdup(StringLiteralBuffer);
            
            PushToken(NewToken);
            CanAdvanceCharacter = AdvanceCharacter(Lines);
            continue;
        }

        if ((CurrentCharacter >= 'a' && CurrentCharacter <= 'z') || (CurrentCharacter >= 'A' && CurrentCharacter <= 'Z') || CurrentCharacter == '_') {
            char WordBuffer[MAX_LINE_LENGTH];
            int BufferIndex = 0;

            while (CanAdvanceCharacter && 
                  ((Lines[CurrentLineIndex][CurrentCharacterIndex] >= 'a' && Lines[CurrentLineIndex][CurrentCharacterIndex] <= 'z') ||
                   (Lines[CurrentLineIndex][CurrentCharacterIndex] >= 'A' && Lines[CurrentLineIndex][CurrentCharacterIndex] <= 'Z') ||
                   (Lines[CurrentLineIndex][CurrentCharacterIndex] >= '0' && Lines[CurrentLineIndex][CurrentCharacterIndex] <= '9') ||
                   Lines[CurrentLineIndex][CurrentCharacterIndex] == '_')) {
                
                WordBuffer[BufferIndex++] = Lines[CurrentLineIndex][CurrentCharacterIndex];
                CanAdvanceCharacter = AdvanceCharacter(Lines);
            }
            WordBuffer[BufferIndex] = '\0';

            Token NewToken;
            NewToken.Line = CurrentLineIndex + 1;
            NewToken.Value = strdup(WordBuffer);
            NewToken.Type = TOKEN_IDENTIFIER; 

            for (int i = 0; i < RecognisedSyntaxCount; i++) {
                if (strcmp(WordBuffer, RecognisedSyntax[i]) == 0) { 
                    NewToken.Type = TOKEN_SYNTAX; 
                    break; 
                }
            }

            for (int i = 0; i < RecognisedDataTypesCount; i++) { 
                if (strcmp(WordBuffer, RecognisedDataTypes[i]) == 0) { 
                    NewToken.Type = TOKEN_DATATYPE; 
                    break; 
                }
            }

            PushToken(NewToken);
            continue;
        }

        if (CurrentCharacter >= '0' && CurrentCharacter <= '9') {
            char NumBuffer[MAX_LINE_LENGTH];
            int BufferIndex = 0;

            while (CanAdvanceCharacter && 
                   Lines[CurrentLineIndex][CurrentCharacterIndex] >= '0' && 
                   Lines[CurrentLineIndex][CurrentCharacterIndex] <= '9') {
                NumBuffer[BufferIndex++] = Lines[CurrentLineIndex][CurrentCharacterIndex];
                CanAdvanceCharacter = AdvanceCharacter(Lines);
            }
            NumBuffer[BufferIndex] = '\0';

            Token NewToken;
            NewToken.Line = CurrentLineIndex + 1;
            NewToken.Type = TOKEN_INTEGER_LITERAL;
            NewToken.Value = strdup(NumBuffer);
            
            PushToken(NewToken);
            continue;
        }
        
        CanAdvanceCharacter = AdvanceCharacter(Lines);
    }

    DisplayTokenStream();
    return TokenStream;
}