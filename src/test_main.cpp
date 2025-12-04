# include"Token.hpp"

int main() {
	std::string request = "GET / HTTP/1.1\r\n EGHAHAH:eagashasehsehs\r\nLOKOLOK:8776876 \r\n\r\n";
	Token&	token_list = Token::toTokenList(request);	
	return (0);
}
