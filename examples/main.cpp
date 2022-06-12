#include "lexer/lexer.hpp"

#include <fstream>
#include <iostream>

int main()
{
	// Open scripts/script.tds
	std::ifstream script("C:/Data/Acads/Programming/C++/threeD/examples/scripts/script.tds");

	// If script.tds could not be opened, exit
	if (!script.is_open())
	{
		std::cout << "Could not open script.tds" << std::endl;
		return 1;
	}

	// Create a lexer
	threeD::Lexer::Lexer lexer(script, "C:/Data/Acads/Programming/C++/threeD/examples/scripts/script.tds");

	for (auto token = lexer.nextToken(); token.type != threeD::Lexer::TokenType::EOF_; token = lexer.nextToken())
	{
		// Print the token
		std::cout << token.type << " " << token.lexeme << std::endl;
	}
}