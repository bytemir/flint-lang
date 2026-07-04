#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lexxer.h"

#define MAX_LINES 256
#define MAX_LINE_LENGTH 512


int main(void) {
    char TestCase[] = "tests/Example.flint";

    FILE *File = fopen(TestCase, "r");
    
    if (File== NULL) { perror("Error opening file!\n"); return 1; }

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

    Token* TokenStream = GenerateTokenStream(Lines, TotalLines);

    FreeTokenStream();
    return 0;
}