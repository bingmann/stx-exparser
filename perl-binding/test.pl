#!/usr/bin/perl -w

use STX::ExpressionParser;

my $pn = STX::ExpressionParser::parseExpressionString(join(" ", @ARGV));

print "Parsed.toString(): ".$pn->toString()."\n";

print "Evaluated: ".$pn->evaluate()->getString()."\n";

