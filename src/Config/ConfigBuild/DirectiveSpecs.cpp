#include "DirectiveSpecs.hpp"

#include <iostream>

DirectiveSpecs::DirectiveSpecs( void )
	: allowedCtxts(0)
	, min_args(0)
	, max_args(0)
{
}

DirectiveSpecs::DirectiveSpecs( const DirectiveSpecs& src )
	: allowedCtxts(src.allowedCtxts)
	, min_args(src.min_args)
	, max_args(src.max_args)
{
}

DirectiveSpecs::DirectiveSpecs	(int ctxt, int min, int max)
	: allowedCtxts(ctxt)
	, min_args(min)
	, max_args(max)
{
}

DirectiveSpecs&	DirectiveSpecs::operator=( const DirectiveSpecs& rhs )
{
	if (this != &rhs) {
		allowedCtxts = rhs.allowedCtxts;
		min_args = rhs.min_args;
		max_args = rhs.max_args;
	}
	return (*this);
}

DirectiveSpecs::~DirectiveSpecs( void )
{
}
