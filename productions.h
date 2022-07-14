
#ifndef PRODUCTIONS_H
#define PRODUCTIONS_H

#include <iostream>
extern map<string, int> symbolTable;

int nextToken = 0;
int level = 0;

extern "C"
{

    extern char *yytext; // text of current lexeme
    extern int yylex();  // the generated lexical analyzer
}

// Declare all functions
bool first_of_program();
programNode *program();
blockNode *block();
compoundNode *compoundStatement();
statementNode *statement();
assignmentNode *assignmentStatement();
readNode *read();
writeNode *write();
ifNode *ifStatement();
whileNode *whileStatement();
expressionNode *expression();
simpleExpressionNode *simpleExpression();
termNode *term();
factorNode *factor();





inline void indent()
{
    for (int i = 0; i < level; i++)
        cout << ("    ");
}

void output()
{
    indent();
    cout << "-->found " << yytext << endl;
}
int lex()
{
    nextToken = yylex();
    if (nextToken == TOK_EOF)
    {
        yytext[0] = 'E';
        yytext[1] = 'O';
        yytext[2] = 'F';
        yytext[3] = 0;
    }
    return nextToken;
}

programNode *program()
{
    programNode *programPtr = nullptr;
    if (!first_of_program())
        throw "3: 'PROGRAM' expected";

    indent();
    cout << "enter <program>\n";

    ++level;
    output();
    lex();

    if (nextToken == TOK_IDENT)
    {
        string tempstr = yytext;
        output();
        lex();

        if (nextToken == TOK_SEMICOLON)
        {
            output();
            //block();
            programPtr = new programNode(tempstr, block());
        }

        else
            throw "14: ';' expected";
    }

    else
        throw "2: identifier expected";

    --level;
    indent();
    cout << "exit <program>\n";
    return programPtr;
}

blockNode *block()
{
    blockNode *blockPtr = nullptr;
    bool isBegin = false;
    indent();
    cout << "enter <block>\n";

    ++level;
    lex();

    if (nextToken == TOK_VAR)
    {
        output();
        lex();
        do
        {
            if (nextToken == TOK_IDENT)
            {

                if (symbolTable.count(yytext))
                    throw "101: identifier declared twice";
                symbolTable.insert(pair<string, int>(yytext, 0));

                output();
                lex();

                if (nextToken == TOK_COLON)
                {
                    output();
                    lex();

                    if (nextToken == TOK_INTEGER || nextToken == TOK_REAL)
                    {
                        output();
                        lex();

                        if (nextToken == TOK_SEMICOLON)
                        {
                            output();
                            lex();

                            cout << endl;
                            if (nextToken == TOK_IDENT)
                            {

                                isBegin = false;
                            }
                            else if (nextToken == TOK_BEGIN)
                            {
                                isBegin = true;
                                //compoundStatement();
                                blockPtr = new blockNode(compoundStatement());
                            }
                            else
                                throw "17: 'BEGIN' expected";
                        }
                        else
                            throw "14: ';' expected";
                    }
                    else
                        throw "10: error in type";
                }
                else
                    throw "5: ':' expected";
            }
            else
                throw "2: identifier expected";
        } while (isBegin == false);
    }
    else if (nextToken == TOK_BEGIN)
    {
        //compoundStatement();
        blockPtr = new blockNode(compoundStatement());
    }
    else
        throw "18: error in declaration part OR 17: 'BEGIN' expected";

    --level;
    indent();
    cout << "exit <block>\n";
    return blockPtr;
}

compoundNode *compoundStatement()
{
    compoundNode *compoundPtr = nullptr;
    indent();
    cout << "enter <compound_statement>\n";
    ++level;
    output();
    lex();
    //statement();
    compoundPtr = new compoundNode(statement());
    while (nextToken == TOK_SEMICOLON)
    {
        output();
        lex();
        //statement();
        compoundPtr->restStatements.push_back(statement());
    }
    if (nextToken == TOK_END)
    {
        output();
        lex();
    }
    else
        throw "13: 'END' expected";
    --level;
    indent();
    cout << "exit <compound_statement>\n";
    return compoundPtr;
}

