#ifndef PARSE_TREE_NODES_H
#define PARSE_TREE_NODES_H

#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"
#include <map>

using namespace std;
extern map<string, int> symbolTable;

// Declare all classes and functions
class programNode;
class blockNode;
class statementNode;
class assignmentNode;
class compoundNode;
class readNode;
class writeNode;
class ifNode;
class whileNode;
class expressionNode;
class nestedExprNode;
class simpleExpressionNode;
class termNode;
class factorNode;
class intLitNode;
class floatLitNode;
class identNode;
class nestedFactorNode;
ostream &operator<<(ostream &os, programNode &node);
ostream &operator<<(ostream &os, blockNode &node);
ostream &operator<<(ostream &os, statementNode &node);
ostream &operator<<(ostream &os, compoundNode &node);
ostream &operator<<(ostream &os, expressionNode &node);
ostream &operator<<(ostream &os, simpleExpressionNode &node);
ostream &operator<<(ostream &os, termNode &node);
ostream &operator<<(ostream &os, factorNode &node);

// Convert to string based on the token
string token_to_string(int token)
{
    switch (token)
    {
    case TOK_PLUS:
        return " + ";
    case TOK_MINUS:
        return " - ";
    case TOK_MULTIPLY:
        return " * ";
    case TOK_DIVIDE:
        return " / ";
    case TOK_ASSIGN:
        return " := ";
    case TOK_EQUALTO:
        return " = ";
    case TOK_LESSTHAN:
        return " < ";
    case TOK_GREATERTHAN:
        return " > ";
    case TOK_NOTEQUALTO:
        return " <> ";
    case TOK_MOD:
        return " MOD ";
    case TOK_NOT:
        return " NOT ";
    case TOK_OR:
        return " OR ";
    case TOK_AND:
        return " AND ";
    default:
        return " UNKNOWN TOKEN ";
    }
}

// constructor for factor node
class factorNode
{
public:
    virtual void printTo(ostream &os) = 0;
    virtual ~factorNode() {}
    virtual int interpret() = 0;
};

// output operator
ostream &operator<<(ostream &os, factorNode &node)
{
    node.printTo(os);
    os << " )";
    return os;
}

// constructor for intLitNode
class intLitNode : public factorNode
{
public:
    int intLiteral = 0;

    intLitNode(int value)
    {
        intLiteral = value;
    }

    void printTo(ostream &os)
    {
        os << "factor( " << intLiteral;
    }
    ~intLitNode();
    int interpret();
};

// destructor for intlitNode
intLitNode::~intLitNode()
{
    cout << "Deleting a factorNode" << endl;
}

// interpret the program
int intLitNode::interpret()
{
    return intLiteral;
}

// constructor for floatLitNode
class floatLitNode : public factorNode
{
public:
    float floatLiteral = 0.0;

    floatLitNode(float value)
    {
        floatLiteral = value;
    }

    void printTo(ostream &os)
    {
        os << "factor( " << floatLiteral;
    }
    ~floatLitNode();
    int interpret();
};
floatLitNode::~floatLitNode()
{
    cout << "Deleting a factorNode" << endl;
}
int floatLitNode::interpret()
{
    return floatLiteral;
}

// constructor for ident node
class identNode : public factorNode
{
public:
    string *id = nullptr;

    identNode(string name)
    {
        id = new string(name);
    }

    void printTo(ostream &os)
    {
        os << "factor( " << *id;
    }
    ~identNode();
    int interpret();
};

// destructor for ident node
identNode::~identNode()
{
    cout << "Deleting a factorNode" << endl;
    delete id;
    id = nullptr;
}

// interpret the program
int identNode::interpret()
{
    return symbolTable[*id];
}

// constructor for nestedFactor
class nestedFactorNode : public factorNode
{
public:
    int operand = 0;
    factorNode *factorPtr = nullptr;

    nestedFactorNode(int op, factorNode *fa)
    {
        this->operand = op;
        factorPtr = fa;
    }
    void printTo(ostream &os)
    {
        os << "factor( ";
        if (operand == TOK_NOT)
            os << "NOT ";
        else
            os << "- ";

        os << *factorPtr;
    }
    ~nestedFactorNode();
    int interpret();
};

// destructor for factor node
nestedFactorNode::~nestedFactorNode()
{
    cout << "Deleting a factorNode" << endl;

    delete factorPtr;
    factorPtr = nullptr;
}

// interpret the factor program
int nestedFactorNode::interpret()
{
    if (operand == TOK_NOT)
        return !factorPtr->interpret();
    else
        return -1 * factorPtr->interpret();
}

// constructor for termNode
class termNode
{
public:
    factorNode *factorPtr = nullptr;

