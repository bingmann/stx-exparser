// $Id$

#include "ExpressionParser.h"

#include <boost/spirit/core.hpp>

#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>

#include <boost/spirit/utility/lists.hpp>
#include <boost/spirit/utility/distinct.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/grammar_def.hpp> 

#include <iostream>
#include <sstream>

// #define STX_DEBUG_PARSER

namespace stx {

/// Enclosure for the spirit parser grammar and hidden parse node
/// implementation classes.
namespace Grammar {

using namespace boost::spirit;

/// This enum specifies ids for the parse tree nodes created for each rule.
enum parser_ids
{
    integer_const_id = 1,
    double_const_id,
    string_const_id,
    constant_id,

    function_call_id,
    function_identifier_id,

    attrname_id,

    atom_expr_id,

    unary_expr_id,
    mul_expr_id,
    add_expr_id,

    cast_expr_id,
    cast_spec_id,

    comp_expr_id,
    and_expr_id,
    or_expr_id,

    expr_id,
    exprlist_id,
};

/// Keyword parser used for matching words with () and spaces as separators.
distinct_parser<> keyword_p("a-zA-Z0-9_");

/// The boost::spirit expression parser grammar
struct ExpressionGrammar : public grammar<ExpressionGrammar>
{
    /// The boost::spirit expression parser grammar definition (for a specific
    /// scanner) with two entry points.
    template <typename ScannerT>
    struct definition : public grammar_def<rule<ScannerT, parser_context<>, parser_tag<expr_id> >,
					   rule<ScannerT, parser_context<>, parser_tag<exprlist_id> > >
    {
	definition(ExpressionGrammar const& /*self*/)
	{
	    // *** Constants

	    constant
		= double_const
		| integer_const
		| string_const
		;
	    
	    integer_const
		= int_p
		;

	    double_const
		= strict_real_p
		;

            string_const
                = lexeme_d[
		    token_node_d[ '"' >> *(c_escape_ch_p - '"') >> '"' ]
                    ]
                ;

	    // *** Function call and function identifier

            function_call
                = root_node_d[function_identifier] >> discard_node_d[ ch_p('(') ] 
						   >> exprlist
						   >> discard_node_d[ ch_p(')') ]
                ;

            function_identifier
                = lexeme_d[ 
		    token_node_d[ alpha_p >> *(alnum_p | ch_p('_')) ]
                    ]
                ;

	    // *** Expression names

            attrname
                = lexeme_d[ 
		    token_node_d[ alpha_p >> *(alnum_p | ch_p('_')) ]
                    ]
                ;

	    // *** Valid Expressions, from small to large

            atom_expr
                = constant
                | inner_node_d[ ch_p('(') >> expr >> ch_p(')') ]
                | function_call
		| attrname
                ;

            unary_expr
                = !( root_node_d[ch_p('+') | ch_p('-') | ch_p('!') | str_p("not")] )
		    >> atom_expr
                ;

            mul_expr
                = unary_expr >>
		  *( root_node_d[ch_p('*')] >> unary_expr 
		   | root_node_d[ch_p('/')] >> unary_expr )
                ;

	    add_expr
		= mul_expr >>
		  *( root_node_d[ch_p('+')] >> mul_expr
		   | root_node_d[ch_p('-')] >> mul_expr )
		;

	    cast_spec
		= discard_node_d[ ch_p('(') ]
		>> (
		    keyword_p("bool") |
		    keyword_p("char") | keyword_p("short") | keyword_p("int") | keyword_p("integer") | keyword_p("long") |
		    keyword_p("byte") | keyword_p("word") | keyword_p("dword") | keyword_p("qword") |
		    keyword_p("float") | keyword_p("double") |
		    keyword_p("string")
		    )
		>> discard_node_d[ ch_p(')') ]
		;

            cast_expr
		= root_node_d[ !cast_spec ] >> add_expr
		;

	    comp_expr
		= cast_expr >>
		*( root_node_d[str_p("==")] >> cast_expr 
		 | root_node_d[ch_p('=')] >> cast_expr
		 | root_node_d[str_p("!=")] >> cast_expr
		 | root_node_d[ch_p('<')] >> cast_expr
		 | root_node_d[ch_p('>')] >> cast_expr
		 | root_node_d[str_p("<=")] >> cast_expr
		 | root_node_d[str_p(">=")] >> cast_expr
		 | root_node_d[str_p("=<")] >> cast_expr
		 | root_node_d[str_p("=>")] >> cast_expr
		 )
		;

	    and_expr
		= comp_expr >>
		*( root_node_d[ as_lower_d[ str_p("and")] ] >> comp_expr
		 | root_node_d[ as_lower_d[ str_p("&&")] ] >> comp_expr
		 )
		;

	    or_expr
		= and_expr >>
		*( root_node_d[ as_lower_d[ str_p("or")] ] >> and_expr
		 | root_node_d[ as_lower_d[ str_p("||")] ] >> and_expr
		 )
		;

	    // *** Base Expression and List

	    expr
		= or_expr
		;

	    exprlist
		= infix_node_d[ !list_p(expr, ch_p(',')) ]
		;

	    // This is a special spirit feature to declare multiple grammar
	    // entry points
	    this->start_parsers(expr, exprlist); 

#ifdef STX_DEBUG_PARSER
	    BOOST_SPIRIT_DEBUG_RULE(constant);

	    BOOST_SPIRIT_DEBUG_RULE(integer_const);
	    BOOST_SPIRIT_DEBUG_RULE(double_const);
	    BOOST_SPIRIT_DEBUG_RULE(string_const);

	    BOOST_SPIRIT_DEBUG_RULE(function_call);
	    BOOST_SPIRIT_DEBUG_RULE(function_identifier);
	    
	    BOOST_SPIRIT_DEBUG_RULE(attrname);

	    BOOST_SPIRIT_DEBUG_RULE(atom_expr);

	    BOOST_SPIRIT_DEBUG_RULE(unary_expr);
	    BOOST_SPIRIT_DEBUG_RULE(mul_expr);
	    BOOST_SPIRIT_DEBUG_RULE(add_expr);

	    BOOST_SPIRIT_DEBUG_RULE(cast_spec);
	    BOOST_SPIRIT_DEBUG_RULE(cast_expr);

	    BOOST_SPIRIT_DEBUG_RULE(comp_expr);
	    BOOST_SPIRIT_DEBUG_RULE(and_expr);
	    BOOST_SPIRIT_DEBUG_RULE(or_expr);

	    BOOST_SPIRIT_DEBUG_RULE(expr);
	    BOOST_SPIRIT_DEBUG_RULE(exprlist);
#endif
	}

	/// Rule for a constant: one of the three scalar types integer_const,
	/// double_const or string_const
        rule<ScannerT, parser_context<>, parser_tag<constant_id> > 		constant;

	/// Integer constant rule: "1234"
        rule<ScannerT, parser_context<>, parser_tag<integer_const_id> > 	integer_const;
	/// Float constant rule: "1234"
        rule<ScannerT, parser_context<>, parser_tag<double_const_id> > 		double_const;
	/// String constant rule: with quotes "abc"
        rule<ScannerT, parser_context<>, parser_tag<string_const_id> > 		string_const;

	/// Function call rule: func1(a,b,c) where a,b,c is a list of exprs
        rule<ScannerT, parser_context<>, parser_tag<function_call_id> > 	function_call;
	/// Function rule to match a function identifier: alphanumeric and _
	/// are allowed.
        rule<ScannerT, parser_context<>, parser_tag<function_identifier_id> > 	function_identifier;

	/// Rule to match an attribute name: alphanumeric with _
        rule<ScannerT, parser_context<>, parser_tag<attrname_id> > 		attrname;

	/// Helper rule which implements () bracket grouping.
        rule<ScannerT, parser_context<>, parser_tag<atom_expr_id> > 		atom_expr;

	/// Unary operator rule: recognizes + - ! and "not".
        rule<ScannerT, parser_context<>, parser_tag<unary_expr_id> > 		unary_expr;
	/// Binary operator rule taking precedent before add_expr:
	/// recognizes * and /
        rule<ScannerT, parser_context<>, parser_tag<mul_expr_id> > 		mul_expr;
	/// Binary operator rule: recognizes + and -
        rule<ScannerT, parser_context<>, parser_tag<add_expr_id> > 		add_expr;

	/// Match all the allowed cast types: short, double, etc.
        rule<ScannerT, parser_context<>, parser_tag<cast_spec_id> >        	cast_spec;
	/// Cast operator written like in C: (short)
        rule<ScannerT, parser_context<>, parser_tag<cast_expr_id> >        	cast_expr;

	/// Comparison operator: recognizes == = != <= >= < > => =<
        rule<ScannerT, parser_context<>, parser_tag<comp_expr_id> >        	comp_expr;
	/// Boolean operator: recognizes && and "and" and works only on boolean
	/// values
        rule<ScannerT, parser_context<>, parser_tag<and_expr_id> >        	and_expr;
	/// Boolean operator: recognizes || and "or" and works only on boolean
	/// values
        rule<ScannerT, parser_context<>, parser_tag<or_expr_id> >        	or_expr;

	/// Base rule to match an expression 
        rule<ScannerT, parser_context<>, parser_tag<expr_id> >        		expr;
	/// Base rule to match a comma-separated list of expressions (used for
	/// function arguments and lists of expressions)
        rule<ScannerT, parser_context<>, parser_tag<exprlist_id> >    		exprlist;
    };
};

// *** Classes representing the nodes in the resulting parse tree, these need
// *** not be publicly available via the header file.

/// Constant value nodes of the parse tree. This class holds any of the three
/// constant types in the enclosed AnyScalar object.
class PNConstant : public ParseNode
{
private:
    /// The constant parsed value.
    class AnyScalar	value;

public:
    /// Assignment from the string received from the parser.
    PNConstant(AnyScalar::attrtype_t type, std::string strvalue)
	: ParseNode(), value(type)
    {
	// check whether to dequote the incoming string.
	if (type == AnyScalar::ATTRTYPE_STRING)
	    value.setStringQuoted(strvalue);
	else
	    value.setString(strvalue); // not a string, but an integer or double
    }

