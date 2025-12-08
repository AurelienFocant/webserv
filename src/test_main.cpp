#include "HTTPTokenizer.hpp"
#include "Request.hpp"

int main() {
	std::string request = "GET / HTTP/1.1\r\n EGHAHAH:eagashasehsehs\r\nLOKOLOK:8776876 \r\n\r\n";
	HTTPTokenizer	traitment(request);
	std::vector<t_Token>	token_list = traitment.scanTokens();	
	for (std::vector<t_Token>::const_iterator it = token_list.begin(); it != token_list.end(); it++) {
		std::cout << Tokenizer::getTokenType(*it) << '\t' << (*it)._lexeme << std::endl;
	}
	std::cout << "----------------------------------------" << std::endl;
	Request	message(token_list);
	std::cout << message << std::endl;
	return (0);
}
