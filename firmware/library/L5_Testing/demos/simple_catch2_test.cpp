#include "L5-testing/testing-frameworks.hpp"
namespace simple_catch2_test 
{
	int sum(int a, int b)
	{
		return a + b;
	}
}

TEST_CASE( "Simple Catch2 Unit test", "[simple_catch2_test]" )
{
	SECTION( "Summation test 0" )
	{
		  CHECK( simple_catch2_test::sum(2,2) == 4 );
		REQUIRE( simple_catch2_test::sum(5,5) != 15 );
	}
}