    termNode(factorNode *pFact)
    {
        factorPtr = pFact;
    }
    vector<int> restFactorOps;
    vector<factorNode *> restFactors;
    ~termNode();
    int interpret();
};

// input operator
ostream &operator<<(ostream &os, termNode &node)
{
    os << "term( ";
    os << *(node.factorPtr);

    int length = node.restFactorOps.size();
    for (int i = 0; i < length; ++i)
    {
        int op = node.restFactorOps[i];
        if (op == TOK_MULTIPLY)
            os << " * ";
        else if (op == TOK_DIVIDE)
            os << " / ";
        else
            os << " AND ";
        os << *(node.restFactors[i]);
    }

    os << " )";
    return os;
}

// destructor for term node
termNode::~termNode()
{
    cout << "Deleting a termNode" << endl;
    delete factorPtr;
    factorPtr = nullptr;

    int length = restFactorOps.size();
    for (int i = 0; i < length; ++i)
    {
        delete restFactors[i];
        restFactors[i] = nullptr;
    }
}

// interpret the program
int termNode::interpret()
{
    int result = 0;
    result = factorPtr->interpret();
    int length = restFactorOps.size();
    for (int i = 0; i < length; ++i)
    {
        int opCode = restFactorOps[i];
        if (opCode == TOK_MULTIPLY)
            result *= restFactors[i]->interpret();
        else if (opCode == TOK_DIVIDE)
            result /= restFactors[i]->interpret();
        else if (opCode == TOK_AND)
            result = result && restFactors[i]->interpret();
    }
    return result;
}

// constructor for simpleExpressionNode
class simpleExpressionNode
{
public:
    termNode *termPtr = nullptr;

    simpleExpressionNode(termNode *pTerm)
    {
        termPtr = pTerm;
    }

    vector<int> restTermOps;
    vector<termNode *> restTerms;
    ~simpleExpressionNode();
    int interpret();
};

// input operator
ostream &operator<<(ostream &os, simpleExpressionNode &node)
{
    os << "simple_expression( ";
    os << *(node.termPtr);

    int length = node.restTermOps.size();
    for (int i = 0; i < length; ++i)
    {
        int op = node.restTermOps[i];
        if (op == TOK_PLUS)
            os << " + ";
        else if (op == TOK_MINUS)
            os << " - ";
        else
            os << " 0R ";
        os << *(node.restTerms[i]);
    }
    os << " )";
    return os;
}

// destructor for simple expression
simpleExpressionNode::~simpleExpressionNode()
{
    cout << "Deleting a simpleExpressionNode" << endl;
    delete termPtr;
    termPtr = nullptr;

    int length = restTermOps.size();
    for (int i = 0; i < length; ++i)
    {
        delete restTerms[i];
        restTerms[i] = nullptr;
    }
}

// interpret the simple expression
int simpleExpressionNode::interpret()
{
    int result = 0;
    result = termPtr->interpret();
    int length = restTermOps.size();
    for (int i = 0; i < length; ++i)
    {
        int opCode = restTermOps[i];
        if (opCode == TOK_PLUS)
            result += restTerms[i]->interpret();
        else if (opCode == TOK_MINUS)
            result -= restTerms[i]->interpret();
        else if (opCode == TOK_OR)
            result = result || restTerms[i]->interpret();
    }
    return result;
}

// constructor for expression node
class expressionNode
{
public:
    simpleExpressionNode *simpleExpression1Ptr = nullptr;
    simpleExpressionNode *simpleExpression2Ptr = nullptr;
    int operand;

    expressionNode(simpleExpressionNode *pSimp1)
    {
        simpleExpression1Ptr = pSimp1;
    }

    expressionNode(simpleExpressionNode *pSimp1, int opCode, simpleExpressionNode *pSimp2)
    {
        simpleExpression1Ptr = pSimp1;
        operand = opCode;
        simpleExpression2Ptr = pSimp2;
    }
    ~expressionNode();
    int interpret();
};

// input operator
ostream &operator<<(ostream &os, expressionNode &node)
{
    os << *(node.simpleExpression1Ptr);

    if (node.operand == TOK_EQUALTO)
        os << " = ";
    else if (node.operand == TOK_LESSTHAN)
        os << " < ";
    else if (node.operand == TOK_GREATERTHAN)
        os << " > ";
    else if (node.operand == TOK_NOTEQUALTO)
        os << " <> ";

    if (node.simpleExpression2Ptr != nullptr)
        os << *(node.simpleExpression2Ptr);
    return os;
}

// destructor for expression node
expressionNode::~expressionNode()
{
    cout << "Deleting an expressionNode" << endl;
    delete simpleExpression1Ptr;
    simpleExpression1Ptr = nullptr;
    delete simpleExpression2Ptr;
    simpleExpression2Ptr = nullptr;
}

