
#include "lexer.h"
#include "parse_tree_nodes.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

extern "C"
{
  extern FILE *yyin;         // input stream
  extern int   yylex();      // the generated lexical analyzer
  extern char *yytext;       // text of current lexeme
}

int nextToken = 0;
int level = 0;

// Function declarations 
int lex();
void indent();
void output();

ExprNode* expr();
bool first_of_expr();
TermNode* term();
bool first_of_term();
FactorNode* factor();
bool first_of_factor();
void error();

// Provide 4 spaces of indent
inline void indent(){
    for (int i = 0; i<level; i++)
        cout << ("    ");
}

// Print a found message
void output() {
  indent();
  cout << "---> FOUND " << yytext << endl;
}

// Get the next token
int lex() {
  nextToken = yylex();

  if (nextToken == TOK_EOF) {
      // save a "lexeme" into yytext
      yytext[0] = 'E';
      yytext[1] = 'O';
      yytext[2] = 'F';
      yytext[3] = 0;
  }

  indent();
  cout << "Next token is: " << nextToken;
  cout << ", Next lexeme is: " << yytext << endl;
  return nextToken;
}

/* expr = <expr> -> <term> {(+ | -) <term>}
*/
ExprNode* expr() {
  /* Check that the <expr> starts with a valid token */
  if (!first_of_expr())
    error();

  indent();
  cout << "Enter <expr>" << endl;
  level = level + 1;
  ExprNode* newExprNode = new ExprNode;

  // Parse the first term
  newExprNode->firstTerm = term();

  // Get the next token
  while (nextToken == TOK_ADD_OP || nextToken == TOK_SUB_OP) {
    output();
    newExprNode->restTermOps.push_back(nextToken);
    lex();
    newExprNode->restTerms.push_back(term());
  }

  level = level - 1;
  indent();
  cout << "Exit <expr>" << endl;

  return newExprNode;
} 


// Get the first token in the expression
bool first_of_expr(){
  return nextToken == TOK_LEFT_PAREN || nextToken == TOK_IDENT || nextToken == TOK_INT_LIT;
}

// term = <term> -> <factor> {(* | /) <factor>)

TermNode* term() {
  // Display error if token is invalid
  if (!first_of_term())
    error();

  indent();
  cout << "Enter <term>" << endl;
  level = level + 1;
  TermNode* newTermNode = new TermNode;

  newTermNode->firstFactor = factor(); // First factor

  // Get the next token
  while (nextToken == TOK_MULT_OP || nextToken == TOK_DIV_OP) {
    output();
    newTermNode->restFactorOps.push_back(nextToken);
    lex();
    newTermNode->restFactors.push_back(factor());
  }

  level = level - 1;
  indent();
  cout << "Exit <term>" << endl;

  return newTermNode;
} 

// Get the first of term token
bool first_of_term(){
  return nextToken == TOK_LEFT_PAREN || nextToken == TOK_IDENT || nextToken == TOK_INT_LIT;
}

// factor =  <factor> -> id | int_constant | ( <expr )

FactorNode* factor() {
  if (!first_of_factor())
    error();

  indent();
  cout << "Enter <factor>" << endl;
  level = level + 1;
  FactorNode* newFactorNode = nullptr;

  // Get next token and check what we have
  switch (nextToken) {

    case TOK_IDENT:
      output();
      newFactorNode = new IdNode(string(yytext));
      nextToken = yylex(); // Read next
      break;

    case TOK_INT_LIT:
      output();
      newFactorNode = new IntLitNode(atoi(yytext));
      nextToken = yylex();
      break;

    case TOK_LEFT_PAREN:
      output();
      nextToken = yylex();
      if (!first_of_expr()) // Check for grammar
        error();

      newFactorNode = new NestedExprNode(expr());

      if (nextToken == TOK_RIGHT_PAREN) {
        output();
        nextToken = yylex();
      }
      else
        throw "<expr> does not end with )";
      break;

    default:
      error();
  }
  level = level - 1;
  indent();
  cout << "Exit <factor>" << endl;

  return newFactorNode;
} 

bool first_of_factor(){
  return nextToken == TOK_LEFT_PAREN || nextToken == TOK_IDENT || nextToken == TOK_INT_LIT;
}

void error() {
  cout << endl << "===========================" << endl;
  cout << "ERROR near: " << yytext;
  cout << endl << "===========================" << endl;
  if (yyin)
    fclose(yyin);
  exit(EXIT_FAILURE);
}
