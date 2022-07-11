#ifndef DFCC_PREPARSER_H
#define DFCC_PREPARSER_H
#include <lexer.h>

/*
 * The preparser will turn the lexer tokens into more effective tokens for the parser
 * For example, the function `int main() { return 0; }` will have the tokens
 * KEYWORD, IDENTIFIER, GRAMMAR, GRAMMAR, GRAMMAR, KEYWORD, LITERAL, GRAMMAR, GRAMMAR
 * This is not so helpful for parsing the tokens what the preparser will do is turn the tokens into
 * TYPE, IDENTIFIER, GRAMMAR, GRAMMAR, OPEN_STATEMENT, KEYWORD, INT, STATEMENT_END CLOSE_STATEMENT
 * which is easier to parse
 */

enum preparser_token_type {
    PRE_CLOSE_STATEMENT,
    PRE_GRAMMAR,
    PRE_IDENTIFIER,
    PRE_KEYWORD,
    PRE_OPEN_STATEMENT,
    PRE_OPERATOR,
    PRE_STATEMENT_END,
    PRE_TYPE,

    /* Literal types */
    PRE_INT,
    PRE_LONG,
    PRE_FLOAT
};

struct preparser_token {
    enum preparser_token_type type;
    const char* context;

    struct lexer_token* lexer_token;

    struct preparser_token* next;
    struct preparser_token* previous;
};


struct preparser {
    struct lexer* lexer;

    struct preparser_token* first;
    struct preparser_token* last;

    struct lexer_token* current_token;
    enum preparser_token_type current_type;
};

struct preparser* preparser;


void start_preparser();
void preparse();

int should_stop_preparse();
enum preparser_token_type lexer_token_to_preparser_token(enum lexer_token_type);

void finish_preparser_token();


#endif// DFCC_PREPARSER_H
