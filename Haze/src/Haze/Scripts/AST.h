#pragma once

#include "Expression.h"

namespace INL 
{

	class AST {
		private:
			std::size_t Index;

			std::vector<std::shared_ptr<Token>> Tokens;
			std::vector<std::shared_ptr<ASTExpression>> Expressions;

		private:
			bool Eof() {
				return Index >= Tokens.size();
			}

			std::shared_ptr<Token> Current() {
				return Tokens[Index];
			}

			std::shared_ptr<Token> Peek(std::size_t position = 1) {
				if (Index + position < Tokens.size()) return Tokens[Index + position];
				else return nullptr;
			}

			bool Advance() {
				return ++Index < Tokens.size();
			}

			std::shared_ptr<ASTExpression> BuildExpression() {
				std::shared_ptr<Token> root = Current();
				std::shared_ptr<ASTExpression> expr = nullptr;

				switch (root->Type) {
					case LEFT_BRACE: return BuildGroupExpression<GroupBraceExpression>(BRACE, RIGHT_BRACE);
					case LEFT_PAREN: return BuildGroupExpression<GroupParenExpression>(PAREN, RIGHT_PAREN);
					case LEFT_SQUARE_BRACE: return BuildGroupExpression<GroupSquareBraceExpression>(SQUARE_BRACE, RIGHT_SQUARE_BRACE);
					case IDENTIFIER: if (Peek()->Type == DOT && Peek(2)->Type == IDENTIFIER) return BuildIdentifierGroupExpression();
				}

				return std::make_shared<GenericExpression>(root);
			}

			std::shared_ptr<ASTExpression> BuildIdentifierGroupExpression() {
				auto expr = std::make_shared<IdentifierGroupExpression>(Current());

				while (Peek()->Type == DOT && Peek(2)->Type == IDENTIFIER && Advance() && Advance()) {
					expr->Expressions.push_back(std::make_shared<GenericExpression>(Current()));
				}

				return expr;
			}

			template <typename T> std::shared_ptr<ASTExpression> BuildGroupExpression(TokenType groupType, TokenType nstop) {
				auto expr = std::make_shared<T>(std::make_shared<Token>(groupType, ""));
				while (Advance() && Current()->Type != nstop) {
					expr->Expressions.push_back(BuildExpression());
				}

				return expr;
			}

		public:
			AST(const std::vector<std::shared_ptr<Token>>& tokens) : Tokens(tokens), Index(0) {
				while (!Eof()) {
					Expressions.push_back(BuildExpression());
					Advance();
				}
			}

			std::vector<std::shared_ptr<ASTExpression>> Get() {
				return Expressions;
			}
	};

}