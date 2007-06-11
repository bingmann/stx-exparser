// $Id$
/** \file exprcalc.cc
 * File containing the first example application:
 * A Simple Expression Calculator.
 * See the doxygen documentation for a detailed walk-through of the code.
 */

// Simple Expression Calculator Example

#include <iostream>
 
#include "ExpressionParser.h"

int main(int argc, char *argv[])
{
    // collect expression by joining all input arguments
    std::string args;
    for(int i = 1; i < argc; i++) {
	if (!args.empty()) args += " ";
	args += argv[i];
    }

    std::cout << "input string: " << args << "\n";

    // parse expression into a parse tree
    stx::ParseTree pt;
    try
    {
	pt = stx::parseExpression(args);
	std::cout << "parsed: " << pt.toString() << "\n";
    }
    catch (stx::ExpressionParserException &e)
    {
	std::cout << "ExpressionParserException: " << e.what() << "\n";
	return 0;
    }

    // evaluate the expression with a very simple symbol table
    stx::BasicSymbolTable bst;
    bst.setVariable("x", 42);

    try
    {
	stx::AnyScalar val = pt.evaluate(bst);

	std::cout << "evaluated: " << val << "\n";
    }
    catch (stx::UnknownSymbolException &e)
    {
	std::cout << "UnknownSymbolException: " << e.what() << "\n";
	return 0;
    }
    catch (stx::ExpressionParserException &e)
    {
	std::cout << "ExpressionParserException: " << e.what() << "\n";
	return 0;
    }

    return 0;
}
