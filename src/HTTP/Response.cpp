#include "Response.hpp"
	
Response::Response() {}


Response::Response(std::string const& Response)
{
	(void) Response;
}

Response::~Response() {
	std::cout << "Destructor called: Response" << std::endl; }
