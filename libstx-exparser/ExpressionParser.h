// $Id$

#ifndef _STX_ExpressionParser_H_
#define _STX_ExpressionParser_H_

#include <string>
#include <vector>
#include <map>
#include "AnyScalar.h"

/// STX - Some Template Extensions namespace
namespace stx {

/** \ingroup Exception
 * Base class for other exceptions of the expression parser and evaluators. */
class ExpressionParserException : public std::runtime_error
{
public:
    /// Construct with a description string.
    inline ExpressionParserException(const std::string &s) throw()
	: std::runtime_error(s)
    { }
};

/** \ingroup Exception
 * BadSyntaxException is an exception class thrown when the parser
 * recognizes a syntax error. */

class BadSyntaxException : public ExpressionParserException
{
public:
    /// Construct with a description string.
    inline BadSyntaxException(const std::string &s) throw()
	: ExpressionParserException(s)
    { }
};

/** \ingroup Exception
 * UnknownSymbolException is an exception class thrown when the symbol table
 * cannot find a variable or function. */

class UnknownSymbolException : public ExpressionParserException
{
public:
    /// Construct with a description string.
    inline UnknownSymbolException(const std::string &s) throw()
	: ExpressionParserException(s)
    { }
};

/** \ingroup Exception
 * BadFunctionCallException is an exception class thrown when the symbol table
 * cannot find a variable or function. */

class BadFunctionCallException : public ExpressionParserException
{
public:
    /// Construct with a description string.
    inline BadFunctionCallException(const std::string &s) throw()
	: ExpressionParserException(s)
    { }
};

/// Abstract class used for evaluation of variables and function placeholders
/// within an expression. If you wish some standard mathematic function, then
/// derive your SymbolTable class from BasicSymbolTable instead of directly
/// from this one.
class SymbolTable
{
public:
    /// STL container type used for parameter lists: a vector
    typedef std::vector<AnyScalar>	paramlist_type;

    /// Required for virtual functions.
    virtual ~SymbolTable();

    /// Return the (constant) value of a variable.
    virtual AnyScalar	lookupVariable(const std::string &varname) const = 0;

    /// Called when a program-defined function needs to be evaluated within an
    /// expression.
    virtual AnyScalar	processFunction(const std::string &funcname,
					const paramlist_type &paramlist) const = 0;
};

/// Class representing variables and functions placeholders within an
/// expression. This base class contain two tables of variables and
/// functions. Variables may be filled into the STL map by the program. The
/// class also contains a set of basic mathematic functions.
class BasicSymbolTable : public SymbolTable
{
public:
    /// Signature of a function used in the symbol table.
    typedef AnyScalar	(*functionptr_type)(const paramlist_type& paramlist);

protected:

    /// Container used to save a map of variable names
    typedef std::map<std::string, AnyScalar>	variablemap_type;

    /// Extra info about a function: the valid arguments.
    struct FunctionInfo
    {
	/// Number of arguments this function takes: either >= 0 for a fixed
	/// number of -1 for no checking.
	int		arguments;

	/// Function pointer to call.
	functionptr_type func;

	/// Initializing Constructor
	FunctionInfo(int _arguments = 0, functionptr_type _func = NULL)
	    : arguments(_arguments), func(_func)
	{
	}
    };

    /// Container used to save a map of function names
    typedef std::map<std::string, struct FunctionInfo>	functionmap_type;

private:
    /// Variable map which can be filled by the user-application
    variablemap_type	variablemap;

    /// Function map used to lookup standard or user-added function
    functionmap_type	functionmap;

protected:
    // *** Lots of Standard Functions

    /// Return the value of PI as a double AnyScalar
    static AnyScalar	funcPI(const paramlist_type& paramlist);

    /// Return the value of sin(x) as a double AnyScalar
    static AnyScalar	funcSIN(const paramlist_type& paramlist);

    /// Return the value of cos(x) as a double AnyScalar
    static AnyScalar	funcCOS(const paramlist_type& paramlist);

    /// Return the value of tan(x) as a double AnyScalar
    static AnyScalar	funcTAN(const paramlist_type& paramlist);

    /// Return the value of abs(x) or fabs(f)
    static AnyScalar	funcABS(const paramlist_type& paramlist);

    /// Return the value of exp(x) as a double AnyScalar
    static AnyScalar	funcEXP(const paramlist_type& paramlist);

    /// Return the value of log(x) as a double AnyScalar
    static AnyScalar	funcLOGN(const paramlist_type& paramlist);

    /// Return the value of pow(x,y) as a double AnyScalar
    static AnyScalar	funcPOW(const paramlist_type& paramlist);

    /// Return the value of sqrt(x) as a double AnyScalar
    static AnyScalar	funcSQRT(const paramlist_type& paramlist);

public:
    /// Fills in the functionmap with the standard functions.
    BasicSymbolTable();

    /// Required for virtual functions.
    virtual ~BasicSymbolTable();

    /// Return the (constant) value of a variable. In this basic implementation
    /// no variables are defined, it always throws an UnknownSymbolException.
    virtual AnyScalar	lookupVariable(const std::string &varname) const;

    /// Called when a program-defined function needs to be evaluated within an
    /// expression.
    virtual AnyScalar	processFunction(const std::string &funcname,
					const paramlist_type &paramlist) const;

    /// Add or replace a variable to the symbol table
    void	setVariable(const std::string& varname, const AnyScalar &value);

    /// Add or replace a function to the symbol table
    void	setFunction(const std::string& funcname, int arguments, functionptr_type funcptr);

    /// Clear variables table
    void	clearVariables();

    /// Clear function table
    void	clearFunctions();

    /// Add set of standard mathematic functions
    void	addStandardFunctions();
};

/** ParseNode is the abstract node interface of different parse nodes. From
 * these parse nodes the the ExpressionParser constructs a tree which can be
 * evaluated using different SymbolTable settings. */

class ParseNode
{
protected:
    /// Usual construction
    inline ParseNode()
    { }

    /// Disable copy construction
    ParseNode(const ParseNode &pn);

    /// And disable assignment
    ParseNode& operator=(const ParseNode &pn);
    
public:
    /// Virtual destructor so derived classes can deallocate their children
    /// nodes.
    virtual ~ParseNode()
    {
    }

    /// Function to recursively evaluate the contained parse tree and retrieve
    /// the calculated scalar value based on the given symbol table.
    virtual AnyScalar evaluate(const class SymbolTable &st = BasicSymbolTable()) const = 0;

    /// (Internal) Function to check if the subtree evaluates to a constant
    /// expression. If dest == NULL then do a static check whether the node is
    /// always a constant (ignoring subnodes), if dest != NULL try to calculate
    /// the constant value and type recursively, thus the return value can be
    /// true for a non-constant tree node.
    virtual bool evaluate_const(AnyScalar *dest) const = 0;

    /// Return the parsed expression as a string, which can be parsed again.
    virtual std::string toString() const = 0;

    /// virtual copy construtor
    virtual ParseNode*	clone() const = 0;
};

/// Parse the given input expression into a parse tree. The parse tree is
/// represented by it's root node, which can be evaluated.
const ParseNode* parseExpressionString(const std::string &input);

/// Parse the given input expression into a parse tree. The parse tree is then
/// transformed into a XML tree for better visualisation.
std::string parseExpressionStringXML(const std::string &input);

} // namespace stx

#endif // _STX_ExpressionParser_H_