statementNode *statement()
{
    statementNode *statementPtr = nullptr;
    indent();
    cout << "enter <statement>\n";
    ++level;
    if (nextToken == TOK_IDENT)
    {

        //assignmentStatement();
        statementPtr = assignmentStatement();
    }
    else if (nextToken == TOK_BEGIN)
    {

        //compoundStatement();
        statementPtr = compoundStatement();
    }
    else if (nextToken == TOK_READ)
    {

        //read();
        statementPtr = read();
    }
    else if (nextToken == TOK_WRITE)
    {

        //write();
        statementPtr = write();
    }
    else if (nextToken == TOK_IF)
    {

        //ifStatement();
        statementPtr = ifStatement();
    }
    else if (nextToken == TOK_WHILE)
    {

        //whileStatement();
        statementPtr = whileStatement();
    }

    else
        throw "900: illegal type of statement";

    --level;
    indent();
    cout << "exit <statement>\n";
    return statementPtr;
}

assignmentNode *assignmentStatement()
{
    assignmentNode *assignmentPtr = nullptr;
    indent();
    cout << "enter <assignment>\n";

    if (!symbolTable.count(yytext))
        throw "104: identifier not declared";
    string tempstr = yytext;
    ++level;
    output();
    lex();

    if (nextToken == TOK_ASSIGN)
    {
        output();
        lex();

        // expression();
        assignmentPtr = new assignmentNode(tempstr, expression());

        --level;
        indent();

        cout << "exit <assignment>\n";
    }
    else
        throw "51: ':=' expected";

    return assignmentPtr;
}
readNode *read()
{
    readNode *readPtr = nullptr;
    indent();
    cout << "enter <read>\n";

    ++level;
    output();
    lex();

    if (nextToken == TOK_OPENPAREN)
    {
        output();
        lex();

        if (nextToken == TOK_IDENT)
        {

            if (!symbolTable.count(yytext))
                throw "104: identifier not declared";

            readPtr = new readNode(yytext);
            output();
            lex();

            if (nextToken == TOK_CLOSEPAREN)
            {

                output();
                lex();
            }

            else
                throw "4: ')' expected";
        }

        else
            throw "2: identifier expected";
    }

    else
        throw "9: '(' expected";

    --level;
    indent();
    cout << "exit <read>\n";

    return readPtr;
}

writeNode *write()
{
    writeNode *writePtr = nullptr;
    indent();
    cout << "enter <write>\n";

    ++level;
    output();
    lex();

    if (nextToken == TOK_OPENPAREN)
    {
        output();
        lex();

        if (nextToken == TOK_IDENT)
        {

            if (!symbolTable.count(yytext))
                throw "104: identifier not declared";
            writePtr = new writeNode(yytext, nextToken);
            output();
            lex();
        }

        else if (nextToken == TOK_STRINGLIT)
        {

            output();
            writePtr = new writeNode(yytext, nextToken);
            lex();
        }

        else
            throw "134: illegal type of operand(s)";

        if (nextToken == TOK_CLOSEPAREN)
        {

            output();
            lex();
        }

        else
            throw "4: ')' expected";
    }

    else
        throw "9: '(' expected";

    --level;
    indent();
    cout << "exit <write>\n";

    return writePtr;
}

ifNode *ifStatement()
{
    ifNode *ifPtr = nullptr;
    indent();
    cout << "enter <if statement>\n";

    ++level;
    output();
    lex();

    // expression();
    expressionNode *expressionPtr = expression();

    if (nextToken == TOK_THEN)
    {
        output();
        lex();

        // statement();
        statementNode *statementPtr = statement();
        ifPtr = new ifNode(expressionPtr, statementPtr);

        if (nextToken == TOK_ELSE)
        {
            output();
            lex();
            //statement();
            ifPtr = new ifNode(expressionPtr, statementPtr, statement());
        }
    }

    else
        throw "52: 'THEN' expected";

    --level;
    indent();
    cout << "exit <if statement>\n";

    return ifPtr;
}