    /// constructor for folded constant values.
    PNConstant(const AnyScalar &_value)
	: value(_value)
    {
    }

    /// Easiest evaluation: return the constant.
    virtual AnyScalar evaluate(const class SymbolTable &) const
    {
	return value;
    }

    /// Returns true, because value is constant
    virtual bool evaluate_const(AnyScalar *dest) const
    {
	if (dest) *dest = value;
	return true;
    }

    /// String representation of the constant AnyScalar value.
    virtual std::string toString() const
    {
	if (value.getType() == AnyScalar::ATTRTYPE_STRING) {
	    // TODO: quote string!!!!
	    return std::string("\"") + value.getString() + std::string("\"");
	}
	return value.getString();
    }
};

/// Parse tree node representing a variable place-holder. It is filled when
/// parameterized by a symbol table.
class PNVariable : public ParseNode
{
private:
    /// String name of the attribute
    std::string		attrname;

public:
    /// Constructor from the string received from the parser.
    PNVariable(std::string _attrname)
	: ParseNode(), attrname(_attrname)
    {
    }

    /// Check the given symbol table for the actual value of this variable.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	return st.lookupVariable(attrname);
    }

    /// Returns false, because value isn't constant.
    virtual bool evaluate_const(AnyScalar *) const
    {
	return false;
    }

