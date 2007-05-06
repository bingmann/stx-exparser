/* $Id$ */

%module "STX::ExpressionParser"
%{
#include "AnyScalar.h"
#include "ExpressionParser.h"

using namespace stx;
%}

%include "std_string.i"

%exception {
    try {
        $action
    }
    catch (BadFunctionCallException &e) {
        croak(e.what());
    }
    catch (BadSyntaxException &e) {
        croak(e.what());
    }
    catch (UnknownSymbolException &e) {
        croak(e.what());
    }
    catch (ConversionException &e) {
        croak(e.what());
    }
    catch (ExpressionParserException &e) {
        croak(e.what());
    }
}

%include "AnyScalar.h"
%include "ExpressionParser.h"

