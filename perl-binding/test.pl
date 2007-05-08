#!/usr/bin/perl -w

use STX::ExpressionParser;

my $pt = STX::ExpressionParser::parseExpression(join(" ", @ARGV));

print "Parsed.toString(): ".$pt->toString()."\n";

my $bst = STX::ExpressionParser::BasicSymbolTable->new();
$bst->setVariables({ x => 42 });

print "Evaluated: ".$pt->evaluate($bst)->getString()."\n";