    /// Nothing but the attribute name.
    virtual std::string toString() const
    {
	return attrname;
    }
};

/// Parse tree node representing a function place-holder. It is filled when
/// parameterized by a symbol table.
class PNFunction : public ParseNode
{
public:
    /// Type of sequence of subtrees to evaluate as function parameters.
    typedef std::vector<const class ParseNode*> paramlist_type;

private:
    /// String name of the function
    std::string		funcname;

    /// The array of function parameter subtrees
    paramlist_type	paramlist;

public:
    /// Constructor from the string received from the parser.
    PNFunction(std::string _funcname, const paramlist_type& _paramlist)
	: ParseNode(), funcname(_funcname), paramlist(_paramlist)
    {
    }

    /// Delete the paramlist
    ~PNFunction()
    {
	for(unsigned int i = 0; i < paramlist.size(); ++i)
	    delete paramlist[i];
    }

    /// Check the given symbol table for the actual value of this variable.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	std::vector<AnyScalar> paramvalues;

	for(unsigned int i = 0; i < paramlist.size(); ++i)
	{
	    paramvalues.push_back( paramlist[i]->evaluate(st) );
	}

	return st.processFunction(funcname, paramvalues);
    }

    /// Returns false, because value isn't constant.
    virtual bool evaluate_const(AnyScalar *) const
    {
	return false;
    }

    /// Nothing but the attribute name.
    virtual std::string toString() const
    {
	std::string str = funcname + "(";
	for(unsigned int i = 0; i < paramlist.size(); ++i)
	{
	    if (i != 0) str += ",";
	    str += paramlist[i]->toString();
	}
	return str + ")";
    }
};

/// Parse tree node representing an unary operator: '+', '-', '!' or
/// "not". This node has one child.
class PNUnaryArithmExpr : public ParseNode
{
private:
    /// Pointer to the single operand
    const ParseNode 	*operand;

    /// Arithmetic operation to perform: either '+', '-' or '!'. Further
    /// optimization would be to create an extra class for each op
    char	op;

public:
    /// Constructor from the parser: operand subnode and operator id.
    PNUnaryArithmExpr(const ParseNode* _operand, char _op)
	: ParseNode(), operand(_operand), op(_op)
    {
	if (op == 'n') op = '!';
    }

    /// Recursively delete the parse tree.
    virtual ~PNUnaryArithmExpr()
    {
	delete operand;
    }

    /// Applies the operator to the recursively calculated value.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	AnyScalar dest = operand->evaluate(st);

	if (op == '-') {
	    dest = -dest;	    
	}
	else if (op == '!')
	{
	    // This should not happend, as types are constant in the parse tree
	    if (dest.getType() != AnyScalar::ATTRTYPE_BOOL)
		throw(BadSyntaxException("Invalid operand for !. Operand must be of type bool."));

	    dest = -dest;
 	}
	else {
	    assert(op == '+');
	}

	return dest;
    }

    /// Calculates subnodes and returns result if the operator can be applied.
    virtual bool evaluate_const(AnyScalar *dest) const
    {
	if (!dest) return false;

	bool b = operand->evaluate_const(dest);

	if (op == '-') {
	    *dest = -(*dest);
	}
	else if (op == '!')
	{
	    if (dest->getType() != AnyScalar::ATTRTYPE_BOOL)
		throw(BadSyntaxException("Invalid operand for !. Operand must be of type bool."));

	    *dest = -(*dest);
	}
	else {
	    assert(op == '+');
	}
	
	return b;
    }

    /// Return the subnode's string with this operator prepended.
    virtual std::string toString() const
    {
	return std::string("(") + op + " " + operand->toString() + ")";
    }
};

/// Parse tree node representing a binary operators: +, -, * and / for numeric
/// values. This node has two children.
class PNBinaryArithmExpr : public ParseNode
{
private:
    /// Pointers to the left of the two child parse trees.
    const ParseNode 	*left;

    /// Pointers to the right of the two child parse trees.
    const ParseNode	*right;

