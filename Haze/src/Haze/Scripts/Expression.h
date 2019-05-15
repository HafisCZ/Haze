#pragma once

#include <vector>

#include "Token.h"

#include "Haze/Log.h"

namespace INL 
{

	class ASTExpression {
		public:
			std::shared_ptr<Token> Token;

		public:
			ASTExpression(std::shared_ptr<INL::Token> token) : Token(token) {}

			virtual void Evaluate() = 0;
	};

	class ASTGroupExpression : public ASTExpression {
		public:
			std::vector<std::shared_ptr<ASTExpression>> Expressions;

		public:
			ASTGroupExpression(std::shared_ptr<INL::Token> token) : ASTExpression(token) {}
	};

	class GenericExpression : public ASTExpression {
		public:
			GenericExpression(std::shared_ptr<INL::Token> token) : ASTExpression(token) {}

			virtual void Evaluate() override {
				HZ_CORE_TRACE("{0}: {1}", ToString(Token->Type), Token->Value);
			}
	};

	class BinaryExpression : public ASTExpression {
		public:
			std::shared_ptr<ASTExpression> ExpressionL;
			std::shared_ptr<ASTExpression> ExpressionR;

		public:
			BinaryExpression(std::shared_ptr<INL::Token> token) : ASTExpression(token) {}

			virtual void Evaluate() override {
				if (ExpressionL) ExpressionL->Evaluate();
				HZ_CORE_WARN("[OPERATOR, {0}]", Token);
				if (ExpressionR) ExpressionR->Evaluate();
			}
	};

	class GroupBraceExpression : public ASTGroupExpression {
		public:
			GroupBraceExpression(std::shared_ptr<INL::Token> token) : ASTGroupExpression(token) {}

			virtual void Evaluate() override {
				HZ_CORE_WARN("[GROUP] BRACE BEGIN");

				for (auto expression : Expressions) {
					expression->Evaluate();
				}

				HZ_CORE_WARN("[GROUP] BRACE END");
		}
	};

	class IdentifierGroupExpression : public ASTGroupExpression {
		public:
			IdentifierGroupExpression(std::shared_ptr<INL::Token> token) : ASTGroupExpression(token) {}

			virtual void Evaluate() override {
				HZ_CORE_WARN("[EXPR] {0}\n[ARGS:]", Token->Value);
				for (auto expr : Expressions) expr->Evaluate();
			}
	};

	class GroupParenExpression : public ASTGroupExpression {
		public:
			GroupParenExpression(std::shared_ptr<INL::Token> token) : ASTGroupExpression(token) {}

			virtual void Evaluate() override {
				HZ_CORE_WARN("[GROUP] PAREN BEGIN");

				for (auto expression : Expressions) {
					expression->Evaluate();
				}

				HZ_CORE_WARN("[GROUP] PAREN END");
			}
	};

	class GroupSquareBraceExpression : public ASTGroupExpression {
		public:
			GroupSquareBraceExpression(std::shared_ptr<INL::Token> token) : ASTGroupExpression(token) {}

			virtual void Evaluate() override {
				HZ_CORE_WARN("[GROUP] SQ BRACE BEGIN");

				for (auto expression : Expressions) {
					expression->Evaluate();
				}

				HZ_CORE_WARN("[GROUP] SQ BRACE END");
			}
	};


}