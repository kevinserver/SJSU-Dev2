#include "uart0.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE( "Uart0 test", "[uart0]" )
{
	SECTION( "Summation test 0" )
	{
		  CHECK( uart0_return_true() == true );
	}
}
