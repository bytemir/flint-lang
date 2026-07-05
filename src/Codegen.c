#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "Codegen.h"

static Token* TokenStream = NULL;

static Variable VariableTable[MAX_VARIABLES];
static int VariableCount = 0;

static int TotalTokenCount = 0;
static int CurrentTokenIndex = -1;

static FILE* OutputFile;

bool IsVariableDeclared(const char* name) {
    for (int i = 0; i < VariableCount; i++) {
        if (strcmp(VariableTable[i].Name, name) == 0) {
            return true;
        }
    }
    return false;
}

void RegisterVariable(const char* name, const char* type) {
    if (VariableCount >= MAX_VARIABLES) {
        printf("Compiler Error: Symbol Table overflow! Too many variables.\n");
        exit(1);
    }
    VariableTable[VariableCount].Name = name;
    VariableTable[VariableCount].Type = type;
    VariableCount++;
}

bool AdvanceTokenInterval(int Interval) {
    CurrentTokenIndex += Interval;
    if (CurrentTokenIndex >= TotalTokenCount) {
        CurrentTokenIndex = TotalTokenCount;
        return false;
    }
    return true;
}

bool AdvanceToken(void) {
    CurrentTokenIndex++;
    if (CurrentTokenIndex >= TotalTokenCount) {
        CurrentTokenIndex = TotalTokenCount;
        return false;
    }
    return true;
}

Token PeekToken(int Offset) {
    int TargetIndex = CurrentTokenIndex + Offset;

    if (TargetIndex < 0 || TargetIndex >= TotalTokenCount) {
        Token EmptyToken; EmptyToken.Type = 0; EmptyToken.Value = ""; EmptyToken.Line = -1;
        return EmptyToken;
    }

    return TokenStream[TargetIndex];
}

