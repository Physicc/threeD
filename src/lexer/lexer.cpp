#include "lexer/lexer.hpp"

#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

/*
 * Rules:
 *	1. Ident followed by punct/space -> Look for keyword and return keyword/identifier
 *	2. Punct followed by space/alphanum/underscore -> Lookup operator/punct and return
 *	3. Punct followed by punct -> Lookup operator/punct and return, if not, backtrack one char and lookup operator/punct and return
 *	3. Digits followed by punct/space -> return int literal
 */

namespace threeD { namespace Lexer {

	static const std::unordered_map<std::string, TokenType> keywords = {
		{"def", TokenType::DEF},
		{"dec", TokenType::DEC},
		{"let", TokenType::LET},
		{"ret", TokenType::RET},
		{"int", TokenType::INT},
		{"true", TokenType::BOOL_LITERAL},
		{"false", TokenType::BOOL_LITERAL},
	};

	static const std::unordered_map<std::string, TokenType> punctOps = {
		{"+", 	TokenType::ADD},
		{"-", 	TokenType::SUB},
		{"*", 	TokenType::MUL},
		{"/", 	TokenType::DIV},
		{"%", 	TokenType::MOD},
		{"==", 	TokenType::EQ},
		{"!=", 	TokenType::NEQ},
		{"<", 	TokenType::LT},
		{"<=", 	TokenType::LEQ},
		{">", 	TokenType::GT},
		{">=", 	TokenType::GEQ},
		{"&&", 	TokenType::AND},
		{"||", 	TokenType::OR},
		{"!", 	TokenType::NOT},
		{":=", 	TokenType::ASSIGN},
		{"+=", 	TokenType::ADD_ASSIGN},
		{"-=", 	TokenType::SUB_ASSIGN},
		{"*=", 	TokenType::MUL_ASSIGN},
		{"/=", 	TokenType::DIV_ASSIGN},
		{"->", 	TokenType::ARROW},
		{"(", 	TokenType::LPAREN},
		{")", 	TokenType::RPAREN},
		{"{", 	TokenType::LBRACE},
		{"}", 	TokenType::RBRACE},
		{",", 	TokenType::COMMA},
		{";", 	TokenType::SEMICOLON},
		{"?", 	TokenType::QUESTION},
		{":", 	TokenType::COLON}
	};

	std::unordered_set<char> escapeChars = {
		'\'', '\"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v', '0'
	};

	std::unordered_set<std::string> findPunctOpsPrefixes()
	{
		std::unordered_set<std::string> us;
		for (auto& pair : punctOps)
		{
			for (size_t i = 0; i <= pair.first.length(); i++)
			{
				us.insert(pair.first.substr(0, i));
			}
		}
		return us;
	}

	static const std::unordered_set<std::string> punctOpsPrefixes = findPunctOpsPrefixes();

	static void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
	{
		// Get the first occurrence
		size_t pos = data.find(toSearch);
		// Repeat till end is reached
		while( pos != std::string::npos)
		{
			// Replace this occurrence of Sub String
			data.replace(pos, toSearch.size(), replaceStr);
			// Get the next occurrence from the current position
			pos =data.find(toSearch, pos + replaceStr.size());
		}
	}

	Lexer::Lexer(std::istream& buffer, std::string filename) : buffer(buffer), filename(filename)
	{
		std::string curLine;
		std::getline(buffer, curLine);
		curLineStream << curLine;
	}

