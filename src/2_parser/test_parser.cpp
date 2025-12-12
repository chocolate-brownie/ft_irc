#include "../../includes/Parser.hpp"

int main() {
	// 1. Create a list of "Fake" network messages
	std::string test_cases[] = {
		"NICK Alice",               // Simple command
		"USER alice 0 * :Alice Wonderland", // Complex trailing argument
		"JOIN #42",                  // Standard command
		"PRIVMSG #help :Hello, I need help", // Message with spaces
		"PRIVMSG Bob :Can you review my code?",
		":prefix COMMAND arg1",      // Command with prefix (advanced) should give error
		"PART #channel :Goodbye",
		"PING :12345678",
		"MODE #channel +o Alice",
		"TOPIC #help :We help with C++ IRC projects",
		""                           // Empty string
	};

	// 2. Loop through them and test the Parser
	for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
		std::cout << "Testing " << i << ": [" << test_cases[i] << "]" << std::endl;

		try {
			ParsedCommand result = Parser::parse(test_cases[i]);

			std::cout << "  -> Command: " << result.command << std::endl;
			for (size_t j = 0; j < result.args.size(); j++) {
				std::cout << "  -> Arg[" << j << "]: " << result.args[j] << std::endl;
			}
		} catch (const std::exception& e) {
			std::cerr << "  -> Parse error: " << e.what() << std::endl;
		}

		std::cout << "--------------------------------" << std::endl;
	}

	return 0;
}
