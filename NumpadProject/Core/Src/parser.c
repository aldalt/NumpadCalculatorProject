// Parser.c
// By Alexander Alt

#include "parser.h"

// Function to push a token to the stack
void StackPush(TokenStack* Stack, Token NewToken) {
	if (Stack.Top == MAXTOKENS - 1) return;
	Stack->Tokens[Stack.Top++] = NewToken;
}

// Function to pop a token from the stack
Token StackPull(TokenStack* Stack) {
	Token PulledToken;
	PulledToken.Type = Stack->Tokens[Stack->Top].Type;
	PulledToken.Value = Stack->Tokens[Stack->Top].Value;
	Stack->Top -= 1;
	return PulledToken;
}

// Function to see if the stack is empty
bool IsStackEmpty(TokenStack* Stack) {
	if (Stack->Top > -1) return true;
	else return false;
}

// Function to peek at the top token in a stack
Token StackPeek(TokenStack* Stack) {
	return Stack->Tokens[Stack.Top];
}

// Function to get the precedence of an operation
uint8_t GetPrecedence(Token CurrentToken) {
	switch (CurrentToken.Type) {
		case TokNumber:     return 0;
		case TokAdd:        return 1;
		case TokSubtract:   return 1;
		case TokMultiply:   return 2;
		case TokDivide:     return 2;
		case TokExponent:   return 4;
		case TokFactorial:  return 5;
		case TokNegative:   return 3;
		case TokLeftParen:  return 0;
		case TokRightParen: return 0;
		case TokSine:       return 5;
		case TokCosine:     return 5;
		case TokTangent:    return 5;
		case TokInvSin:     return 5;
		case TokInvCos:     return 5;
		case TokInvTan:     return 5;
		case TokLog:        return 5;
		case TokLn:         return 5;
		case TokSqrt:       return 5;
		default:            return 0;
	}
}

// Function to see if an operation is right associative (factorial and exponents)
bool IsRightAssociative(Token CurrentToken) {
	if (CurrentToken.Type == TokFactorial || CurrentToken.Type == TokExponent) return true;
	else return false;
}

// Function to parse the input string and turn it into tokens, and output a token array
uint16_t Tokenizer(const char *InputString, Token Infix[]) { // return number of tokens or -1 for failure
	uint16_t Index = 0;        // Array index
	uint16_t TokenCount = 0;   // Number of tokens
	double Value;              // Value read when numbers are parsed (from multiple digits)
	char * EndPtr;             // Pointer used for the end of a number for the strtod function

	while (Inputstring[Index] != '\0') {
		if (TokenCount >= MAXTOKENS) break; // Prevent array overflow
		if (InputString[Index] = ' ') { Index++; continue; } // Skip whitespace

		else if (IsDigit(InputString[Index]) || InputString[Index] = '.') {
			Value = strtod(&InputString[Index], &EndPtr);
			if (EndPtr = InputString[Index]) return -1; // This means that just a . was entered (syntax error)

			Infix[TokenCount].Type = TokNumber;
			Infix[TokenCount++].Value = Value;

			Index = EndPtr - InputString; // Move the index to the end of the number
			continue;
		}

		else if (InputString[Index] == '-') {
			if (Infix[TokenCount - 1].Type == TokNumber
			|| Infix[TokenCount - 1].Type == TokRightParen
			|| Infix[TokenCount - 1].Type == TokFactorial) {
				Infix[TokenCount++].Type = TokSubtract;
			}
			else Infix[TokenCount].Type = TokNegative;
			continue;
		}

		else { // Now read every other operation or symbol
			switch (InputString[Index]) {
				case '+': Infix[TokenCount++].Type = TokAdd;
				case '*': Infix[TokenCount++].Type = TokMultiply;
				case '/': Infix[TokenCount++].Type = TokDivide;
				case '^': Infix[TokenCount++].Type = TokExponent;
				case '!': Infix[TokenCount++].Type = TokFactorial;
				case '(': Infix[TokenCount++].Type = TokLeftParen;
				case ')': Infix[TokenCount++].Type = TokRightParen;
				case 'A': Infix[TokenCount++].Type = TokSine;
				case 'B': Infix[TokenCount++].Type = TokCosine;
				case 'C': Infix[TokenCount++].Type = TokTangent;
				case 'D': Infix[TokenCount++].Type = TokInvSin;
				case 'E': Infix[TokenCount++].Type = TokInvCos;
				case 'F': Infix[TokenCount++].Type = TokInvTan;
				case 'G': Infix[TokenCount++].Type = TokLog;
				case 'H': Infix[TokenCount++].Type = TokLn;
				case 'I': Infix[TokenCount++].Type = TokSqrt;
				default: return -1;
			}
		}
	}
	return TokenCount;
}