    /// Arithmetic operation to perform: left op right.
    /// Further optimization would be to create an extra class for each op
    char	op;

public:
    /// Constructor from the parser: both operand subnodes and the operator id.
    PNBinaryArithmExpr(const ParseNode* _left,
		       const ParseNode* _right,
		       char _op)
	: ParseNode(),
	  left(_left), right(_right), op(_op)
    { }

    /// Recursively delete parse tree.
    virtual ~PNBinaryArithmExpr()
    {
	delete left;
	delete right;
    }

    /// Applies the operator to the two recursive calculated values. The actual
    /// switching between types is handled by AnyScalar's operators.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	AnyScalar vl = left->evaluate(st);
	AnyScalar vr = right->evaluate(st);

	if (op == '+') {
	    return (vl + vr);
	}
	else if (op == '-') {
	    return (vl - vr);
	}
	else if (op == '*') {
	    return (vl * vr);
	}
	else if (op == '/') {
	    return (vl / vr);
	}

	assert(0);
	return 0;
    }

    /// Returns false because this node isn't always constant. Tries to
    /// calculate a constant subtree's value.
    virtual bool evaluate_const(AnyScalar *dest) const
    {
	if (!dest) return false;

	AnyScalar vl(AnyScalar::ATTRTYPE_INVALID), vr(AnyScalar::ATTRTYPE_INVALID);
	
	bool bl = left->evaluate_const(&vl);
	bool br = right->evaluate_const(&vr);

	if (op == '+') {
	    *dest = vl + vr;
	}
	else if (op == '-') {
	    *dest = vl - vr;
	}
	else if (op == '*') {
	    *dest = vl * vr;
	}
	else if (op == '/') {
	    *dest = vl / vr;
	}

	return (bl and br);
    }

    /// String representing (operandA op operandB)
    virtual std::string toString() const
    {
	return std::string("(") + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

/// Parse tree node handling type conversions within the tree.
class PNCastExpr : public ParseNode
{
private:
    /// Child tree of which the return value should be casted.
    const ParseNode*	operand;

    /// Attribute type to cast the value to.
    AnyScalar::attrtype_t	type;

public:
    /// Constructor from the parser: operand subnode and the cast type as
    /// recognized by AnyScalar.
    PNCastExpr(const ParseNode* _operand, AnyScalar::attrtype_t _type)
	: ParseNode(),
	  operand(_operand), type(_type)
    { }

    /// Recursively delete parse tree.
    virtual ~PNCastExpr()
    {
	delete operand;
    }

    /// Recursive calculation of the value and subsequent casting via
    /// AnyScalar's convertType method.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	AnyScalar val = operand->evaluate(st);
	val.convertType(type);
	return val;
    }

    /// Returns false because this node isn't always constant.
    virtual bool evaluate_const(AnyScalar *dest) const
    {
	if (!dest) return false;

	bool b = operand->evaluate_const(dest);
	dest->convertType(type);
	return b;
    }

    /// c-like representation of the cast
    virtual std::string toString() const
    {
	return std::string("((") + AnyScalar::getTypeString(type) + ")" + operand->toString() + ")";
    }
};

/// Parse tree node representing a binary comparison operator: ==, =, !=, <, >,
/// >=, <=, =>, =<. This node has two children.
class PNBinaryComparisonExpr : public ParseNode
{
private:
    /// Pointers to the left of the two child parse trees.
    const ParseNode 	*left;

    /// Pointers to the right of the two child parse trees.
    const ParseNode	*right;

    /// Comparison operation to perform: left op right
    enum { EQUAL, NOTEQUAL, LESS, GREATER, LESSEQUAL, GREATEREQUAL } op;

    /// String saved for toString()
    std::string		opstr;

public:
    /// Constructor from the parser: both operand subnodes and the operator id.
    PNBinaryComparisonExpr(const ParseNode* _left,
			   const ParseNode* _right,
			   std::string _op)
	: ParseNode(),
	  left(_left), right(_right), opstr(_op)
    {
	if (_op == "==" or _op == "=")
	    op = EQUAL;
	else if (_op == "!=")
	    op = NOTEQUAL;
	else if (_op == "<")
	    op = LESS;
	else if (_op == ">")
	    op = GREATER;
	else if (_op == "<=" or _op == "=<")
	    op = LESSEQUAL;
	else if (_op == ">=" or _op == "=>")
	    op = GREATEREQUAL;
	else
	    throw(BadSyntaxException("Program Error: invalid binary comparision operator."));
    }

    /// Recursively delete parse tree.
    virtual ~PNBinaryComparisonExpr()
    {
	delete left;
	delete right;
    }

    /// Applies the operator to the two recursive calculated values. The actual
    /// switching between types is handled by AnyScalar's operators. This
    /// result type of this processing node is always bool.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	AnyScalar vl = left->evaluate(st);
	AnyScalar vr = right->evaluate(st);

	AnyScalar dest(AnyScalar::ATTRTYPE_BOOL);

