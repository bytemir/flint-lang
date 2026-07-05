#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Codegen.h"


#define MAX_LINES 256
#define MAX_LINE_LENGTH 512


int main(void) {
    char TestCase[] = "tests/Example.flint";
    char OutName[] = "out/Example.c";
    char ExecutableName[] = "out/Example";


    FILE *File = fopen(TestCase, "r");
    FILE *OutputFile = fopen(OutName, "w");

    if (File== NULL) { perror("Error opening source code file!\n"); return 1; }
    if (OutputFile== NULL) { perror("Error opening output C file!\n"); return 1; }

    char Lines[MAX_LINES][MAX_LINE_LENGTH];
    int TotalLines = 0;

    while (TotalLines < MAX_LINES && fgets(Lines[TotalLines], MAX_LINE_LENGTH, File) != NULL) {
        Lines[TotalLines][strcspn(Lines[TotalLines], "\n")] = '\0';
        TotalLines++;
    }

    fclose(File);
    printf("--------------------------------------------------\n");
    printf("Successfully read %d lines:\n", TotalLines);
    printf("--------------------------------------------------\n");
    for (int i = 0; i < TotalLines; i++) { printf("%i.  %s\n", i+1, Lines[i]); }

    int TokenCount = 0;
    Token* TokenStream = GenerateTokenStream(Lines, TotalLines, &TokenCount);

    GenerateTranspile(TokenStream, TokenCount, OutputFile);
    
    fclose(OutputFile);
    FreeTokenStream();

    char CompileCommand[512];
    sprintf(CompileCommand, "gcc %s -o %s", OutName, ExecutableName);
    int Result = system(CompileCommand);

    if (Result == 0) {
        printf("Compilation successful! Executable created at: %s\n", ExecutableName);


        printf("Program Output:\n\n");

        char RunCommand[512];
        #ifdef _WIN32
            sprintf(RunCommand, "out\\Example.exe");
        #else
            sprintf(RunCommand, "./out/Example");
        #endif

        system(RunCommand);
    } else {
        printf("GCC Compilation failed with exit code: %d\n", Result);
    }
    
    return 0;
}