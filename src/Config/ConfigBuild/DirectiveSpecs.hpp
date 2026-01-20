#ifndef DIRECTIVESPECS_HPP
#define DIRECTIVESPECS_HPP

class DirectiveSpecs
{
	public:
		int	allowedCtxts;
		int	min_args;
		int	max_args;

		DirectiveSpecs	( void );
		DirectiveSpecs	(int ctxt, int min, int max);
		DirectiveSpecs	( const DirectiveSpecs& src );
		DirectiveSpecs&	operator= ( const DirectiveSpecs& rhs );
		~DirectiveSpecs	( void );

};

#endif // DIRECTIVESPECS_HPP
