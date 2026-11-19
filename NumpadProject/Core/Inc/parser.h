// Parser.h
// By Alexander Alt

#ifndef __PARSER_H__
#define __PARSER_H__

#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "main.h"

#define MAXTOKENS 64

typedef enum {
	TokNumber,
	TokAdd,
	TokSubtract,
	TokMultiply,
	TokDivide,
	TokExponent,
	TokFactorial,
	TokNegative,
	TokLeftParen,
	TokRightParen,
	TokSine,
	TokCosine,
	TokTangent,
	TokInvSin,
	TokInvCos,
	TokInvTan,
	TokLog,
	TokLn,
	TokSqrt
} TokenType;

typedef struct {
	TokenType Type;
	double Value;
} Token; // Individual token element for the stack

typedef struct {
	Token Tokens[MAXTOKENS];
	uint16_t Top = -1;
} TokenStack; // The token stack, which is array based to save memory

#endif
