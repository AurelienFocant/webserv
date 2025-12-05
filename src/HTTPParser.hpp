
#include "HeaderTokenizer.hpp"

std::vector<t_Token>	scanTokens() {
	std::vector	
	switch (peek()) {
		case (' '): 
			advance() ;
			break ;
		}
		case (':'):
			addToken(':');
			break ;
		}
		case ('\r'):
			advance();		
			if (peek() == '\n')
				addToken("\r\n");
			else
//				handle_error;
			advance();
			break ;
		}
		default:
			addToken(WORD);
		}
}

void	addToken(t_tokenType type, std::string& content) {
	_tkType = type;
	_lexeme = content;
}
