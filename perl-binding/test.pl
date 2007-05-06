#!/usr/bin/perl -w

use STX::ExpressionParser;

my $pn = STX::ExpressionParser::parseExpressionString(join(" ", @ARGV));

print "Parsed.toString(): ".$pn->toString()."\n";

my $bst = STX::ExpressionParser::BasicSymbolTable->new();
$bst->setVariables({ x => 42 });

print "Evaluated: ".$pn->evaluate($bst)->getString()."\n";

