// Parser.c
// By Alexander Alt

#include "parser.h"

// Function to push a token to the stack
void StackPush(TokenStack* Stack, Token NewToken) {
	if (Stack->Top == MAXTOKENS - 1) return;
	Stack->Tokens[++Stack->Top] = NewToken;
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
	if (Stack->Top > -1) return false;
	else return true;
}

// Function to peek at the top token in a stack
Token StackPeek(TokenStack* Stack) {
	return Stack->Tokens[Stack->Top];
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
	if (CurrentToken.Type == TokNegative || CurrentToken.Type == TokExponent) return true;
	else return false;
}

// Function to calculate the factorial of a number
double Factorial(double Number) {
	double Result = 1;
	for (double Factor = 2.0; Factor <= Number; Factor++) Result *= Factor;
	return Result;
}

// Function to parse the input string and turn it into tokens, and output a token array
uint16_t Tokenizer(const char *InputString, Token Infix[]) { // return number of tokens or 0 for failure
	uint16_t Index = 0;        // Array index
	uint16_t TokenCount = 0;   // Number of tokens
	double Value;              // Value read when numbers are parsed (from multiple digits)
	char * EndPtr;             // Pointer used for the end of a number for the strtod function

	while (InputString[Index] != '\0') {
		if (TokenCount >= MAXTOKENS) break; // Prevent array overflow
		if (InputString[Index] == ' ') { Index++; continue; } // Skip whitespace

		else if (IsDigit(InputString[Index]) || InputString[Index] == '.') {
			Value = strtod(&InputString[Index], &EndPtr);
			if (EndPtr == InputString[Index]) return -1; // This means that just a . was entered (syntax error)

			Infix[TokenCount].Type = TokNumber;
			Infix[TokenCount++].Value = Value;

			Index = EndPtr - InputString; // Move the index to the end of the number
			continue;
		}

		else if (InputString[Index] == '-') {
			if (TokenCount > 0 && (Infix[TokenCount - 1].Type == TokNumber
				|| Infix[TokenCount - 1].Type == TokRightParen
				|| Infix[TokenCount - 1].Type == TokFactorial))
			{
				Infix[TokenCount++].Type = TokSubtract;
			}
			else Infix[TokenCount++].Type = TokNegative;
			Index++;
			continue;
		}

		// ADD CASES HERE FOR PI AND E!

		else { // Now read every other operation or symbol
			switch (InputString[Index]) {
				case '+':
					Infix[TokenCount++].Type = TokAdd;
					break;
				case '*':
					Infix[TokenCount++].Type = TokMultiply;
					break;
				case '/':
					Infix[TokenCount++].Type = TokDivide;
					break;
				case '^':
					Infix[TokenCount++].Type = TokExponent;
					break;
				case '!':
					Infix[TokenCount++].Type = TokFactorial;
					break;
				case '(':
					Infix[TokenCount++].Type = TokLeftParen;
					break;
				case ')':
					Infix[TokenCount++].Type = TokRightParen;
					break;
				case 'A':
					Infix[TokenCount++].Type = TokSine;
					break;
				case 'B':
					Infix[TokenCount++].Type = TokCosine;
					break;
				case 'C':
					Infix[TokenCount++].Type = TokTangent;
					break;
				case 'D':
					Infix[TokenCount++].Type = TokInvSin;
					break;
				case 'E':
					Infix[TokenCount++].Type = TokInvCos;
					break;
				case 'F':
					Infix[TokenCount++].Type = TokInvTan;
					break;
				case 'G':
					Infix[TokenCount++].Type = TokLog;
					break;
				case 'H':
					Infix[TokenCount++].Type = TokLn;
					break;
				case 'I':
					Infix[TokenCount++].Type = TokSqrt;
					break;
				default: return 0;
			}
		}
	}
	return TokenCount;
}

