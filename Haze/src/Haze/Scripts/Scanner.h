#pragma once

#include <vector>
#include <unordered_map>
#include <cctype>

#include "Token.h"

namespace INL 
{

	class Scanner {
		private:
			int Start;
			int Current;
			int Line;

			const std::string& Source;

			std::vector<std::shared_ptr<Token>> Tokens;

		private:
			inline static std::unordered_map<std::string, TokenType>& Keywords() {
				static std::unordered_map<std::string, TokenType> keywords = {
					{ "and", AND },
					{ "class", CLASS },
					{ "else", ELSE },
					{ "false", TFALSE },
					{ "fun", FUN },
					{ "for", FOR },
					{ "if", IF },
					{ "null", TNULL },
					{ "or", OR },
					{ "print", PRINT },
					{ "return", RETURN },
					{ "super", SUPER },
					{ "this", THIS },
					{ "true", TTRUE },
					{ "var", VAR },
					{ "while", WHILE }
				};

				return keywords;
			}

			bool Eof() {
				return Current >= Source.size();
			}

			char Peek() {
				return Eof() ? '\0' : Source[Current];
			}

			char PeekNext() {
				return Current + 1 >= Source.size() ? '\0' : Source[Current + 1];
			}

			char Advance() {
				return Source[Current++];
			}

			bool Match(char c) {
				if (Eof() || Source[Current] != c) {
					return false;
				} else {
					Current++;
					return true;
				}
			}

			void Add(TokenType type) {
				Tokens.push_back(std::make_shared<Token>(type, ""));
			}

			void Read() {
				char c = Advance();

				switch (c) {
					case '(': Add(LEFT_PAREN); break;
					case ')': Add(RIGHT_PAREN); break;
					case '{': Add(LEFT_BRACE); break;
					case '}': Add(RIGHT_BRACE); break;
					case '[': Add(RIGHT_SQUARE_BRACE); break;
					case ']': Add(LEFT_SQUARE_BRACE); break;
					case '.': Add(DOT); break;
					case '-': Add(Match('=') ? MINUS_EQUAL : MINUS); break;
					case '+': Add(Match('=') ? PLUS_EQUAL : PLUS); break;
					case ';': Add(SEMICOLON); break;
					case '*': Add(Match('*') ? STAR_EQUAL : STAR); break;
					case ':': Add(COLON); break;
					case '!': Add(Match('=') ? BANG_EQUAL : BANG); break;
					case '=': Add(Match('=') ? EQUAL_EQUAL : EQUAL); break;
					case '<': Add(Match('=') ? LESS_EQUAL : LESS); break;
					case '>': Add(Match('=') ? GREATER_EQUAL : GREATER); break;
					case ' ':
					case '\r':
					case '\t':
						break;
					case '\n':
						Line++;
						break;
					case '/':
						if (Match('/')) while (Peek() != '\n' && !Eof()) Advance();
						else Add(Match('=') ? SLASH_EQUAL : SLASH);
						break;
					case '\'':
						ReadString();
						break;
					default:
						if (std::isdigit(c)) ReadNumber();
						else if (std::isalpha(c)) ReadIdentifier();
				}
			}

			void ReadString() {
				while (Peek() != '\'' && !Eof()) {
					if (Peek() == '\n') Line++;
					Advance();
				}

				if (!Eof()) {
					Advance();

					Tokens.push_back(std::make_shared<Token>(STRING, Source.substr(Start + 1, Current - Start - 2)));
				}
			}

			void ReadNumber() {
				while (std::isdigit(Peek())) Advance();

				if (Peek() == '.' && std::isdigit(PeekNext())) {
					Advance();

					while (std::isdigit(Peek())) Advance();
				}

				Tokens.push_back(std::make_shared<Token>(NUMBER, Source.substr(Start, Current - Start)));
			}

			void ReadIdentifier() {
				while (std::isalnum(Peek())) Advance();

				auto text = Source.substr(Start, Current - Start);
				auto it = Keywords().find(text);

				Tokens.push_back(std::make_shared<Token>(it == Keywords().end() ? IDENTIFIER : it->second, text));
			}

		public:
			Scanner(const std::string& source) : Source(source), Start(0), Current(0), Line(1) {
				while (!Eof()) {
					Start = Current;
					Read();
				}

				Tokens.push_back(std::make_shared<Token>(TEOF, ""));
			}

			std::vector<std::shared_ptr<Token>> Get() {
				return Tokens;
			}
	};

}