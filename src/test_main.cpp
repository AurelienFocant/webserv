#include "HTTPTokenizer.hpp"
#include "Request.hpp"

int main(int argc, char **argv) {
	std::string	request;
	if (argc == 1)
//		request = "GET / HTTP/1.1\r\n EGHAHAH:eagashasehsehs\r\nLOKOLOK:8776876, 9875453434 \r\n\r\n";
//		request = "GET / HTTP/1.1\r\n\r\n";
//		request = "GET / HTTP/1.1\n\r\n";
//		request = "GET / HTTP/1.1\r\n";
//		request = "GET / HTTP/1.1";
//		request = "HEAD / HTTP/1.1";
//		request = "SHLA / HTTP/1.1";
//		request = "GET / HTTP/1.1\r\n EGHAHAH:eagashasehsehs\r\nLOKOLOK:8776876, 9875453434 \r\n\r\nseagasf.sahsrhsahashhshah:";
		request = "POST / HTTP/1.1\r\n LOKOLOK:8776876, 9875453434 \r\n\r\n";
	else if (argc == 2)
		request.assign(argv[1]);
	else
		std::cout << "Too many arguments" << std::endl;
	Request	message(request);
	/*for (std::vector<t_Token>::const_iterator it = token_list.begin(); it != token_list.end(); it++) {
		std::cout << Tokenizer::getTokenType(*it) << '\t' << (*it)._lexeme << std::endl;
	}
	std::cout << "----------------------------------------" << std::endl;
	*/

	std::cout << message << std::endl;
	return (0);
}