// Function to complete the shunting yard algorithm
uint16_t ShuntingYard(Token Infix[], uint16_t InfixCount, Token Postfix[]) {
	uint16_t PostfixCount = 0;  // Number of tokens in the postfix array
	TokenStack OperatorStack;   // The stack created by the shunting yard

	OperatorStack.Top = -1;

	for (uint16_t Index = 0; Index < InfixCount; Index++) { // Scroll over the whole infix array

		if (Infix[Index].Type == TokNumber
			|| Infix[Index].Type == TokFactorial) // These go straight into the postfix array
		{
			Postfix[PostfixCount++] = Infix[Index];
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
			StackPush(&OperatorStack, Infix[Index]); // Put operators in the operator stack
		}

		else if (Infix[Index].Type == TokRightParen) {
			while (!IsStackEmpty(&OperatorStack) && StackPeek(&OperatorStack).Type != TokLeftParen)
				Postfix[PostfixCount++] = StackPull(&OperatorStack); // Pop operators from the operator stack and put them in the postfix
			if (OperatorStack.Top == -1) return 0; // This means that there was a syntax error, as there are no more operators left
			OperatorStack.Top--;

			if (StackPeek(&OperatorStack).Type == TokSine
				|| StackPeek(&OperatorStack).Type == TokCosine
				|| StackPeek(&OperatorStack).Type == TokTangent
				|| StackPeek(&OperatorStack).Type == TokInvCos
				|| StackPeek(&OperatorStack).Type == TokInvSin
				|| StackPeek(&OperatorStack).Type == TokInvTan
				|| StackPeek(&OperatorStack).Type == TokSqrt
				|| StackPeek(&OperatorStack).Type == TokLog
				|| StackPeek(&OperatorStack).Type == TokLn)
			{
				Postfix[PostfixCount++] = StackPull(&OperatorStack);
			}
		}

		else if (Infix[Index].Type == TokAdd
				 || Infix[Index].Type == TokSubtract
				 || Infix[Index].Type == TokMultiply
				 || Infix[Index].Type == TokDivide
				 || Infix[Index].Type == TokExponent
				 || Infix[Index].Type == TokNegative) // Process standard operators in order of precedence
		{
			while (!IsStackEmpty(&OperatorStack) && StackPeek(&OperatorStack).Type != TokLeftParen) {
				if (IsRightAssociative(Infix[Index])) {
					if (GetPrecedence(Infix[Index]) < GetPrecedence(StackPeek(&OperatorStack)))
						Postfix[PostfixCount++] = StackPull(&OperatorStack);
					else break;
				}
				else {
					if (GetPrecedence(Infix[Index]) <= GetPrecedence(StackPeek(&OperatorStack)))
						Postfix[PostfixCount++] = StackPull(&OperatorStack);
					else break;
				}
			}
			StackPush(&OperatorStack, Infix[Index]);
		}
	}
	while (!IsStackEmpty(&OperatorStack)) { // Empty all remaining operators into the postfix array
		Token Operator = StackPull(&OperatorStack);
		if (Operator.Type == TokLeftParen) return 0;
		Postfix[PostfixCount++] = Operator;
	}
	return PostfixCount;
}

// --------------------------------------------------
// RETURN VALUES FOR EVALUATE (FOR ERROR TYPES):
// 0 - No errors, successful evaluation
// 1 - Syntax error
// 2 - Square root of a negative (imaginary answers)
// 3 - Divide by zero
// 4 - Log of a negative number (invalid)
// 5 - Overflow error (stack or double data type)
// 6 - Factorial of non-whole number
// 7 - Domain error (Trigonometry)
//---------------------------------------------------

