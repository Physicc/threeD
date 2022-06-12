#ifndef __LEXER_H__
#define __LEXER_H__

#include "token.hpp"

#include <string>
#include <sstream>
#include <istream>

namespace threeD { namespace Lexer {

	class Lexer
	{
	public:
		Lexer(std::istream& buffer, std::string filename = "<source>");
		~Lexer() = default;
		Token nextToken();
		Token peekToken();

	private:
		std::istream& buffer;
		Token nextTokenFound;
		int line = 1;
		int column = 0;
		std::string lexeme;
		std::string filename;
		std::string curLineWhitespace;
		std::stringstream curLineStream;

		// Stores what has been seen so far
		enum class LexerState
		{
			START, 				/* Nothing seen so far */
			ALPHA_UNDERSCORE, 	/* Identifier */
			IDENTIFIER, 		/* Identifier */
			ZERO,				/* 0 (Prefix for DIGIT_PREFIX) */
			ZERO_PREFIX, 		/* Int Literal prefix (eg 0x, 0b, 0o) */
			INT_LITERAL, 		/* Int literal */
			SINGLE_QUOTE, 		/* ' */
			CHAR_ESCAPE, 		/* '\ */
			CHAR_LITERAL, 		/* eg. 'a, '\n */
			DOUBLE_QUOTE, 		/* " */
			STR_ESCAPE, 		/* " Hello \ */
			STR_LITERAL, 		/* " Hello \t W*/
			DIGITS,				/* Digits */
			FLOAT_LITERAL, 		/* Float literal */
			PUNCT,				/* Punctuation */
			LINE_COMMENT,		/* eg. // This is a line comment */
			BLOCK_COMMENT		/* eg. /* This is a block comment */
		} state = LexerState::START;

		template<LexerState state_>
		bool handleState(char nextChar);

		char getNextChar();
	};	

}}

#endif // __LEXER_H__