	template<Lexer::LexerState state>
	bool Lexer::handleState(char nextChar)
	{
		static_assert(false, "handleState not implemented for state");
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::START>(char nextChar)
	{
		lexeme.clear();
		lexeme += nextChar;
		if(nextChar == '\'')
		{
			state = LexerState::SINGLE_QUOTE;
		}
		if(nextChar == '\"')
		{
			state = LexerState::DOUBLE_QUOTE;
		}
		else if (ispunct(nextChar))
		{
			state = LexerState::PUNCT;
		}
		else if(isalpha(nextChar) || nextChar == '_')
		{
			state = LexerState::ALPHA_UNDERSCORE;
		}
		else if(nextChar == '0')
		{
			state = LexerState::ZERO;
		}
		else if(isdigit(nextChar))
		{
			state = LexerState::DIGITS;
		}
		else if(isspace(nextChar) || nextChar == EOF)
		{
			lexeme.clear();
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::ALPHA_UNDERSCORE>(char nextChar)
	{
		lexeme += nextChar;
		if (isalnum(nextChar) || nextChar == '_')
		{
			state = LexerState::IDENTIFIER;
		}
		else if(isspace(nextChar) || ispunct(nextChar) || nextChar == EOF)
		{
			state = LexerState::START;
			lexeme.pop_back();

			auto keyword = keywords.find(lexeme);
			if (keyword != keywords.end())
				nextTokenFound = {keyword->second, lexeme, filename, line, column};
			else
				nextTokenFound = {TokenType::IDENTIFIER, lexeme, filename, line, column};

			handleState<LexerState::START>(nextChar);
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::IDENTIFIER>(char nextChar)
	{
		lexeme += nextChar;
		if (isalnum(nextChar) || nextChar == '_')
		{
			state = LexerState::IDENTIFIER;
		}
		else if(isspace(nextChar) || ispunct(nextChar) || nextChar == EOF)
		{
			state = LexerState::START;
			lexeme.pop_back();

			auto keyword = keywords.find(lexeme);
			if (keyword != keywords.end())
				nextTokenFound = {keyword->second, lexeme, filename, line, column};
			else
				nextTokenFound = {TokenType::IDENTIFIER, lexeme, filename, line, column};

			handleState<LexerState::START>(nextChar);
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::ZERO>(char nextChar)
	{
		lexeme += nextChar;
		if (nextChar == 'x' || nextChar == 'X' || nextChar == 'b' || nextChar == 'B' || nextChar == 'o' || nextChar == 'O')
		{
			state = LexerState::ZERO_PREFIX;
		}
		else if(isdigit(nextChar))
		{
			state = LexerState::DIGITS;
		}
		else if(nextChar == '.')
		{
			state = LexerState::FLOAT_LITERAL;
		}
		else if(isspace(nextChar) || ispunct(nextChar) || nextChar == EOF)
		{
			state = LexerState::START;
			lexeme.pop_back();
			nextTokenFound = {TokenType::INT_LITERAL, lexeme, filename, line, column};
			handleState<LexerState::START>(nextChar);
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::ZERO_PREFIX>(char nextChar)
	{
		lexeme += nextChar;
		if (isdigit(nextChar))
		{
			state = LexerState::INT_LITERAL;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::INT_LITERAL>(char nextChar)
	{
		lexeme += nextChar;
		if (isdigit(nextChar))
		{
			state = LexerState::INT_LITERAL;
		}
		else if(isspace(nextChar) || (ispunct(nextChar) && nextChar != '.') || nextChar == EOF)
		{
			state = LexerState::START;
			lexeme.pop_back();

			nextTokenFound = {TokenType::INT_LITERAL, lexeme, filename, line, column};

			handleState<LexerState::START>(nextChar);
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::SINGLE_QUOTE>(char nextChar)
	{
		lexeme += nextChar;
		if (nextChar == '\\')
		{
			state = LexerState::CHAR_ESCAPE;
		}
		else if (nextChar == ' ' || isalnum(nextChar) )
		{
			state = LexerState::CHAR_LITERAL;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::CHAR_ESCAPE>(char nextChar)
	{
		lexeme += nextChar;
		if (escapeChars.find(nextChar) != escapeChars.end())
		{
			state = LexerState::CHAR_LITERAL;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::CHAR_LITERAL>(char nextChar)
	{
		lexeme += nextChar;
		if (nextChar == '\'')
		{
			state = LexerState::START;
			nextTokenFound = {TokenType::CHAR_LITERAL, lexeme, filename, line, column};
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::DOUBLE_QUOTE>(char nextChar)
	{
		lexeme += nextChar;
		if (nextChar == '\"')
		{
			state = LexerState::START;
			nextTokenFound = {TokenType::STR_LITERAL, lexeme, filename, line, column};
			return true;
		}
		if (nextChar == ' ' || isalnum(nextChar) || ispunct(nextChar))
		{
			state = LexerState::STR_LITERAL;
		}
		else if (nextChar == '\\')
		{
			state = LexerState::STR_ESCAPE;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::STR_ESCAPE>(char nextChar)
	{
		lexeme += nextChar;
		if (escapeChars.find(nextChar) != escapeChars.end())
		{
			state = LexerState::STR_LITERAL;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::STR_LITERAL>(char nextChar)
	{
		lexeme += nextChar;
		if (nextChar == '\\')
		{
			state = LexerState::STR_ESCAPE;
		}
		else if (nextChar == '\"')
		{
			state = LexerState::START;
			nextTokenFound = {TokenType::STR_LITERAL, lexeme, filename, line, column};
			return true;
		}
		else if (nextChar == ' ' || isalnum(nextChar) || ispunct(nextChar))
		{
			state = LexerState::STR_LITERAL;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::DIGITS>(char nextChar)
	{
		lexeme += nextChar;
		if (isdigit(nextChar))
		{
			state = LexerState::DIGITS;
		}
		else if(nextChar == '.')
		{
			state = LexerState::FLOAT_LITERAL;
		}
		else if(isspace(nextChar) || ispunct(nextChar) || nextChar == EOF)
		{
			state = LexerState::START;
			lexeme.pop_back();
			nextTokenFound = {TokenType::INT_LITERAL, lexeme, filename, line, column};
			handleState<LexerState::START>(nextChar);
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::FLOAT_LITERAL>(char nextChar)
	{
		lexeme += nextChar;
		if (isdigit(nextChar))
		{
			state = LexerState::FLOAT_LITERAL;
		}
		else if(isspace(nextChar) || ispunct(nextChar) || nextChar == EOF)
		{
			if (lexeme[lexeme.length() - 1] == '.')
			{
				lexeme.pop_back();

				auto curLine = curLineStream.str();
				findAndReplaceAll(curLine, "\t", "    ");
				findAndReplaceAll(curLineWhitespace, "\t", "    ");

				std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
				std::cerr << " " << line << "|" << curLine << std::endl;
				auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
				std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

				exit(1);
			}

			state = LexerState::START;
			lexeme.pop_back();
			nextTokenFound = {TokenType::FLOAT_LITERAL, lexeme, filename, line, column};
			handleState<LexerState::START>(nextChar);
			return true;
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::PUNCT>(char nextChar)
	{
		lexeme += nextChar;
		if (lexeme == "//")
		{
			state = LexerState::LINE_COMMENT;
		}
		else if (lexeme == "/*")
		{
			state = LexerState::BLOCK_COMMENT;
		}
		else if (ispunct(nextChar))
		{
			auto pre = punctOpsPrefixes.find(lexeme);
			if (pre != punctOpsPrefixes.end())
			{
				state = LexerState::PUNCT;
				return false;
			}

			lexeme.pop_back();
			auto op = punctOps.find(lexeme);
			if (op != punctOps.end())
			{
				state = LexerState::START;

				nextTokenFound = {op->second, lexeme, filename, line, column};

				handleState<LexerState::START>(nextChar);
				return true;
			}

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");
			
			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		else if(isspace(nextChar) || isalnum(nextChar) || nextChar == '_' || nextChar == EOF)
		{
			lexeme.pop_back();

			auto op = punctOps.find(lexeme);
			if (op != punctOps.end())
			{
				state = LexerState::START;

				nextTokenFound = {op->second, lexeme, filename, line, column};

				handleState<LexerState::START>(nextChar);
				return true;
			}
			else
			{
				auto curLine = curLineStream.str();
				findAndReplaceAll(curLine, "\t", "    ");
				findAndReplaceAll(curLineWhitespace, "\t", "    ");
				
				std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
				std::cerr << " " << line << "|" << curLine << std::endl;
				auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
				std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

				exit(1);
			}
		}
		else
		{
			lexeme.pop_back();

			auto curLine = curLineStream.str();
			findAndReplaceAll(curLine, "\t", "    ");
			findAndReplaceAll(curLineWhitespace, "\t", "    ");

			std::cerr << filename << ":" << line << ":" << column << ": " << "error: Unexpected character: '" << nextChar << "'" << std::endl;
			std::cerr << " " << line << "|" << curLine << std::endl;
			auto whitespaces = std::string(std::to_string(line).length() + 1, ' ');
			std::cout << curLineWhitespace << whitespaces << "^" << std::endl;

			exit(1);
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::LINE_COMMENT>(char nextChar)
	{
		if (nextChar == '\n' || nextChar == EOF)
		{
			state = LexerState::START;
			handleState<LexerState::START>(nextChar);
			return false;
		}
		else
		{
			return false;
		}
		return false;
	}

	template<>
	bool Lexer::handleState<Lexer::LexerState::BLOCK_COMMENT>(char nextChar)
	{
		if (nextChar == '*' && curLineStream.peek() == '/')
		{
			getNextChar();
			state = LexerState::START;
		}
		return false;
	}

	char Lexer::getNextChar()
	{
		column++;
		char nextChar = curLineStream.get();
		if (curLineStream.eof() && buffer.eof())
		{
			column = 0;
			curLineWhitespace.clear();
			std::stringstream().swap(curLineStream);
			return EOF;
		}
		else if (curLineStream.eof())
		{
			column = 0;
			line++;
			curLineWhitespace.clear();
			std::string curLine;
			std::getline(buffer, curLine);
			std::stringstream().swap(curLineStream);
			curLineStream << curLine;
			return '\n';
		}
		return nextChar;
	}

	Token Lexer::nextToken()
	{
		for (char nextChar = getNextChar();; nextChar = getNextChar())
		{
			if(isspace(nextChar))
				curLineWhitespace += nextChar;
			else
				curLineWhitespace += ' ';

			switch (state)
			{
			case LexerState::START:
				if (handleState<LexerState::START>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::ALPHA_UNDERSCORE:
				if (handleState<LexerState::ALPHA_UNDERSCORE>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::IDENTIFIER:
				if (handleState<LexerState::IDENTIFIER>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::ZERO:
				if (handleState<LexerState::ZERO>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::ZERO_PREFIX:
				if (handleState<LexerState::ZERO_PREFIX>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::INT_LITERAL:
				if (handleState<LexerState::INT_LITERAL>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::SINGLE_QUOTE:
				if (handleState<LexerState::SINGLE_QUOTE>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::CHAR_ESCAPE:
				if (handleState<LexerState::CHAR_ESCAPE>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::CHAR_LITERAL:
				if (handleState<LexerState::CHAR_LITERAL>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::DOUBLE_QUOTE:
				if (handleState<LexerState::DOUBLE_QUOTE>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::STR_ESCAPE:
				if (handleState<LexerState::STR_ESCAPE>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::STR_LITERAL:
				if (handleState<LexerState::STR_LITERAL>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::DIGITS:
				if (handleState<LexerState::DIGITS>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::FLOAT_LITERAL:
				if (handleState<LexerState::FLOAT_LITERAL>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::PUNCT:
				if (handleState<LexerState::PUNCT>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::LINE_COMMENT:
				if (handleState<LexerState::LINE_COMMENT>(nextChar))
					return nextTokenFound;
				break;
			case LexerState::BLOCK_COMMENT:
				if (handleState<LexerState::BLOCK_COMMENT>(nextChar))
					return nextTokenFound;
				break;
			default:
				break;
			}


			if (nextChar == EOF)
				break;
		}

		// Return EOF token
		nextTokenFound = {TokenType::EOF_, "", filename, line, column};
		return nextTokenFound;
	}


	Token Lexer::peekToken()
	{
		return nextTokenFound;
	}
}}
