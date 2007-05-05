/* $Id$ */

%module "STX::ExpressionParser"
%{
#include "AnyScalar.h"
#include "ExpressionParser.h"

using namespace stx;
%}

%include "std_string.i"

%include "AnyScalar.h"
%include "ExpressionParser.h"