void GenerateTranspile(Token* pTokenStream, int pTokenCount, FILE* pOutputFile) {
    TokenStream = pTokenStream;
    TotalTokenCount = pTokenCount;
    OutputFile = pOutputFile;

    fprintf(OutputFile, "#include <stdio.h>\n");
    fprintf(OutputFile, "#include <string.h>\n");
    fprintf(OutputFile, "\nint main(void) {\n");

    CurrentTokenIndex = 0;
    
    if (TotalTokenCount == 0) return;

    bool CanAdvance = true;
    while (CanAdvance) {
        Token CurrentToken = TokenStream[CurrentTokenIndex];
        if (CurrentToken.Type == TOKEN_SYNTAX && strcmp(CurrentToken.Value, "input") == 0) 
        {
            Token LBracket = PeekToken(1);
            if (LBracket.Type != TOKEN_SYMBOL || strcmp(LBracket.Value, "(") != 0) {  
                printf("Error on line %d: Expected '(' after 'input'!\n", CurrentToken.Line); 
                exit(1); 
            }

            Token StringToken = PeekToken(2);
            if (StringToken.Type != TOKEN_STRING_LITERAL) {  
                printf("Error on line %d: Expected arg-1 for input to be type of string!\n", CurrentToken.Line); 
                exit(1); 
            }

            Token CommaToken = PeekToken(3);
            if (CommaToken.Type != TOKEN_SYMBOL || strcmp(CommaToken.Value, ",") != 0) {  
                printf("Error on line %d: Expected ',' after arg-1 for input(...).\n", CurrentToken.Line); 
                exit(1); 
            }

            Token StoreToken = PeekToken(4);
            if (StoreToken.Type != TOKEN_IDENTIFIER) {  
                printf("Error on line %d: Expected variable identifier to store input within.\n", CurrentToken.Line); 
                exit(1); 
            }
            
            if (!IsVariableDeclared(StoreToken.Value)) {
                printf("Error on line %d: Use of undeclared identifier '%s'!\n", StoreToken.Line, StoreToken.Value);
                exit(1);
            }

            Token RBracket = PeekToken(5);
            if (RBracket.Type != TOKEN_SYMBOL || strcmp(RBracket.Value, ")") != 0) {  
                printf("Error on line %d: Expected ')' after input arg-2!\n", CurrentToken.Line); 
                exit(1); 
            }

            fprintf(OutputFile, "scanf(\"%s\", &%s)", StringToken.Value, StoreToken.Value);

            CanAdvance = AdvanceTokenInterval(6); 
            continue;
        }

        if (CurrentToken.Type == TOKEN_SYNTAX && strcmp(CurrentToken.Value, "print") == 0) 
        {
            fprintf(OutputFile, "printf");
            CanAdvance = AdvanceTokenInterval(1);
            continue;
        }
        if (CurrentToken.Type == TOKEN_SYNTAX && strcmp(CurrentToken.Value, "var") == 0) {
            Token VariableType  = PeekToken(1);
            if (VariableType.Type != TOKEN_DATATYPE) { 
                printf("Error on line %d: Expected DataType after 'var'!\n", CurrentToken.Line); 
                exit(1); 
            }

            Token VariableName  = PeekToken(2);
            if (VariableName.Type != TOKEN_IDENTIFIER) { 
                printf("Error on line %d: Expected Identifier after data type!\n", VariableType.Line); 
                exit(1); 
            }
            
            if (IsVariableDeclared(VariableName.Value)) {
                printf("Error on line %d: Redefinition of variable '%s'!\n", VariableName.Line, VariableName.Value);
                exit(1);
            }
            
            Token VariableNext = PeekToken(3); 
            if (VariableNext.Type == TOKEN_SYMBOL && strcmp(VariableNext.Value, ";") == 0) {
                RegisterVariable(VariableName.Value, VariableType.Value);
                if (strcmp(VariableType.Value, "string") == 0) {
                    fprintf(OutputFile, "const char* %s;\n", VariableName.Value);
                } else if (strcmp(VariableType.Value, "int") == 0) {
                    fprintf(OutputFile, "int %s;\n", VariableName.Value);
                }

                CanAdvance = AdvanceTokenInterval(4);
                continue;
            }

            if (VariableNext.Type != TOKEN_SYMBOL || strcmp(VariableNext.Value, "=") != 0) {
                printf("Error on line %d: Expected '=' operator during variable initialization!\n", VariableName.Line);
                exit(1);
            }
            RegisterVariable(VariableName.Value, VariableType.Value);

            if (strcmp(VariableType.Value, "string") == 0) {
                Token FirstValue = PeekToken(4);
                Token NextOp     = PeekToken(5);

                if (FirstValue.Type == TOKEN_STRING_LITERAL && strcmp(NextOp.Value, "+") == 0) {
                    
                    fprintf(OutputFile, "const char* %s = \"", VariableName.Value);
                    
                    int LookAheadIndex = 4;
                    while (true) {
                        Token LitToken = PeekToken(LookAheadIndex);
                        Token OpToken  = PeekToken(LookAheadIndex + 1);

                        if (LitToken.Type == TOKEN_STRING_LITERAL) {
                            fprintf(OutputFile, "%s", LitToken.Value);
                        }

                        if (strcmp(OpToken.Value, "+") == 0) {
                            LookAheadIndex += 2;
                            continue;
                        } 
                        
                        if (strcmp(OpToken.Value, ";") == 0) {
                            fprintf(OutputFile, "\";\n");
                            LookAheadIndex += 2;
                            break;
                        }

                        fprintf(OutputFile, "\";\n");
                        break;
                    }

                    CanAdvance = AdvanceTokenInterval(LookAheadIndex);
                    continue;
                }

                fprintf(OutputFile, "const char* %s = ", VariableName.Value);
            } else if (strcmp(VariableType.Value, "int") == 0) {
                fprintf(OutputFile, "int %s = ", VariableName.Value);
            }

            CanAdvance = AdvanceTokenInterval(4);
            continue;
        } 


        if (CurrentToken.Type == TOKEN_IDENTIFIER) {
            if (!IsVariableDeclared(CurrentToken.Value)) {
                printf("Error on line %d: Use of undeclared identifier '%s'!\n", CurrentToken.Line, CurrentToken.Value);
                exit(1);
            }

            const char* VarType = "";
            for (int i = 0; i < VariableCount; i++) {
                if (strcmp(VariableTable[i].Name, CurrentToken.Value) == 0) {
                    VarType = VariableTable[i].Type;
                    break;
                }
            }

            Token NextOp = PeekToken(1);
            if (strcmp(VarType, "string") == 0 && strcmp(NextOp.Value, "=") == 0) {
                Token LeftSide  = PeekToken(2);
                Token MathOp    = PeekToken(3);
                Token RightSide = PeekToken(4);
                Token SemiColon = PeekToken(5);

                if (LeftSide.Type == TOKEN_STRING_LITERAL && strcmp(MathOp.Value, "+") == 0 && RightSide.Type == TOKEN_STRING_LITERAL) {
                    fprintf(OutputFile, "%s = \"%s%s\"", CurrentToken.Value, LeftSide.Value, RightSide.Value);
                    if (strcmp(SemiColon.Value, ";") == 0) {
                        fprintf(OutputFile, ";\n");
                    }
                    CanAdvance = AdvanceTokenInterval(6);
                    continue;
                }
            }
        }
        
        if (CurrentToken.Type == TOKEN_SYMBOL || 
            CurrentToken.Type == TOKEN_STRING_LITERAL || 
            CurrentToken.Type == TOKEN_INTEGER_LITERAL ||
            CurrentToken.Type == TOKEN_IDENTIFIER ||
            CurrentToken.Type == TOKEN_SYNTAX) {
            
            if (CurrentToken.Type == TOKEN_STRING_LITERAL) {
                fprintf(OutputFile, "\"%s\"", CurrentToken.Value);
            } else {
                fprintf(OutputFile, "%s", CurrentToken.Value);
            }

            if (strcmp(CurrentToken.Value, ";") == 0) {
                fprintf(OutputFile, "\n");
            } 
        }

        CanAdvance = AdvanceToken();
    }

    fprintf(OutputFile, "return 0;\n");
    fprintf(OutputFile, "}\n");
}