// Function to actually evaluate the postfix notation array
uint8_t Evaluate(TokenStack Postfix[], bool DegreeMode, double * Result, uint16_t PostfixCount) {
	int16_t ValTop = -1;           // Top index of the values stack
	double ValueStack[MAXTOKENS];   // Stack of numbers for evaluation
	double CurrNum1 = 0;            // First number for binary operations/number for unary operations
	double CurrNum2 = 0;            // Second number for binary operations

	for (uint8_t Index = 0; Index < PostfixCount; Index++) { // Iterate over postfix array
		if (ValTop < MAXTOKENS - 1) { // Prevent stack overflow

			if (Postfix[Index].Type == TokNumber) // Put numbers straight into the number stack
				ValueStack[++ValTop] = Postfix[Index].Value;

			else if (Postfix[Index].Type == TokNegative
					 || Postfix[Index].Type == TokFactorial
					 || Postfix[Index].Type == TokSine
					 || Postfix[Index].Type == TokCosine
					 || Postfix[Index].Type == TokTangent
					 || Postfix[Index].Type == TokLn
					 || Postfix[Index].Type == TokLog
					 || Postfix[Index].Type == TokSqrt
					 || Postfix[Index].Type == TokInvSin
					 || Postfix[Index].Type == TokInvCos
					 || Postfix[Index].Type == TokInvTan) // Unary operators -> Take a number and complete operation
			{
				if (ValTop < 0) return 1; // This means improper syntax was entered

				CurrNum1 = ValueStack[ValTop--];
				switch (Postfix[Index].Type) {
					case TokFactorial:
						if (CurrNum1 < 0.0 || CurrNum1 > 170.0) return 5; // Can't take -x!, >170 overflows double type
						else if (floor(CurrNum1) != CurrNum1) return 6; // Number must be whole (no decimals)
						else ValueStack[++ValTop] = Factorial(CurrNum1);
						break;
					case TokSine:
						ValueStack[++ValTop] = DegreeMode ? sin(CurrNum1 * PI / 180.0) : sin(CurrNum1);
						break;
					case TokCosine:
						ValueStack[++ValTop] = DegreeMode ? cos(CurrNum1 * PI / 180.0) : cos(CurrNum1);
						break;
					case TokTangent:
						ValueStack[++ValTop] = DegreeMode ? tan(CurrNum1 * PI / 180.0) : tan(CurrNum1);
						break;
					case TokLn:
						if (CurrNum1 <= 0.0) return 4; // Can't take log of a negative number
						else ValueStack[++ValTop] = log(CurrNum1);
						break;
					case TokLog:
						if (CurrNum1 <= 0.0) return 4; // Can't take log of a negative number
						else ValueStack[++ValTop] = log10(CurrNum1);
						break;
					case TokSqrt:
						if (CurrNum1 < 0.0) return 2; // Can't take square root of a negative number
						else ValueStack[++ValTop] = sqrt(CurrNum1);
						break;
					case TokInvSin:
						if (CurrNum1 > 1.0 && CurrNum1 < -1.0) return 7;
						else ValueStack[++ValTop] = DegreeMode ? asin(CurrNum1) * 180.0 / PI : asin(CurrNum1);
						break;
					case TokInvCos:
						if (CurrNum1 > 1.0 && CurrNum1 < -1.0) return 7;
						else ValueStack[++ValTop] = DegreeMode ? acos(CurrNum1) * 180.0 / PI : acos(CurrNum1);
						break;
					case TokInvTan:
						ValueStack[++ValTop] = DegreeMode ? atan(CurrNum1) * 180.0 / PI : atan(CurrNum1);
						break;
					case TokNegative:
						ValueStack[++ValTop] = -CurrNum1;
						break;
					default: return 1;
				}
			}

			else if (Postfix[Index].Type == TokAdd
					 || Postfix[Index].Type == TokSubtract
					 || Postfix[Index].Type == TokMultiply
					 || Postfix[Index].Type == TokDivide
					 || Postfix[Index].Type == TokExponent)
			{
				if (ValTop < 1) return 1; // This means that there are less than two operands in the stack

				CurrNum2 = ValueStack[ValTop--];
				CurrNum1 = ValueStack[ValTop--];

				switch (Postfix[Index].Type) {
					case TokAdd:
						ValueStack[++ValTop] = CurrNum1 + CurrNum2;
						break;
					case TokSubtract:
						ValueStack[++ValTop] = CurrNum1 - CurrNum2;
						break;
					case TokMultiply:
						ValueStack[++ValTop] = CurrNum1 * CurrNum2;
						break;
					case TokDivide:
						if (CurrNum2 == 0) return 3; // Divide by zero error
						else ValueStack[++ValTop] = CurrNum1 / CurrNum2;
						break;
					case TokExponent:
						ValueStack[++ValTop] = pow(CurrNum1, CurrNum2);
						break;
					default: return 1;
				}
			}
		}
		else return 5; // This means stack overflow
	}
	if (ValTop > 0) return 1; // The whole equation should collapse to one number in the value stack
	*Result = ValueStack;
	return 0;
}