whileNode *whileStatement()
{
    whileNode *whilePtr = nullptr;

    indent();
    cout << "enter <while statement>\n";

    ++level;
    output();
    lex();

    //expression();
    expressionNode *pEx = expression();

    //statement();
    whilePtr = new whileNode(pEx, statement());

    --level;
    indent();
    cout << "exit <while statement>\n";
    return whilePtr;
}
expressionNode *expression()
{
    expressionNode *expressionPtr = nullptr;
    indent();
    cout << "enter <expression>\n";
    ++level;

    if (nextToken != TOK_INTLIT && nextToken != TOK_FLOATLIT && nextToken != TOK_IDENT && nextToken != TOK_OPENPAREN && nextToken != TOK_NOT && nextToken != TOK_MINUS)
    {
        throw "144: illegal type of expression";
    }

    //simpleExpression();
    simpleExpressionNode *simpleExpressionPtr = simpleExpression();
    expressionPtr = new expressionNode(simpleExpressionPtr);

    if (nextToken == TOK_EQUALTO || nextToken == TOK_LESSTHAN || nextToken == TOK_GREATERTHAN || nextToken == TOK_NOTEQUALTO)
    {

        output();
        int x = nextToken;
        lex();

        // simpleExpression();
        expressionPtr = new expressionNode(simpleExpressionPtr, x, simpleExpression());
    }

    --level;
    indent();
    cout << "exit <expression>\n";
    return expressionPtr;
}

simpleExpressionNode *simpleExpression()
{
    simpleExpressionNode *simpleExpressionPtr = nullptr;
    indent();
    cout << "enter <simple expression>\n";

    ++level;
    if (nextToken != TOK_INTLIT && nextToken != TOK_FLOATLIT && nextToken != TOK_IDENT && nextToken != TOK_OPENPAREN && nextToken != TOK_NOT && nextToken != TOK_MINUS)
    {
        throw "901: illegal type of simple expression";
    }

    // term();
    simpleExpressionPtr = new simpleExpressionNode(term());

    while (nextToken == TOK_PLUS || nextToken == TOK_MINUS || nextToken == TOK_OR)
    {

        output();
        simpleExpressionPtr->restTermOps.push_back(nextToken);
        lex();

        // term();
        simpleExpressionPtr->restTerms.push_back(term());
    }

    --level;
    indent();
    cout << "exit <simple expression>\n";
    return simpleExpressionPtr;
}

termNode *term()
{
    termNode *termPtr = nullptr;
    indent();
    cout << "enter <term>\n";
    ++level;

    if (nextToken != TOK_INTLIT && nextToken != TOK_FLOATLIT && nextToken != TOK_IDENT && nextToken != TOK_OPENPAREN && nextToken != TOK_NOT && nextToken != TOK_MINUS)
    {
        throw "902: illegal type of term";
    }

    // factor();
    termPtr = new termNode(factor());

    while (nextToken == TOK_MULTIPLY || nextToken == TOK_DIVIDE || nextToken == TOK_AND)
    {
        output();
        termPtr->restFactorOps.push_back(nextToken);
        lex();
        //factor();
        termPtr->restFactors.push_back(factor());
    }

    --level;
    indent();
    cout << "exit <term>\n";
    return termPtr;
}

factorNode *factor()
{
    factorNode *factorPtr = nullptr;
    indent();
    cout << "enter <factor>\n";
    ++level;

    if (nextToken == TOK_INTLIT)
    {
        factorPtr = new intLitNode(stoi(yytext));
        output();
        lex();
    }

    else if (nextToken == TOK_FLOATLIT)
    {
        factorPtr = new floatLitNode(stof(yytext));
        output();
        lex();
    }

    else if (nextToken == TOK_IDENT)
    {

        if (!symbolTable.count(yytext))
            throw "104: identifier not declared";
        factorPtr = new identNode(yytext);
        output();
        lex();
    }

    else if (nextToken == TOK_OPENPAREN)
    {

        output();
        lex();

        // expression();
        factorPtr = new nestedExprNode(expression());

        if (nextToken == TOK_CLOSEPAREN)
        {

            output();
            lex();
        }

        else
            throw "4: ')' expected";
    }

    else if (nextToken == TOK_NOT)
    {
        int x = nextToken;
        output();
        lex();

        //factor();
        factorPtr = new nestedFactorNode(x, factor());
    }

    else if (nextToken == TOK_MINUS)
    {
        int x = nextToken;
        output();
        lex();

        //factor();
        factorPtr = new nestedFactorNode(x, factor());
    }

    else
        throw "903: illegal type of factor";

    --level;
    indent();
    cout << "exit <factor>\n";
    return factorPtr;
}

bool first_of_program(void)
{
    return nextToken == TOK_PROGRAM;
}

#endif