	switch(op)
	{
	case EQUAL:
	    dest = AnyScalar( vl.equal_to(vr) );
	    break;

	case NOTEQUAL:
	    dest = AnyScalar( vl.not_equal_to(vr) );
	    break;

	case LESS:
	    dest = AnyScalar( vl.less(vr) );
	    break;

	case GREATER:
	    dest = AnyScalar( vl.greater(vr) );
	    break;

	case LESSEQUAL:
	    dest = AnyScalar( vl.less_equal(vr) );
	    break;

	case GREATEREQUAL:
	    dest = AnyScalar( vl.greater_equal(vr) );
	    break;

	default:
	    assert(0);
	}

	return dest;
    }

    /// Returns false because this node isn't always constant.
    virtual bool evaluate_const(AnyScalar *dest) const
    {
	if (!dest) return false;

	AnyScalar vl(AnyScalar::ATTRTYPE_INVALID), vr(AnyScalar::ATTRTYPE_INVALID);
	
	bool bl = left->evaluate_const(&vl);
	bool br = right->evaluate_const(&vr);

	switch(op)
	{
	case EQUAL:
	    *dest = AnyScalar( vl.equal_to(vr) );
	    break;

	case NOTEQUAL:
	    *dest = AnyScalar( vl.not_equal_to(vr) );
	    break;

	case LESS:
	    *dest = AnyScalar( vl.less(vr) );
	    break;

	case GREATER:
	    *dest = AnyScalar( vl.greater(vr) );
	    break;

	case LESSEQUAL:
	    *dest = AnyScalar( vl.less_equal(vr) );
	    break;

	case GREATEREQUAL:
	    *dest = AnyScalar( vl.greater_equal(vr) );
	    break;

	default:
	    assert(0);
	}

	return (bl and br);
    }

    /// String (operandA op operandB)
    virtual std::string toString() const
    {
	return std::string("(") + left->toString() + " " + opstr + " " + right->toString() + ")";
    }
};

/// Parse tree node representing a binary logic operator: and, or, &&, ||. This
/// node has two children.
class PNBinaryLogicExpr : public ParseNode
{
private:
    /// Pointers to the left of the two child parse trees.
    const ParseNode 	*left;

    /// Pointers to the right of the two child parse trees.
    const ParseNode	*right;

    /// Comparison operation to perform: left op right
    enum { OP_AND, OP_OR } op;

public:
    /// Constructor from the parser: both operand subnodes and the operator id.
    PNBinaryLogicExpr(const ParseNode* _left,
		      const ParseNode* _right,
		      std::string _op)
	: ParseNode(),
	  left(_left), right(_right)
    {
	if (_op == "and" or _op == "&&")
	    op = OP_AND;
	else if (_op == "or" or _op == "||")
	    op = OP_OR;
	else
	    throw(BadSyntaxException("Program Error: invalid binary logic operator."));
    }

    /// Recursively delete parse tree.
    virtual ~PNBinaryLogicExpr()
    {
	if (left) delete left;
	if (right) delete right;
    }

    /// Calculate the operator
    inline bool do_operator(bool left, bool right) const
    {
	if (op == OP_AND)
	    return left && right;
	else if (op == OP_OR)
	    return left || right;
	else
	    return false;
    }

    /// Return the string of this operator
    inline std::string get_opstr() const
    {
	return (op == OP_AND) ? "&&" : "||";
    }

    /// Applies the operator to the two recursive calculated values. The actual
    /// switching between types is handled by AnyScalar's operators.
    virtual AnyScalar evaluate(const class SymbolTable &st) const
    {
	AnyScalar vl = left->evaluate(st);
	AnyScalar vr = right->evaluate(st);

	// these should never happen.
	if (vl.getType() != AnyScalar::ATTRTYPE_BOOL)
	    throw(BadSyntaxException(std::string("Invalid left operand for ") + get_opstr() + ". Both operands must be of type bool."));
	if (vr.getType() != AnyScalar::ATTRTYPE_BOOL)
	    throw(BadSyntaxException(std::string("Invalid right operand for ") + get_opstr() + ". Both operands must be of type bool."));

	int bvl = vl.getInteger();
	int bvr = vr.getInteger();

	return AnyScalar( do_operator(bvl, bvr) );
    }

    /// Applies the operator to the two recursive calculated const
    /// values. Determining if this node is constant is somewhat more tricky
    /// than with the other parse nodes: AND with a false operand is always
    /// false. OR with a true operand is always true.
    virtual bool evaluate_const(AnyScalar *dest) const
    {
	if (!dest) return false; // returns false because this node isnt always constant

	AnyScalar vl(AnyScalar::ATTRTYPE_INVALID), vr(AnyScalar::ATTRTYPE_INVALID);
	
	bool bl = left->evaluate_const(&vl);
	bool br = right->evaluate_const(&vr);

	if (vl.getType() != AnyScalar::ATTRTYPE_BOOL)
	    throw(BadSyntaxException(std::string("Invalid left operand for ") + get_opstr() + ". Both operands must be of type bool."));
	if (vr.getType() != AnyScalar::ATTRTYPE_BOOL)
	    throw(BadSyntaxException(std::string("Invalid right operand for ") + get_opstr() + ". Both operands must be of type bool."));

	int bvl = vl.getInteger();
	int bvr = vr.getInteger();

	*dest = AnyScalar( do_operator(bvl, bvr) );

	if (op == OP_AND)
	{
	    // true if either both ops are themselves constant, or if either of
	    // the ops are constant and evaluates to false.
	    return (bl and br) or (bl and !bvl) or (br and !bvr);
	}
	else if (op == OP_OR)
	{
	    // true if either both ops are themselves constant, or if either of
	    // the ops is constant and evaluates to true.
	    return (bl and br) or (bl and bvl) or (br and bvr);
	}
	else {
	    assert(0);
	    return false;
	}
    }

