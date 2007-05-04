// $Id$

#include <cppunit/extensions/HelperMacros.h>

#include "AnyScalar.h"

#include <stdlib.h>
#include <boost/lexical_cast.hpp>

class AnyScalarTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( AnyScalarTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test1()
    {
	using namespace stx;

	CPPUNIT_ASSERT( AnyScalar(bool(false)).getInteger() == 0 );
	CPPUNIT_ASSERT( AnyScalar(bool(true)).getInteger() == 1 );

	AnyScalar t_bool (AnyScalar::ATTRTYPE_BOOL);
	AnyScalar t_integer (AnyScalar::ATTRTYPE_INTEGER);
	AnyScalar t_float (AnyScalar::ATTRTYPE_FLOAT);
	AnyScalar t_string (AnyScalar::ATTRTYPE_STRING);

	CPPUNIT_ASSERT( t_bool.setInteger(2) == true );
	CPPUNIT_ASSERT( t_bool.setInteger(1) == true );
	CPPUNIT_ASSERT( t_bool.getBoolean() == true );
	CPPUNIT_ASSERT( t_bool.getString() == "true" );

	CPPUNIT_ASSERT( t_bool.setDouble(0.3) );
	CPPUNIT_ASSERT( t_bool.getBoolean() == false );
	CPPUNIT_ASSERT( t_bool.getString() == "false" );

	CPPUNIT_ASSERT( t_bool.setString("t") );
	CPPUNIT_ASSERT( t_bool.getString() == "true" );


	CPPUNIT_ASSERT( t_integer.setInteger(42) );
	CPPUNIT_ASSERT( t_integer.getBoolean() == true );
	CPPUNIT_ASSERT( t_integer.getInteger() == 42 );
	CPPUNIT_ASSERT( t_integer.getString() == "42" );

	CPPUNIT_ASSERT( t_integer.setDouble(42.2) );
	CPPUNIT_ASSERT( t_integer.getBoolean() == true );
	CPPUNIT_ASSERT( t_integer.getInteger() == 42 );
	CPPUNIT_ASSERT( t_integer.getString() == "42" );

	CPPUNIT_ASSERT( t_integer.setString("42.2") == false );
	CPPUNIT_ASSERT( t_integer.setString("42") == true );
	CPPUNIT_ASSERT( t_integer.getBoolean() == true );
	CPPUNIT_ASSERT( t_integer.getInteger() == 42 );
	CPPUNIT_ASSERT( t_integer.getString() == "42" );

	CPPUNIT_ASSERT( t_float.setDouble(42) );
	CPPUNIT_ASSERT( t_float.getBoolean() == true );
	CPPUNIT_ASSERT( t_float.getInteger() == 42 );
	CPPUNIT_ASSERT( t_float.getDouble() == 42.0 );
	CPPUNIT_ASSERT( t_float.getString() == "42" );

	CPPUNIT_ASSERT( t_float.setDouble(42.42) );
	CPPUNIT_ASSERT( t_float.getInteger() == 42 );
	CPPUNIT_ASSERT( t_float.getString() == "42.42" );

	CPPUNIT_ASSERT( t_float.setString("42.42") );
	CPPUNIT_ASSERT( t_float.getInteger() == 42 );
	CPPUNIT_ASSERT( t_float.getString() == "42.42" );


	CPPUNIT_ASSERT( t_string.setInteger(20) );
	CPPUNIT_ASSERT_THROW( t_string.getBoolean(), ConversionException );
	CPPUNIT_ASSERT( t_string.getString() == "20" );

	CPPUNIT_ASSERT( t_string.setInteger(0) );
	CPPUNIT_ASSERT( t_string.getBoolean() == false );
	CPPUNIT_ASSERT( t_string.getString() == "0" );

	CPPUNIT_ASSERT( t_string.setInteger(-20) );
	CPPUNIT_ASSERT( t_string.getString() == "-20" );

	CPPUNIT_ASSERT( t_string.setStringQuoted("\"bla\\n\\\\\\\"h\"") );
	CPPUNIT_ASSERT( t_string.getString() == "bla\n\\\"h" );

	AnyScalar t_other = t_string;
	CPPUNIT_ASSERT( t_other.getType() == AnyScalar::ATTRTYPE_STRING );
	CPPUNIT_ASSERT( t_other.getString() == t_string.getString() );
	CPPUNIT_ASSERT( t_other == t_string );

	t_other = t_integer;
	CPPUNIT_ASSERT( t_other.getType() == AnyScalar::ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( t_other != t_string );
	CPPUNIT_ASSERT( t_other == t_integer );

	CPPUNIT_ASSERT( t_other.convertType(AnyScalar::ATTRTYPE_STRING) );
	CPPUNIT_ASSERT( t_other.getInteger() == 42 );

	// Arithmetic and Comparison Operators
	
	CPPUNIT_ASSERT( (AnyScalar(5) + AnyScalar(6)) == 11 );
	CPPUNIT_ASSERT( (AnyScalar(5) + AnyScalar(6)).getDouble() == 11.0 );

	CPPUNIT_ASSERT( (AnyScalar(5.0) + AnyScalar(4)).getString() == "9" );

	CPPUNIT_ASSERT( (AnyScalar("abc") + AnyScalar("def")) == "abcdef" );

	CPPUNIT_ASSERT( (AnyScalar("1.1") + AnyScalar(3.0)) == 4.1 );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnyScalarTest );
