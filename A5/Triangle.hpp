#pragma once

#include <glm/glm.hpp>
#include "utils.hpp"

class Triangle
{
    public:
    size_t v1;
	size_t v2;
	size_t v3;

  Triangle( size_t pv1, size_t pv2, size_t pv3 )
  : v1(pv1)
  , v2(pv2)
  , v3(pv3)
  {}

  
};

class MeshTriangle
{
    public:
	size_t v1;
	size_t v2;
	size_t v3;
	
	size_t uv1;
	size_t uv2;
	size_t uv3;

	size_t n1;
	size_t n2;
	size_t n3;

	MeshTriangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1(pv1)
  		, v2(pv2)
  		, v3(pv3)
		, uv1( 0 )
		, uv2( 0 )
		, uv3( 0 )
		, n1( 0 )
		, n2( 0 )
		, n3( 0 )
	{}

	MeshTriangle( 
		size_t pv1, size_t pv2, size_t pv3,
		size_t tv1, size_t tv2, size_t tv3
	 )
		: v1(pv1)
  		, v2(pv2)
  		, v3(pv3)
		, uv1( tv1 )
		, uv2( tv2 )
		, uv3( tv3 )
		, n1( 0 )
		, n2( 0 )
		, n3( 0 )
	{}

	MeshTriangle(size_t v[3], size_t t[3], size_t n[3])
		: v1(v[0])
  		, v2(v[1])
  		, v3(v[2])
		, uv1( t[0] )
		, uv2( t[1] )
		, uv3( t[2] )
		, n1( n[0] )
		, n2( n[1] )
		, n3( n[2] )
	{}
};