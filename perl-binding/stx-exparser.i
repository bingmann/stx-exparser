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

# Another nice function to add a hash of variables to the symbol table. Does
# not use direct calls to the constructor new($$hashref{$k}). Instead each hash
# value is converted to a string and processed by the parser as such. This
# circumvents swig's type switching, which has strange results for single
# character strings.
sub setStringVariables($\%) {
    my ($self,$hashref) = (@_);

    foreach my $k (keys %$hashref) {
        my $v = STX::ExpressionParser::AnyScalar->new(STX::ExpressionParser::AnyScalar::ATTRTYPE_STRING);
        $v->setString("$$hashref{$k}");
	$self->setVariable($k, $v);
    }
}

%}