    /// String (operandA op operandB)
    virtual std::string toString() const
    {
	return std::string("(") + left->toString() + " " + get_opstr() + " " + right->toString() + ")";
    }

    /// Detach left node
    inline const ParseNode* detach_left()
    {
	const ParseNode *n = left;
	left = NULL;
	return n;
    }

    /// Detach right node
    inline const ParseNode* detach_right()
    {
	const ParseNode *n = right;
	right = NULL;
	return n;
    }
};

// *** Functions which translate the resulting parse tree into our expression
// *** tree, simultaneously folding constants.

typedef std::string::const_iterator InputIterT;

typedef tree_match<InputIterT> ParseTreeMatchT;

typedef ParseTreeMatchT::const_tree_iterator TreeIterT;

/// Build_expr is the constructor method to create a parse tree from the
/// AST-tree returned by the spirit parser.
static const ParseNode* build_expr(TreeIterT const& i)
{
#ifdef STX_DEBUG_PARSER
    std::cout << "In build_expr. i->value = " <<
        std::string(i->value.begin(), i->value.end()) <<
        " i->children.size() = " << i->children.size() << 
	" i->value.id = " << i->value.id().to_long() << std::endl;
#endif

    switch(i->value.id().to_long())
    {
    // *** Constant node cases

    case integer_const_id:
    {
	return new PNConstant(AnyScalar::ATTRTYPE_INTEGER,
			      std::string(i->value.begin(), i->value.end()));
    }

    case double_const_id:
    {
	return new PNConstant(AnyScalar::ATTRTYPE_DOUBLE,
			      std::string(i->value.begin(), i->value.end()));
    }

    case string_const_id:
    {
	return new PNConstant(AnyScalar::ATTRTYPE_STRING,
			      std::string(i->value.begin(), i->value.end()));
    }

    // *** Arithmetic node cases

    case unary_expr_id:
    {
	char arithop = *i->value.begin();
	assert(i->children.size() == 1);

	const ParseNode *val = build_expr(i->children.begin());

	if (val->evaluate_const(NULL))
	{
	    // construct a constant node
	    PNUnaryArithmExpr tmpnode(val, arithop);
	    AnyScalar constval(AnyScalar::ATTRTYPE_INVALID);

	    tmpnode.evaluate_const(&constval);

	    return new PNConstant(constval);
	}
	else
	{
	    // calculation node
	    return new PNUnaryArithmExpr(val, arithop);
	}
    }

    case add_expr_id:
    case mul_expr_id:
    {
	char arithop = *i->value.begin();
	assert(i->children.size() == 2);

	// auto_ptr needed because of possible parse exceptions in build_expr.

	std::auto_ptr<const ParseNode> left( build_expr(i->children.begin()) );
	std::auto_ptr<const ParseNode> right( build_expr(i->children.begin()+1) );

	if (left->evaluate_const(NULL) and right->evaluate_const(NULL))
	{
	    // construct a constant node
	    PNBinaryArithmExpr tmpnode(left.release(), right.release(), arithop);
	    AnyScalar both(AnyScalar::ATTRTYPE_INVALID);

	    tmpnode.evaluate_const(&both);

	    // left and right are deleted by tmpnode's deconstructor

	    return new PNConstant(both);
	}
	else
	{
	    // calculation node
	    return new PNBinaryArithmExpr(left.release(), right.release(), arithop);
        }
    }

    // *** Cast node case

    case cast_spec_id:
    {
	assert(i->children.size() == 1);

	std::string tname(i->value.begin(), i->value.end());
	AnyScalar::attrtype_t at = AnyScalar::stringToType(tname);
	
	const ParseNode *val = build_expr(i->children.begin());

	if (val->evaluate_const(NULL))
	{
	    // construct a constant node
	    PNCastExpr tmpnode(val, at);

	    AnyScalar constval(AnyScalar::ATTRTYPE_INVALID);

	    tmpnode.evaluate_const(&constval);

	    return new PNConstant(constval);
	}
	else
	{
	    return new PNCastExpr(val, at);
	}
    }

    // *** Binary Comparison Operator

    case comp_expr_id:
    {
	assert(i->children.size() == 2);

	std::string arithop(i->value.begin(), i->value.end());

	// we need auto_ptr because of possible parse exceptions in build_expr.

	std::auto_ptr<const ParseNode> left( build_expr(i->children.begin()) );
	std::auto_ptr<const ParseNode> right( build_expr(i->children.begin()+1) );

	if (left->evaluate_const(NULL) and right->evaluate_const(NULL))
	{
	    // construct a constant node
	    PNBinaryComparisonExpr tmpnode(left.release(), right.release(), arithop);
	    AnyScalar both(AnyScalar::ATTRTYPE_INVALID);

	    tmpnode.evaluate_const(&both);

	    // left and right are deleted by tmpnode's deconstructor

	    return new PNConstant(both);
	}
	else
	{
	    // calculation node
	    return new PNBinaryComparisonExpr(left.release(), right.release(), arithop);
        }
    }

    // *** Binary Logic Operator

    case and_expr_id:
    case or_expr_id:
    {
	assert(i->children.size() == 2);

	std::string logicop(i->value.begin(), i->value.end());
	std::transform(logicop.begin(), logicop.end(), logicop.begin(), tolower);

	// auto_ptr needed because of possible parse exceptions in build_expr.

	std::auto_ptr<const ParseNode> left( build_expr(i->children.begin()) );
	std::auto_ptr<const ParseNode> right( build_expr(i->children.begin()+1) );

	bool constleft = left->evaluate_const(NULL);
	bool constright = right->evaluate_const(NULL);

	// a logical node is constant if one of the two ops is constant. so we
	// construct a calculation node and check later.
	std::auto_ptr<PNBinaryLogicExpr> node( new PNBinaryLogicExpr(left.release(), right.release(), logicop) );

	if (constleft or constright)
	{
	    AnyScalar both(AnyScalar::ATTRTYPE_INVALID);

	    // test if the node is really const.
	    if (node->evaluate_const(&both))
	    {
		// return a constant node instead, node will be deleted by
		// auto_ptr, left,right by node's destructor.
		return new PNConstant(both);
	    }
	}
	if (constleft)
	{
	    // left node is constant, but the evaluation is not
	    // -> only right node is meaningful.
	    return node->detach_right();
	}
	if (constright)
	{
	    // right node is constant, but the evaluation is not
	    // -> only left node is meaningful.
	    return node->detach_left();
	}

	return node.release();
    }

    // *** Attribute and Function name place-holder

    case attrname_id:
    {
	assert(i->children.size() == 0);

	std::string attrname(i->value.begin(), i->value.end());

        return new PNVariable(attrname);
    }

    case function_identifier_id:
    {
	std::string funcname(i->value.begin(), i->value.end());
	std::vector<const class ParseNode*> paramlist;

	if (i->children.size() > 0)
	{
	    TreeIterT const& paramlistchild = i->children.begin();

	    if (paramlistchild->value.id().to_long() == exprlist_id)
	    {
		try
		{
		    for(TreeIterT ci = paramlistchild->children.begin(); ci != paramlistchild->children.end(); ++ci)
		    {
			const ParseNode *pas = build_expr(ci);
			paramlist.push_back(pas);
		    }
		}
		catch (...) // need to clean-up
		{
		    for(unsigned int i = 0; i < paramlist.size(); ++i)
			delete paramlist[i];
		    throw;
		}
	    }
	    else
	    {
		// just one subnode and its not a full expression list
		paramlist.push_back( build_expr(paramlistchild) );
	    }
	}

        return new PNFunction(funcname, paramlist);
    }

    default:
	throw(ExpressionParserException("Unknown AST parse tree node found. This should never happen."));
    }
}