// Interpret the program based on operand
int expressionNode::interpret()
{
    int result;
    result = simpleExpression1Ptr->interpret();

    if (operand == TOK_EQUALTO)
    {
        if (result == simpleExpression2Ptr->interpret())
            result = 1;
        else
            result = 0;
    }
    else if (operand == TOK_LESSTHAN)
    {
        if (result < simpleExpression2Ptr->interpret())
        {
            result = 1;
        }
        else
            result = 0;
    }
    else if (operand == TOK_GREATERTHAN)
    {
        if (result > simpleExpression2Ptr->interpret())
            result = 1;
        else
            result = 0;
    }
    else if (operand == TOK_NOTEQUALTO)
    {
        if (result != simpleExpression2Ptr->interpret())
            result = 1;
        else
            result = 0;
    }
    return result;
}

// constructor for nestedExprNode
class nestedExprNode : public factorNode
{
public:
    expressionNode *expressionPtr = nullptr;

    nestedExprNode(expressionNode *ex)
    {
        expressionPtr = ex;
    }
    void printTo(ostream &os)
    {
        os << "nested_expression( expression( " << *expressionPtr;
        os << " )";
    }
    ~nestedExprNode();
    int interpret();
};

// destructor for nestedExpr Node
nestedExprNode::~nestedExprNode()
{
    cout << "Deleting a factorNode" << endl;

    delete expressionPtr;
    expressionPtr = nullptr;
}

// return interpret 
int nestedExprNode::interpret()
{
    return expressionPtr->interpret();
}

// constructors, destructors and methods for nodes below:

// constructor for statement node
class statementNode
{
public:
    virtual void printTo(ostream &os) = 0;
    virtual ~statementNode() {}
    virtual int interpret() = 0;
};

// output operator
ostream &operator<<(ostream &os, statementNode &node)
{
    node.printTo(os);
    return os;
}

// constructor for assignment node
class assignmentNode : public statementNode
{
public:
    string *name = nullptr;
    expressionNode *expressionPtr = nullptr;

    assignmentNode(string id, expressionNode *expr)
    {
        name = new string(id);
        expressionPtr = expr;
    }

    void printTo(ostream &os)
    {
        os << "Assignment " << *name << " := expression( ";
        os << *expressionPtr << " )" << endl;
    }
    ~assignmentNode();
    int interpret();
};

// destructor for assignment node
assignmentNode::~assignmentNode()
{
    cout << "Deleting an assignmentNode" << endl;
    delete name;
    name = nullptr;
    delete expressionPtr;
    expressionPtr = nullptr;
}

// interpret the program
int assignmentNode::interpret()
{
    return symbolTable[*name] = expressionPtr->interpret();
}

// constructor for read node
class readNode : public statementNode
{
public:
    string *name = nullptr;

    readNode(string id)
    {
        this->name = new string(id);
    }
    void printTo(ostream &os)
    {
        os << "Read Value " << *name << endl;
    }
    ~readNode();
    int interpret();
};

// destructor for read node
readNode::~readNode()
{
    cout << "Deleting a readNode" << endl;
    delete name;
    name = nullptr;
}

// interpret the program
int readNode::interpret()
{
    int inputNumber;
    cin >> inputNumber;
    symbolTable[*name] = inputNumber;
    return inputNumber;
}


// contructor for write node
class writeNode : public statementNode
{
public:
    string *name = nullptr;
    int tokenNumber = 0;

    writeNode(string id, int tokenNum)
    {
        name = new string(id);
        tokenNumber = tokenNum;
    }
    void printTo(ostream &os)
    {
        if (tokenNumber == TOK_STRINGLIT)
            os << "Write String " << *name << endl;
        else
            os << "Write Value " << *name << endl;
    }
    ~writeNode();
    int interpret();
};

// destructor for write node
writeNode::~writeNode()
{
    cout << "Deleting a writeNode" << endl;
    delete name;
    name = nullptr;
}

//interpret the symboltable
int writeNode::interpret()
{
    if (this->tokenNumber == TOK_IDENT)
    {
        cout << symbolTable.at(*name) << endl;
    }
    else
    {
        string consoleOut = *name;
        consoleOut.erase(consoleOut.begin());
        consoleOut.pop_back();
        cout << consoleOut << endl;
    }
    return 0;
}

// constructor for ifnode
class ifNode : public statementNode
{
public:
    expressionNode *expressionPtr = nullptr;
    statementNode *thenStatementPtr = nullptr;
    statementNode *elseStatementPtr = nullptr;

