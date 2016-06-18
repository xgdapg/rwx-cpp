#include "ast.h"
#include <iostream>
#include <stack>

namespace Lang {
	void throwException(Token* t, string e) {
		cout << "[" << t->row << "," << t->col << "] ";
		throw exception(e.c_str());
	}

	Token* AST::tk(int offset) {
		int i = index + offset;
		if (i >= 0 && i < (int)lex->tokens.size()) return lex->tokens[i];
		return Token::Empty;
	}

	void AST::parse() {
		root = parseBlock();
	}

	AST::Node* AST::parseBlock(string end/*="}"*/) {
		auto block = new Node(new Token(Token::Kind::kBlock, Token::Type::tUnknown, "Block", tk()->row, tk()->col));
		if (tk()->isOperator("{")) index += 1;

		while (tk() != Token::Empty) {
			auto t = tk();
			if (t->isKeyword("var")) {
				block->addChild(parseDeclVar());
				continue;
			}

			if (t->isKeyword("const")) {
				block->addChild(parseDeclConst());
				continue;
			}

			if (t->isKeyword("if")) {
				block->addChild(parseIfExpr());
				continue;
			}

			if (t->isKeyword("while")) {
				block->addChild(parseWhile());
				continue;
			}

			if (t->isOperator("=") || 
				t->isOperator("+=") || 
				t->isOperator("-=") || 
				t->isOperator("*=") || 
				t->isOperator("/=") || 
				t->isOperator("%=")) {
				parseAssign(block);
				continue;
			}

			if (t->isOperator("{")) {
				block->addChild(parseBlock());
				continue;
			}
			if (t->isOperator(end)) {
				index += 1;
				break;
			}
			//...
			auto n = parseExpression(true);
			if (n != nullptr) {
				block->addChild(n);
				continue;
			}

			throwException(t, "unmatched statement, got `" + t->value + "`");
		}
		return block;
	}

	AST::Node* AST::parsePrimaryExpr() {
		auto t = tk();
		if (t->isLiteral()) {
			index += 1;
			return new Node(t);
		}
		if (t->isIdentifier()) {
			if (tk(1)->isOperator("(")) {
				return parseFuncCall();
			}
			if (tk(1)->isOperator("[")) {
				return parseSubscript();
			}
			index += 1;
			return new Node(t);
		}
		if (t->isOperator("(")) {
			return parseParenExpr();
		}
		return nullptr;
	}

	AST::Node* AST::parseFuncCall() {
		auto name = tk();
		auto node = new Node(new Token(Token::Kind::kOperator, Token::Type::tFuncCall, "FuncCall", name->row, name->col));
		node->addChild(new Node(name));
		index += 2;

		while (!tk()->isOperator(")")) {
			node->addChild(parseExpression());
			if (tk()->isOperator(",")) { index += 1; continue; }
			if (tk()->isOperator(")")) break;
			throwException(tk(), "expect `,` or `)`, got `" + tk()->value + "`");
		}
		index += 1;
		return node;
	}

	AST::Node* AST::parseSubscript() {
		auto name = tk();
		auto node = new Node(new Token(Token::Kind::kOperator, Token::Type::tSubscript, "Subscript", name->row, name->col));
		node->addChild(new Node(name));

		index += 2;
		node->addChild(parseExpression());

		if (!tk()->isOperator("]")) {
			throwException(tk(), "expect `]`, got `" + tk()->value + "`");
		}
		index += 1;
		return node;
	}

	AST::Node* AST::parseParenExpr() {
		index += 1;
		auto node = parseExpression();

		if (!tk()->isOperator(")")) {
			throwException(tk(), "expect `)`, got `" + tk()->value + "`");
		}

		index += 1;
		return node;
	}