/// build_attrlist constructs the vector holding the ParseNode parse tree for
/// each requested attribute.
std::vector<const class ParseNode*> build_exprlist(TreeIterT const &i)
{
#ifdef STX_DEBUG_PARSER
    std::cout << "In build_attrlist. i->value = " <<
        std::string(i->value.begin(), i->value.end()) <<
        " i->children.size() = " << i->children.size() << 
	" i->value.id = " << i->value.id().to_long() << std::endl;
#endif

    std::vector<const class ParseNode*> sellist;

    for(TreeIterT ci = i->children.begin(); ci != i->children.end(); ++ci)
    {
	const ParseNode *vas = build_expr(ci);

	sellist.push_back(vas);
    }

    return sellist;
}

template <typename TreeInfo>
static inline void debug_dump_xml(std::ostream &os, const std::string &input, const TreeInfo &info)
{
    // used by the xml dumper to label the nodes

    std::map<parser_id, std::string> rule_names;

    rule_names[integer_const_id] = "integer_const";
    rule_names[double_const_id] = "double_const";
    rule_names[string_const_id] = "string_const";
    rule_names[constant_id] = "constant";

    rule_names[function_call_id] = "function_call";
    rule_names[function_identifier_id] = "function_identifier";

    rule_names[attrname_id] = "attrname";

    rule_names[unary_expr_id] = "unary_expr";
    rule_names[mul_expr_id] = "mul_expr";
    rule_names[add_expr_id] = "add_expr";

    rule_names[cast_expr_id] = "cast_expr";
    rule_names[cast_spec_id] = "cast_spec";

    rule_names[comp_expr_id] = "comp_expr";
    rule_names[and_expr_id] = "and_expr";
    rule_names[or_expr_id] = "or_expr";

    rule_names[expr_id] = "expr";
    rule_names[exprlist_id] = "exprlist";

    tree_to_xml(os, info.trees, input.c_str(), rule_names);
}

} // namespace Grammar

