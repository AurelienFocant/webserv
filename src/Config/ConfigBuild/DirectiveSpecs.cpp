#include "DirectiveSpecs.hpp"

#include <iostream>

StementSpecs::StementSpecs( void )
	: allowedCtxts(0)
	, min_args(0)
	, max_args(0)
{
}

StementSpecs::StementSpecs( const StementSpecs& src )
	: allowedCtxts(src.allowedCtxts)
	, min_args(src.min_args)
	, max_args(src.max_args)
{
}

StementSpecs::StementSpecs	(int ctxt, int min, int max)
	: allowedCtxts(ctxt)
	, min_args(min)
	, max_args(max)
{
}

StementSpecs&	StementSpecs::operator=( const StementSpecs& rhs )
{
	if (this != &rhs) {
		allowedCtxts = rhs.allowedCtxts;
		min_args = rhs.min_args;
		max_args = rhs.max_args;
	}
	return (*this);
}

StementSpecs::~StementSpecs( void )
{
	std::cout << "DirectiveSpecs Object Destroyed" << std::endl;
}
