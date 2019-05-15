#pragma once

#include <string>

namespace INL 
{

	enum TokenType {
		LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, BRACE, PAREN, SQUARE_BRACE, 
		COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, COLON, SLASH_EQUAL, STAR_EQUAL, PLUS_EQUAL, MINUS_EQUAL,
		BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, RIGHT_SQUARE_BRACE, LEFT_SQUARE_BRACE,
		IDENTIFIER, STRING, NUMBER,
		AND, CLASS, ELSE, TFALSE, FUN, FOR, IF, TNULL, OR, PRINT, RETURN, SUPER, THIS, TTRUE, VAR, WHILE, TEOF
	};

	std::string ToString(TokenType type) {
		switch (type) {
			case LEFT_PAREN: return "LEFT_PAREN";
			case RIGHT_PAREN: return "RIGHT_PAREN";
			case LEFT_BRACE: return "LEFT_BRACE";
			case RIGHT_BRACE: return "RIGHT_BRACE";
			case BRACE: return "BRACE";
			case PAREN: return "PAREN";
			case SQUARE_BRACE: return "SQUARE_BRACE";
			case RIGHT_SQUARE_BRACE: return "RIGHT_SQUARE_BRACE";
			case LEFT_SQUARE_BRACE: return "LEFT_SQUARE_BRACE";
			case COMMA: return "COMMA";
			case DOT: return "DOT";
			case MINUS: return "MINUS";
			case PLUS: return "PLUS";
			case SEMICOLON: return "SEMICOLON";
			case SLASH: return "SLASH";
			case STAR: return "STAR";
			case SLASH_EQUAL: return "SLASH_EQUAL";
			case STAR_EQUAL: return "STAR_EQUAL";
			case PLUS_EQUAL: return "PLUS_EQUAL";
			case MINUS_EQUAL: return "MINUS_EQUAL";
			case COLON: return "COLON";
			case BANG: return "BANG";
			case BANG_EQUAL: return "BANG_EQUAL";
			case EQUAL: return "EQUAL";
			case EQUAL_EQUAL: return "EQUAL_EQUAL";
			case GREATER: return "GREATER";
			case GREATER_EQUAL: return "GREATER_EQUAL";
			case LESS: return "LESS";
			case LESS_EQUAL: return "LESS_EQUAL";
			case IDENTIFIER: return "IDENTIFIER";
			case STRING: return "STRING";
			case NUMBER: return "NUMBER";
			case AND: return "AND";
			case CLASS: return "CLASS";
			case ELSE: return "ELSE";
			case TFALSE: return "FALSE";
			case FUN: return "FUN";
			case FOR: return "FOR";
			case IF: return "IF";
			case TNULL: return "NULL";
			case OR: return "OR";
			case PRINT: return "PRINT";
			case RETURN: return "RETURN";
			case SUPER: return "SUPER";
			case THIS: return "THIS";
			case TTRUE: return "TRUE";
			case VAR: return "VAR";
			case WHILE: return "WHILE";
			case TEOF: return "EOF";
			default: return "ERROR TYPE";
		}
	}

	struct Token {
		const TokenType Type;
		const std::string Value;

		Token(TokenType type, const std::string value) : Type(type), Value(value) {}
	};

}