// #include "../../includes/Parser.hpp"

// int main()
// {
//     // 1. Create a list of "Fake" network messages
//     std::string test_cases[] = {// valid
//                                 "TOPIC #help :We help with C++ IRC projects",
//                                 "TOPIC #strangers",
//                                 "PRIVMSG #help :Hello, I need help",
//                                 "PRIVMSG Bob :Can you review my code?",
//                                 "KICK #42 Enrico :for being too pretty",
//                                 "KICK #help smartass",
//                                 "INVITE little_rat #trap",
//                                 "JOIN #family",
//                                 "JOIN #project 123456",
//                                 "NICK irkalini",
//                                 "MODE #channel +i",
//                                 "MODE #channel -t",
//                                 "MODE #channel +o Alice",
//                                 "MODE #channel +k secret123",
//                                 "MODE #channel +l 12",
//                                 "USER guest tolmoon tolsun :Ronnie Reagan",
//                                 "PART #channel",
//                                 // invalid
//                                 ":prefix COMMAND arg1",
//                                 "PING :12345678",
//                                 "",
//                                 "TOPIC",
//                                 "TOPIC user",
//                                 "PRIVMSG",
//                                 "PRIVMSG bob :\tg",
//                                 "KICK #block",
//                                 "KICK 42 cat",
//                                 "INVITE everybody",
//                                 "INVITE irkalini 42",
//                                 "JOIN",
//                                 "JOIN 42",
//                                 "NICK !onely",
//                                 "NICK VerySmartDog",
//                                 "MODE #42 l 10",
//                                 "MODE #42 +f",
//                                 "MODE #42 +t Clara",
//                                 "MODE #42 -l 10",
//                                 "MODE #42 +k",
//                                 "MODE #42 o",
//                                 "USER user! host serv :Real name",
//                                 "USER user host serv",
//                                 "PART 42"};

//     // 2. Loop through them and test the Parser
//     for(size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++)
//     {
//         std::cout << "Testing " << i << ": [" << test_cases[i] << "]" << std::endl;

//         ParsedCommand result = Parser::parse(test_cases[i]);

//         std::cout << "  -> Command: " << result.command << std::endl;
//         for(size_t j = 0; j < result.args.size(); j++)
//         {
//             std::cout << "  -> Arg[" << j << "]: " << result.args[j] << std::endl;
//         }
// 		std::cout << "  -> err: " << result.err << std::endl;

//         std::cout << "--------------------------------" << std::endl;
//     }

//     return 0;
// }
