#ifndef DIRECTIVESPECS_HPP
#define DIRECTIVESPECS_HPP

class StementSpecs
{
	public:
		int	allowedCtxts;
		int	min_args;
		int	max_args;

		StementSpecs	( void );
		StementSpecs	(int ctxt, int min, int max);
		StementSpecs	( const StementSpecs& src );
		StementSpecs&	operator= ( const StementSpecs& rhs );
		~StementSpecs	( void );

};

#endif // DIRECTIVESPECS_HPP
