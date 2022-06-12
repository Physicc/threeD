#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>
#include <ostream>

namespace threeD { namespace Lexer {
	
	enum class TokenType {
		// EOF_ is a special token type that is used to indicate the end of the file
		EOF_,

		// IDENTIFIER is a token type that is used to represent identifiers
		IDENTIFIER, 	/* eg. answerToLifeUniverseAndEverything */

		// Keywords
		DEF, 			/* def */
		DEC, 			/* dec */
		LET, 			/* let */
		RET, 			/* ret */
		INT, 			/* int */

		// Literals
		BOOL_LITERAL, 	/* eg. true, false */
		INT_LITERAL, 	/* eg. 42, 0x2A, 0b101010, 0o52 */
		FLOAT_LITERAL,	/* eg. 3.14 */
		CHAR_LITERAL, 	/* eg. 'a' */
		STR_LITERAL, 	/* eg. "Answer to the Ultimate Question of Life, the Universe, and Everything" */

		// Operators

		// Arithmetic Operators
		ADD, 			/* + */
		SUB, 			/* - */
		MUL, 			/* * */
		DIV, 			/* / */
		MOD, 			/* % */

		// Logical Operators
		EQ, 			/* == */
		NEQ, 			/* != */
		LT, 			/* < */
		LEQ, 			/* <= */
		GT, 			/* > */
		GEQ, 			/* >= */

		// Binary Operators
		AND, 			/* && */
		OR, 			/* || */
		NOT, 			/* ! */


		// Special Operators
		ASSIGN, 		/* := */
		ADD_ASSIGN, 	/* += */
		SUB_ASSIGN, 	/* -= */
		MUL_ASSIGN, 	/* *= */
		DIV_ASSIGN, 	/* /= */
		ARROW, 			/* -> */

		// Punctuation
		LPAREN, 		/* ( */
		RPAREN, 		/* ) */
		LBRACE, 		/* { */
		RBRACE, 		/* } */
		LBRACKET, 		/* [ */
		RBRACKET, 		/* ] */
		COMMA, 			/* , */
		SEMICOLON, 		/* ; */
		QUESTION, 		/* ? */
		COLON, 			/* : */
	};

	std::ostream& operator<<(std::ostream& out, const TokenType& type);

	struct Token {
	public:
		TokenType type;
		std::string lexeme;
		std::string file;
		int line;
		int column;
		Token() = default;
		Token(TokenType type, std::string lexeme, std::string file, int line, int column)
			: type(type), lexeme(lexeme), file(file), line(line), column(column) {}

	};

}}

#endif // __TOKEN_H__