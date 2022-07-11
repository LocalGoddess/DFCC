#ifndef DFCC_LEXER_H
#define DFCC_LEXER_H

enum lexer_token_type
{
    IDENTIFIER,// Things like variable names, function names, etc...
    KEYWORD,   // Things like int, char, void, struct, enum. More found here https://en.cppreference.com/w/c/keyword
    LITERAL,   // 10, 0xFF, 0o88, 'c', "Char*", 3.14 might make each literal its own thing /shrug
    OPERATOR,  // + - * / < > & = !
    GRAMMAR    // ( ) { } [ ] ; ,
};

struct lexer_token {
    enum lexer_token_type type;
    const char* context;

    unsigned long line;
    unsigned long column;

    struct lexer_token* next;
    struct lexer_token* previous;
};

struct lexer {
    const char* code;
    int debug; /* bool - if true will print debug messages */

    struct lexer_token* first;
    struct lexer_token* last;

    //// Lexer Working Data ////
    int current_pos; /* The current read index */

    char current_char;  /* The current char currently being analyzed */
    char previous_char; /* The previous char worked on. Just makes it easier to check for escapes */

    unsigned long current_line; /* The current line and column being worked on */
    unsigned long current_column;

    unsigned long token_count; /* The current amount of tokens */

    int findingToken;                         /* bool - if true we have a target for a token */
    enum lexer_token_type current_token_type; /* see above */
    char* current_context_data;               /* the current context data */
    unsigned long context_len;
    ///////////////////////////
};

struct lexer* lexer; /* Main lexer "instance" for DFCC */

/**
 * Initialize the lexer
 * @param code - the code to be lexed
 * @param debug (bool) - should the lexer print debugging messages
 */
void start_lexer( const char* code, int debug );
void lex(); /* actually starts the lexical process */
char look_ahead();

void clean(); /* refresh token data */
void search_for_token();
void continue_token();
void handle_new_line();

int should_stop(); /* should the lexer stop */
void append_char( char c );
int is_identifier_compatible( char c );
int is_numeral_compatible( char c );
int is_numeral_ending( char c );

void create_token();
void append_single_char_token( char c );

#endif// DFCC_LEXER_H
