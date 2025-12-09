#include "HTTPTokenizer.hpp"
#include "Request.hpp"

int main() {
	std::string request = "GET / HTTP/1.1\r\n EGHAHAH:eagashasehsehs\r\nLOKOLOK:8776876, 9875453434 \r\n\r\n";
	Request	message(request);
	/*for (std::vector<t_Token>::const_iterator it = token_list.begin(); it != token_list.end(); it++) {
		std::cout << Tokenizer::getTokenType(*it) << '\t' << (*it)._lexeme << std::endl;
	}
	std::cout << "----------------------------------------" << std::endl;
	*/

	std::cout << message << std::endl;
	return (0);
}