const ParseNode* parseExpressionString(const std::string &input)
{
    // instance of the grammar
    Grammar::ExpressionGrammar g;

#ifdef STX_DEBUG_PARSER
    BOOST_SPIRIT_DEBUG_GRAMMAR(g);
#endif

    Grammar::tree_parse_info<Grammar::InputIterT> info =
	boost::spirit::ast_parse(input.begin(), input.end(),
				 g.use_parser<0>(),	// use first entry point: expr
				 boost::spirit::space_p);

    if (not info.full)
    {
	std::ostringstream oss;
	oss << "Syntax error at position "
	    << static_cast<int>(info.stop - input.begin())
	    << " near " 
	    << std::string(info.stop, input.end());

	throw(BadSyntaxException(oss.str()));
    }

    return Grammar::build_expr(info.trees.begin());
}

std::string parseExpressionStringXML(const std::string &input)
{
    // instance of the grammar
    Grammar::ExpressionGrammar g;

#ifdef STX_DEBUG_PARSER
    BOOST_SPIRIT_DEBUG_GRAMMAR(g);
#endif

    Grammar::tree_parse_info<Grammar::InputIterT> info =
	boost::spirit::ast_parse(input.begin(), input.end(),
				 g.use_parser<0>(),	// use first entry point: expr
				 boost::spirit::space_p);

    if (not info.full)
    {
	std::ostringstream oss;
	oss << "Syntax error at position "
	    << static_cast<int>(info.stop - input.begin())
	    << " near " 
	    << std::string(info.stop, input.end());

	throw(BadSyntaxException(oss.str()));
    }

    std::ostringstream oss;
    Grammar::debug_dump_xml(oss, input, info);
    return oss.str();
}

std::vector<const class ParseNode*> parseExpressionListString(const std::string &input)
{
    // instance of the grammar
    Grammar::ExpressionGrammar g;

#ifdef STX_DEBUG_PARSER
    BOOST_SPIRIT_DEBUG_GRAMMAR(g);
#endif

    Grammar::tree_parse_info<Grammar::InputIterT> info =
	boost::spirit::ast_parse(input.begin(), input.end(),
				 g.use_parser<1>(),	// use first entry point: exprlist
				 boost::spirit::space_p);

    if (not info.full)
    {
	std::ostringstream oss;
	oss << "Syntax error at position "
	    << static_cast<int>(info.stop - input.begin())
	    << " near " 
	    << std::string(info.stop, input.end());

	throw(BadSyntaxException(oss.str()));
    }

    return Grammar::build_exprlist(info.trees.begin());
}

std::string ExpressionSelectorListtoString(const std::vector<const class ParseNode*> &el)
{
    std::string sl;

    for(std::vector<const class ParseNode*>::const_iterator i = el.begin(); i != el.end(); i++) {

	if (i != el.begin()) {
	    sl += ", ";
	}

	sl += (*i)->toString();
    }

    return sl;
}

/// *** BasicSymbolTable implementation

SymbolTable::~SymbolTable()
{
}

BasicSymbolTable::~BasicSymbolTable()
{
}

AnyScalar BasicSymbolTable::lookupVariable(const std::string &varname) const
{
    throw(UnknownSymbolException(std::string("Unknown variable ") + varname));
}

AnyScalar BasicSymbolTable::processFunction(const std::string &_funcname,
					    const std::vector<AnyScalar> paramlist) const
{
    std::string funcname = _funcname;
    std::transform(funcname.begin(), funcname.end(), funcname.begin(), toupper);

    if (funcname == "PI")
    {
	if (paramlist.size() != 0)
	    throw(BadFunctionCallException("Function PI() does not take any parameter"));

	return AnyScalar(M_PI);
    }
    else if (funcname == "SIN")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function SIN() takes exactly one parameter"));
	
	return AnyScalar( sin(paramlist[0].getDouble()) );
    }
    else if (funcname == "COS")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function COS() takes exactly one parameter"));
	
	return AnyScalar( cos(paramlist[0].getDouble()) );
    }
    else if (funcname == "TAN")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function COS() takes exactly one parameter"));
	
	return AnyScalar( tan(paramlist[0].getDouble()) );
    }
    else if (funcname == "ABS")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function ABS() takes exactly one parameter"));

	if (paramlist[0].isIntegerType()) {
	    return AnyScalar( abs(paramlist[0].getInteger()) );
	}
	else if (paramlist[0].isFloatingType()) {
	    return AnyScalar( fabs(paramlist[0].getDouble()) );
	}
	else {
	    throw(BadFunctionCallException("Function ABS() takes exactly one parameter"));
	}
    }
    else if (funcname == "EXP")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function EXP() takes exactly one parameter"));
	
	return AnyScalar( exp(paramlist[0].getDouble()) );
    }
    else if (funcname == "LOGN")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function LOGN() takes exactly one parameter"));
	
	return AnyScalar( log(paramlist[0].getDouble()) );
    }
    else if (funcname == "POW")
    {
	if (paramlist.size() != 2)
	    throw(BadFunctionCallException("Function POW() takes exactly two parameters"));
	
	return AnyScalar( pow(paramlist[0].getDouble(), paramlist[1].getDouble()) );
    }
    else if (funcname == "SQRT")
    {
	if (paramlist.size() != 1)
	    throw(BadFunctionCallException("Function SQRT() takes exactly one parameter"));
	
	return AnyScalar( sqrt(paramlist[0].getDouble()) );
    }

    throw(UnknownSymbolException(std::string("Unknown function ") + funcname));
}

} // namespace stx
