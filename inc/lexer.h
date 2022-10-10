#ifndef DFCC_LEXER_H
#define DFCC_LEXER_H

#include <stdint.h>
/**
 * All the token types for C
 *
 */
enum token_type
{
    NONE = -1,

    /* Keywords */

    AUTO = 0,
    BREAK,
    CASE,
    CHAR,
    CONST,
    CONTINUE,
    DEFAULT,
    DO,
    DOUBLE,
    ELSE,
    ENUM,
    EXTERN,
    FLOAT,
    FOR,
    GOTO,
    IF,
    INLINE,
    INT,
    LONG,
    REGISTER,
    RESTRICT,
    RETURN,
    SHORT,
    SIGNED,
    SIZEOF,
    STATIC,
    STRUCT,
    SWITCH,
    TYPEDEF,
    UNION,
    UNSIGNED,
    VOID,
    VOLATILE,
    WHILE,

    // TODO(Chloe) make this more than just keywords

    /* Literals */

    LIT_CHAR,
    LIT_INT,
    LIT_LONG,
    LIT_FLOAT,
    LIT_DOUBLE,
    LIT_CHAR_PTR,

    IDENTIFIER,

    /* Symbols */
    SEMICOLON,
    END_OF_LINE = SEMICOLON,

    OPEN_PARENTH,   // (
    CLOSE_PARENTH,  // )

    OPEN_CBRACKET,  // {
    CLOSE_CBRACKET, // }

};

uint32_t token_is_literal_number(enum token_type type);
const char* token_keyword_str(enum token_type type);
enum token_type token_from_keyword_str(const char* keyword);

uint32_t str_is_keyword(char* c);

struct lexer_token {
    enum token_type type;
    const char* content;

    uint32_t line;
    uint32_t column;

    struct lexer_token* previous;
    struct lexer_token* next;
};

struct lexer {
    const char* source;

    char current;
    uint32_t index;

    uint32_t line;
    uint32_t column;

    uint32_t finding_token;// true or false

    struct lexer_token current_token;
    char* current_content;
    uint32_t current_content_size;

    // errors

    struct lexer_token* first_token;
    struct lexer_token* last_token;
};

void lex_init( const char* source );
void lex();
void lex_clean();

#endif// DFCC_LEXER_H