	AST::Node* AST::parseExpression(bool enableEmpty/*=false*/) {
		auto list = vector<Node*>();
		bool isOp = true;
		while (tk() != Token::Empty) {
			auto t = tk();
			if (t->isBinaryOperator() || t->isUnaryOperator()) {
				if (isOp && !t->convertToUnaryOperator()) throwException(t, "expect unary operator, got `" + t->value + "`");
				isOp = true;

				list.push_back(new Node(t));
				index += 1;
				continue;
			}
			isOp = false;

			auto expr = parsePrimaryExpr();
			if (expr != nullptr) {
				list.push_back(expr);
				continue;
			}
			break;
		}
		if (list.empty()) {
			if (enableEmpty) return nullptr;
			throwException(tk(), "expect expression, got `" + tk()->value + "`");
		}

		if (isOp) {
			throwException(list.back()->token, "incomplete expression, end with `" + list.back()->token->value + "`");
		}

		return buildTree(list, 0, list.size() - 1);
	}

	AST::Node * AST::buildTree(vector<Node*> list, int begin, int end) {
		int opi = -1;
		int priority = 99;
		for (auto i = begin; i <= end; i++) {
			if (!list[i]->children.empty()) continue;
			auto t = list[i]->token;
			if (t->isUnaryOperator()  && t->getPriority() <  priority ||
				t->isBinaryOperator() && t->getPriority() <= priority)
			{
				opi = i;
				priority = t->getPriority();
			}
		}
		if (opi != -1) {
			auto op = list[opi];
			if (op->token->isBinaryOperator()) {
				op->addChild(buildTree(list, begin, opi - 1));
				op->addChild(buildTree(list, opi + 1, end));
			} else if (op->token->isUnaryOperator()) {
				op->addChild(buildTree(list, opi + 1, end));
			} else {
				//error
				throwException(op->token, "unexpected operator `" + op->token->value + "`");
			}
			return op;
		}
		return list[begin];
	}

	AST::Node* AST::parseDeclVar() {
		auto node = new Node(tk());
		index += 1;

		auto name = tk();
		if (!name->isIdentifier()) {
			throwException(name, "expect identifier, got `" + name->value + "`");
		}

		node->addChild(new Node(name));
		index += 1;

		if (!(tk()->isOperator(":") && tk(1)->isIdentifier()) && !tk()->isOperator("=") && !tk()->isKeyword("in")) {
			throwException(name, "cannot determine the type of variable `" + name->value + "`");
		}

		if (tk()->isOperator(":") && tk(1)->isIdentifier()) {
			node->addChild(new Node(tk(1)));
			index += 2;
		}

		return node;
	}

	AST::Node* AST::parseDeclConst() {
		auto node = new Node(tk());
		index += 1;
		const string;
		auto name = tk();
		if (!name->isIdentifier()) {
			throwException(name, "expect identifier, got `" + name->value + "`");
		}

		node->addChild(new Node(name));
		index += 1;

		if (tk()->isOperator(":") && tk(1)->isIdentifier()) {
			node->addChild(new Node(tk(1)));
			index += 2;
		}

		if (!tk()->isOperator("=")) {
			throwException(name, "constant value required");
		}

		return node;
	}

	AST::Node* AST::parseIfExpr() {
		auto node = new Node(tk());
		index += 1;

		node->addChild(parseExpression());

		if (!tk()->isOperator("{")) {
			throwException(tk(), "expect `{`, got `" + tk()->value + "`");
		}

		node->addChild(parseBlock());
		
		if (tk()->isKeyword("else")) {
			index += 1;
			if (tk()->isKeyword("if")) {
				node->addChild(parseIfExpr());
			} else if (tk()->isOperator("{")) {
				node->addChild(parseBlock());
			} else {
				throwException(tk(), "expect `{` or `if`, got `" + tk()->value + "`");
			}
		}

		return node;
	}

	AST::Node* AST::parseAssign(Node* block) {
		if (block->children.empty()) throwException(tk(), "lvalue not found");

		auto lv = block->children.back();
		//if (lv->token->isLiteral()) throwException(tk(), "invalid lvalue");

		auto assign = new Node(tk());
		index += 1;

		assign->addChild(lv);
		lv->parent = assign;
		block->children[block->children.size() - 1] = assign;

		assign->addChild(parseExpression());

		return assign;
	}

	AST::Node* AST::parseWhile() {
		auto node = new Node(tk());
		index += 1;

		node->addChild(parseExpression());

		if (!tk()->isOperator("{")) {
			throwException(tk(), "expect `{`, got `" + tk()->value + "`");
		}

		node->addChild(parseBlock());

		return node;
	}

	
	

}