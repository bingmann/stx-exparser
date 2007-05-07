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

	// test reevaluation of the toString().
	{
	    std::string strexpr = pn->toString();
	    std::auto_ptr<const stx::ParseNode> pn2 (stx::parseExpressionString(strexpr));

	    CPPUNIT_ASSERT( strexpr == pn2->toString() );
	}

	stx::BasicSymbolTable bst;
	bst.setVariable("a", 42);
	bst.setVariable("e", 2.7183);

	return pn->evaluate(bst);
    }

    void test1()
    {
	using namespace stx;

	CPPUNIT_ASSERT( eval("5 + 4.5") == 9.5 );

	CPPUNIT_ASSERT( eval(" \"abc\" + \"def\" ") == "abcdef" );

	CPPUNIT_ASSERT( eval("5 * 6 + 7 * 2 - 2") == 42 );

	CPPUNIT_ASSERT( eval("(integer)(5 * 1.5)") == 7 );

	CPPUNIT_ASSERT( eval("((integer)(5 * 1.5 >= 1)) + 5") == 6 );

	CPPUNIT_ASSERT( eval(" logn(exp(21)) + sqrt(pow(21,2)) == 42 ") == true );

	CPPUNIT_ASSERT( eval("10 > 5 AND 5.2 <= 42.2 OR 2 == 4") == true );

	CPPUNIT_ASSERT( eval("(a * 2 + 4) / 2 == 44") == true );

	CPPUNIT_ASSERT( eval("-5 + 4.5") == -0.5 );

	CPPUNIT_ASSERT( eval("(0 < 1) && (4 > 2) && (1 = 1) && (2 == 2) && (2 != 4) && (1 <= 1) && (1 >= 1) && (2 =< 3) && (4 => 1)") == true );

	CPPUNIT_ASSERT( eval("!(true AND false) || (NOT true OR FALSE)") == true );

	CPPUNIT_ASSERT_THROW( eval("5 +"), stx::ExpressionParserException );

	CPPUNIT_ASSERT_THROW( eval("5 + FUNCXYZ()"), stx::UnknownSymbolException );

	CPPUNIT_ASSERT_THROW( eval("5 + xyz"), stx::UnknownSymbolException );

	CPPUNIT_ASSERT_THROW( eval("5 + COS(2,2)"), stx::BadFunctionCallException );

	{
	    std::string xmlstr = stx::parseExpressionStringXML("((integer)(5 * 1.5 >= 1)) + 5");
	    CPPUNIT_ASSERT( xmlstr.size() == 1010 );
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ExpressionParserTest );