    ifNode(expressionNode *expr, statementNode *thenState)
    {
        expressionPtr = expr;
        thenStatementPtr = thenState;
    }
    ifNode(expressionNode *expr, statementNode *thenState, statementNode *elseState)
    {
        expressionPtr = expr;
        thenStatementPtr = thenState;
        elseStatementPtr = elseState;
    }
    void printTo(ostream &os)
    {
        os << "If expression( ";
        os << *expressionPtr << " )" << endl;

        os << "%%%%%%%% True Statement %%%%%%%%" << endl;
        os << *thenStatementPtr;
        os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";

        if (elseStatementPtr != nullptr)
        {
            os << "%%%%%%%% False Statement %%%%%%%%" << endl;
            os << *elseStatementPtr;
            os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
        }
    }
    ~ifNode();
    int interpret();
};

// destructor for if node
ifNode::~ifNode()
{
    cout << "Deleting an ifNode" << endl;
    delete expressionPtr;
    expressionPtr = nullptr;
    delete thenStatementPtr;
    thenStatementPtr = nullptr;
    if (elseStatementPtr != nullptr)
    {
        delete elseStatementPtr;
        elseStatementPtr = nullptr;
    }
}

//interpret the expression
int ifNode::interpret()
{
    int result = expressionPtr->interpret();
    if (result != 0)
        return thenStatementPtr->interpret();
    if (elseStatementPtr != nullptr)
        return elseStatementPtr->interpret();
    return 0;
}

// constructor for while node
class whileNode : public statementNode
{
public:
    expressionNode *expressionPtr = nullptr;
    statementNode *loopBody = nullptr;

    whileNode(expressionNode *expr, statementNode *loopB)
    {
        expressionPtr = expr;
        loopBody = loopB;
    }
    void printTo(ostream &os)
    {
        os << "While expression( ";
        os << *expressionPtr << " )" << endl;

        os << "%%%%%%%% Loop Body %%%%%%%%" << endl;
        os << *loopBody;

        os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    }
    ~whileNode();
    int interpret();
};

// destructor for while node
whileNode::~whileNode()
{
    cout << "Deleting a whileNode" << endl;
    delete expressionPtr;
    expressionPtr = nullptr;
    delete loopBody;
    loopBody = nullptr;
}

// Interpret the expression
int whileNode::interpret()
{
    while (expressionPtr->interpret())
    {
        loopBody->interpret();
    }
    return 0;
}

// contructor for compoundNode
class compoundNode : public statementNode
{
public:
    statementNode *statementPtr = nullptr;

    compoundNode(statementNode *mystate)
    {
        statementPtr = mystate;
    }
    vector<statementNode *> restStatements;
    void printTo(ostream &os)
    {
        os << "Begin Compound Statement" << endl;
        os << *statementPtr;
        int length = restStatements.size();
        for (int i = 0; i < length; ++i)
        {
            statementNode *sts = restStatements[i];
            os << *sts;
        }
        os << "End Compound Statement" << endl;
    }
    ~compoundNode();
    int interpret();
};

// output operator
ostream &operator<<(ostream &os, compoundNode &node)
{
    node.printTo(os);
    return os;
}

// compund node destructor
compoundNode::~compoundNode()
{
    cout << "Deleting a compoundNode" << endl;
    delete statementPtr;
    statementPtr = nullptr;
    int length = restStatements.size();
    for (int i = 0; i < length; ++i)
    {
        delete restStatements[i];
        restStatements[i] = nullptr;
    }
}

// interpret the statement
int compoundNode::interpret()
{
    statementPtr->interpret();
    int length = restStatements.size();
    for (int i = 0; i < length; ++i)
    {
        restStatements[i]->interpret();
    }
    return 0;
}


class blockNode
{
public:
    compoundNode *compoundStatement = nullptr;

    blockNode(compoundNode *compoundPtr)
    {
        this->compoundStatement = compoundPtr;
    }
    ~blockNode();
    int interpret();
};

// input operator
ostream &operator<<(ostream &os, blockNode &node)
{
    os << *(node.compoundStatement);
    return os;
}


// delete a node
blockNode::~blockNode()
{
    cout << "Deleting a blockNode" << endl;
    delete compoundStatement;
    compoundStatement = nullptr;
}
int blockNode::interpret()
{
    return compoundStatement->interpret();
}

// class for program node
class programNode
{
public:
    string *name = nullptr;
    blockNode *block = nullptr;

    programNode(string id, blockNode *blk)
    {
        name = new string(id);
        block = blk;
    }
    ~programNode();
    int interpret();
};

// input operator
ostream &operator<<(ostream &os, programNode &node)
{
    os << "Program Name " << *(node.name) << endl;
    os << *(node.block) << endl;
    return os;
}

// delete program node
programNode::~programNode()
{
    cout << "Deleting a programNode" << endl;
    delete name;
    name = nullptr;
    delete block;
    block = nullptr;
}

int programNode::interpret()
{
    return block->interpret();
}

//----------------------------------------------------------------//

#endif