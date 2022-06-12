#include "lexer/token.hpp"

namespace threeD { namespace Lexer {
	std::ostream& operator<<(std::ostream& out, const TokenType& type)
	{
		switch (type)
		{
		case TokenType::EOF_:
			out << "EOF_";
			break;
		case TokenType::IDENTIFIER:
			out << "IDENTIFIER";
			break;
		case TokenType::DEF:
			out << "DEF";
			break;
		case TokenType::DEC:
			out << "DEC";
			break;
		case TokenType::LET:
			out << "LET";
			break;
		case TokenType::INT:
			out << "INT";
			break;
		case TokenType::RET:
			out << "RET";
			break;
		case TokenType::BOOL_LITERAL:
			out << "BOOL_LITERAL";
			break;
		case TokenType::INT_LITERAL:
			out << "INT_LITERAL";
			break;
		case TokenType::FLOAT_LITERAL:
			out << "FLOAT_LITERAL";
			break;
		case TokenType::CHAR_LITERAL:
			out << "CHAR_LITERAL";
			break;
		case TokenType::STR_LITERAL:
			out << "STR_LITERAL";
			break;
		case TokenType::ADD:
			out << "ADD";
			break;
		case TokenType::SUB:
			out << "SUB";
			break;
		case TokenType::MUL:
			out << "MUL";
			break;
		case TokenType::DIV:
			out << "DIV";
			break;
		case TokenType::MOD:
			out << "MOD";
			break;
		case TokenType::EQ:
			out << "EQ";
			break;
		case TokenType::NEQ:
			out << "NEQ";
			break;
		case TokenType::LT:
			out << "LT";
			break;
		case TokenType::LEQ:
			out << "LEQ";
			break;
		case TokenType::GT:
			out << "GT";
			break;
		case TokenType::GEQ:
			out << "GEQ";
			break;
		case TokenType::AND:
			out << "AND";
			break;
		case TokenType::OR:
			out << "OR";
			break;
		case TokenType::NOT:
			out << "NOT";
			break;
		case TokenType::ASSIGN:
			out << "ASSIGN";
			break;
		case TokenType::ADD_ASSIGN:
			out << "ADD_ASSIGN";
			break;
		case TokenType::SUB_ASSIGN:
			out << "SUB_ASSIGN";
			break;
		case TokenType::MUL_ASSIGN:
			out << "MUL_ASSIGN";
			break;
		case TokenType::DIV_ASSIGN:
			out << "DIV_ASSIGN";
			break;
		case TokenType::ARROW:
			out << "ARROW";
			break;
		case TokenType::LPAREN:
			out << "LPAREN";
			break;
		case TokenType::RPAREN:
			out << "RPAREN";
			break;
		case TokenType::LBRACE:
			out << "LBRACE";
			break;
		case TokenType::RBRACE:
			out << "RBRACE";
			break;
		case TokenType::LBRACKET:
			out << "LBRACKET";
			break;
		case TokenType::RBRACKET:
			out << "RBRACKET";
			break;
		case TokenType::COMMA:
			out << "COMMA";
			break;
		case TokenType::SEMICOLON:
			out << "SEMICOLON";
			break;
		case TokenType::QUESTION:
			out << "QUESTION";
			break;
		case TokenType::COLON:
			out << "COLON";
			break;
		default:
			out << "UNKNOWN";
			break;
		}

		return out;
	}
}}