// $Id$

#include <cppunit/extensions/HelperMacros.h>

#include "ExpressionParser.h"

#include <stdlib.h>
#include <boost/lexical_cast.hpp>

class ExpressionParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ExpressionParserTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    inline stx::AnyScalar eval(const std::string &str)
    {
	std::auto_ptr<const stx::ParseNode> pn (stx::parseExpressionString(str));
	return pn->evaluate();
    }

    void test1()
    {
	using namespace stx;

	CPPUNIT_ASSERT( eval("5 + 4.5") == 9.5 );

	CPPUNIT_ASSERT( eval(" \"abc\" + \"def\" ") == "abcdef" );

	CPPUNIT_ASSERT( eval("5 * 6 + 7 * 2 - 2") == 42 );

	CPPUNIT_ASSERT( eval("((integer)(5 * 1.5 >= 1)) + 5") == 6 );

	CPPUNIT_ASSERT( eval("(integer) 5 * 1.5") == 7 );

	CPPUNIT_ASSERT( eval(" logn(exp(21)) + sqrt(pow(21,2)) == 42 ") == true );

	CPPUNIT_ASSERT( eval("10 > 5 AND 5.2 <= 42.2 OR 2 == 4") == true );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ExpressionParserTest );
