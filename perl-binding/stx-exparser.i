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
    catch (ExpressionParserException &e) {
        croak(e.what());
    }
}

%include "AnyScalar.h"
%include "ExpressionParser.h"

%perlcode %{
package STX::ExpressionParser::BasicSymbolTable;

# Nicer Function to add a hash of variables to the symbol table. Directly calls
# the constructor new($$hashref{$k})
sub setVariables($\%) {
    my ($self,$hashref) = (@_);

    foreach my $k (keys %$hashref) {
	my $v = STX::ExpressionParser::AnyScalar->new($$hashref{$k});
	$self->setVariable($k, $v);
    }
}

%}