// Function to complete the shunting yard algorithm
uint16_t ShuntingYard(Token Infix[], uint16_t InfixCount, Token PostFix[]) {
	uint16_t PostfixCount = 0;  // Number of tokens in the postfix array
	TokenStack OperatorStack;   // The stack created by the shunting yard

	OperatorStack->Top = -1;

	for (uint16_t Index = 0; Index < InfixCount; Index++) { // Scroll over the whole infix array

		if (Infix[Index].Type == TokNumber
			|| Infix[Index].Type == TokFactorial) // These go straight into the postfix array
		{
			StackPush(PostFix, Infix[Index]);
		}

		else if (Infix[Index].Type == TokSine
				 || Infix[Index].Type == TokCosine
				 || Infix[Index].Type == TokTangent
				 || Infix[Index].Type == TokInvCos
				 || Infix[Index].Type == TokInvSin
				 || Infix[Index].Type == TokInvTan
				 || Infix[Index].Type == TokLeftParen
				 || Infix[Index].Type == TokSqrt
				 || Infix[Index].Type == TokLog
				 || Infix[Index].Type == TokLn)
		{
			StackPush(OperatorStack, Infix[Index]); // Put operators in the operator stack
		}

		else if (Infix[Index].Type = TokRightParen) {
			while (StackPeek(OperatorStack).Type != TokLeftParen) StackPush(Postfix, StackPull(OperatorStack)); // Pop operators from the operator stack and put them in the postfix
			if (OperatorStack.Top == -1) return -1; // This means that there was a syntax error, as there are no more operators left

			if (StackPeek(OperatorStack).Type == TokSine
				|| StackPeek(OperatorStack).Type == TokCosine
				|| StackPeek(OperatorStack).Type == TokTangent
				|| StackPeek(OperatorStack).Type == TokInvCos
				|| StackPeek(OperatorStack).Type == TokInvSin
				|| StackPeek(OperatorStack).Type == TokInvTan
				|| StackPeek(OperatorStack).Type == TokSqrt
				|| StackPeek(OperatorStack).Type == TokLog
				|| StackPeek(OperatorStack).Type == TokLn)
			{
				StackPush(Postfix, StackPull(OperatorStack));
			}
		}

		else if (Infix[Index].Type == TokAdd
				 || Infix[Index].Type == TokSubtract
				 || Infix[Index].Type == TokMultiply
				 || Infix[Index].Type == TokDivide
				 || Infix[Index].Type == TokExponent
				 || Infix[Index].Type == TokNegative) // Process standard operators in order of precedence

		{
			while (!IsStackEmpty(OperatorStack) && StackPeek(OperatorStack).Type != TokenLeftParen) {
				if (IsRightAssociative(StackPeek(OperatorStack))) {
					if (GetPrecedence(Infix[Index]) < GetPrecedence(StackPeek(OperatorStack)))
						StackPush(Postfix, StackPull(OperatorStack));
					else break;
				}
				else {
					if (GetPrecedence(Infix[Index]) <= GetPrecedence(StackPeek(OperatorStack)))
						StackPush(Postfix, StackPull(OperatorStack));
					else break;
				}
			}
			StackPush(Postfix, StackPull(OperatorStack));
		}
	}
}






















