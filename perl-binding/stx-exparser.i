/* $Id$ */

%module "STX::ExpressionParser"
%{
#include "AnyScalar.h"
#include "ExpressionParser.h"

using namespace stx;
%}

%include "std_string.i"
%include "std_vector.i"

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

%perlcode %{
package STX::ExpressionParser::BasicSymbolTable;

# Nicer Function to add a hash of variables to the symbol table
sub setVariables($\%) {
    my ($self,$hashref) = (@_);

    foreach my $k (keys %$hashref) {
	$self->setVariable($k, STX::ExpressionParser::AnyScalar->new($$hashref{$k}));
    }
}

%}