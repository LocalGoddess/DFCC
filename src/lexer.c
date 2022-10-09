#include <lexer.h>
#include <malloc.h>
#include <string.h>

#ifdef DFCC_DEBUG
#include <stdio.h>
#endif

char valid_number_chars[13] = "0123456789-.\0";
char valid_identifier_chars[54] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_\0";

uint32_t has_decimal = 0;

void find_token();
void continue_token();
void finish_token();
void create_single_char_token( enum token_type type );

void continue_number();
void continue_char();
void continue_char_ptr();
void continue_identifier();

char peek();
uint32_t is_valid_number( char c );
uint32_t is_number_end( char c );
uint32_t is_valid_identifier( char c );
uint32_t is_line_ending( char c );
void append_context_char( char c );
struct lexer_token create_empty();

void lex_init( const char* source )
{
    lexer = malloc( sizeof( struct lexer ) );
    lexer->source = source;

    lexer->index = 0;
    lexer->current = '\0';

    lexer->line = 0;
    lexer->column = 0;

    lexer->finding_token = 1;

    lexer->current_token = create_empty();
    lexer->current_content = "\0";

    lexer->first_token = NULL;
    lexer->last_token = NULL;
}

void lex()
{
    size_t len = strlen( lexer->source );
    while ( lexer->index < len ) {
        lexer->current = lexer->source[lexer->index];
        if ( lexer->current == '\n' ) {
            lexer->column = 0;
            lexer->line += 1;
        }


        if ( is_line_ending( lexer->current ) ) {
            break;
        }

        if ( lexer->finding_token ) {
            find_token();
        }
        else {
            continue_token();
        }

        lexer->index += 1;
        lexer->column += 1;
    }
}


void find_token()
{
    switch ( lexer->current ) {
        case '(':
            create_single_char_token( OPEN_PARENTH );
            break;
        case ')':
            create_single_char_token( CLOSE_PARENTH );
            break;
        case '{':
            create_single_char_token( OPEN_CBRACKET );
            break;
        case '}':
            create_single_char_token( CLOSE_CBRACKET );
            break;
        case ';':
            create_single_char_token(SEMICOLON);
            break;

        case ' ':
            lexer->column += 1;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            lexer->finding_token = 0;
            lexer->current_token.line = lexer->line;
            lexer->current_token.column = lexer->column;
            lexer->current_token.type = LIT_INT;
            append_context_char( lexer->current );
            break;
        }

        case '\'':
        {
            lexer->finding_token = 0;
            lexer->current_token.line = lexer->line;
            lexer->current_token.column = lexer->column;
            lexer->current_token.type = LIT_CHAR;
            // Don't append we don't want to have the ' inside our character
            break;
        }

        case '"':
        {
            lexer->finding_token = 0;
            lexer->current_token.line = lexer->line;
            lexer->current_token.column = lexer->column;
            lexer->current_token.type = LIT_CHAR_PTR;
            // Same as above don't want to have " in the string
            break;
        }

        default:
        {
            lexer->finding_token = 0;
            lexer->current_token.line = lexer->line;
            lexer->current_token.column = lexer->column;
            lexer->current_token.type = IDENTIFIER;
            break;
        }
    }
}


/* Continue Functions */

void continue_token()
{
    switch ( lexer->current_token.type ) {
        case LIT_INT:
        case LIT_LONG:
        case LIT_FLOAT:
        case LIT_DOUBLE:
            continue_number();
            break;

        case LIT_CHAR:
            continue_char();
            break;

        case LIT_CHAR_PTR:
            continue_char_ptr();
            break;

        case IDENTIFIER:
            continue_identifier();
            break;
    }
}

void finish_token()
{
#ifdef DFCC_DEBUG
    printf( "[DEBUG] Finished Token! Type: %d, Content: %s, Line: %u, Column: %u", lexer->current_token.type,
            lexer->current_token.content, lexer->current_token.line, lexer->current_token.column );
#endif

    append_context_char( '\0' );
    lexer->current_token.content = lexer->current_content;
    lexer->current_token.previous = lexer->last_token;

    if ( lexer->first_token == NULL ) {
        lexer->first_token = &lexer->current_token;
    }

    lexer->last_token = &lexer->current_token;

    lexer->finding_token = 1;
    lexer->current_token = create_empty();
    has_decimal = 0;
}

void create_single_char_token( enum token_type type )
{
    lexer->finding_token = 0;
    lexer->current_token.line = lexer->line;
    lexer->current_token.column = lexer->column;
    lexer->current_token.type = type;
    append_context_char( lexer->current );
}

void continue_number()
{
    if ( is_valid_number( lexer->current ) ) {
        append_context_char( lexer->current );
    }

    if ( lexer->current == '.' ) {
        if ( !has_decimal ) {
            lexer->current_token.type = LIT_FLOAT;
            has_decimal = 1;
        }
        else {
            // TODO(Chloe) Error
        }
    }

    if ( !is_number_end( peek() ) ) {
        if ( peek() == 'l' || peek() == 'L' ) {
            lexer->current_token.type = LIT_LONG;
        }
        if ( peek() == 'f' || peek() == 'F' ) {
            lexer->current_token.type = LIT_FLOAT;
        }
    }

    if ( !is_valid_number( peek() ) ) {
        finish_token();
    }
}

void continue_char()
{
    if ( peek() == '\'' ) {
        char c = '\0';
        const char* temp = lexer->current_content;
        const char* checkData = strncat( temp, &c, 1 );
        size_t len = strlen( checkData );

        if ( ( len - 1 ) > 1 ) {
            if ( checkData[0] != '\\' ) {
                // TODO(Chloe): Error
            }
        }

        finish_token();
    }
    append_context_char( lexer->current );
}

void continue_char_ptr()
{
    if ( peek() == '"' && lexer->current != '\\' ) {
        finish_token();
    }

    append_context_char( lexer->current );
}

void continue_identifier()
{
    if ( token_is_keyword( lexer->current_content ) ) {
        lexer->current_token.type = token_from_keyword_str( lexer->current_content );
        finish_token();
        return;
    }

    if ( is_valid_identifier( peek() ) ) {
        finish_token();
    }
    append_context_char( lexer->current );
}

/* Utility Functions */

char peek() { return lexer->source[lexer->index + 1]; }

uint32_t is_valid_number( char c )
{
    for ( int i = 0; i < strlen( valid_number_chars ); i++ ) {
        if ( c == '\0' ) break;
        if ( c == valid_number_chars[i] ) return 1;
    }
    return 0;
}

uint32_t is_number_end( char c )
{
    if ( c == 'l' || c == 'L' || c == 'f' || c == 'F' ) {
        return 1;
    }
    return 0;
}

uint32_t is_valid_identifier( char c )
{
    for ( int i = 0; i < strlen( valid_identifier_chars ); i++ ) {
        if ( c == '\0' ) break;
        if ( c == valid_identifier_chars[i] ) return 1;
    }
    return 0;
}

void append_context_char( char c ) { strncat( lexer->current_content, &c, 1 ); }

struct lexer_token create_empty()
{
    struct lexer_token empty = {};
    return empty;
}

uint32_t token_is_literal_number( enum token_type type )
{
    switch ( type ) {
        case LIT_INT:
        case LIT_LONG:
        case LIT_FLOAT:
        case LIT_DOUBLE:
            return 1;
        default:
            return 0;
    }
}

const char* token_keyword_str( enum token_type type )
{
    switch ( type ) {
        case AUTO:
            return "auto";
        case BREAK:
            return "break";
        case CASE:
            return "case";
        case CHAR:
            return "char";
        case CONST:
            return "const";
        case CONTINUE:
            return "continue";
        case DEFAULT:
            return "default";
        case DO:
            return "do";
        case DOUBLE:
            return "double";
        case ELSE:
            return "else";
        case ENUM:
            return "enum";
        case EXTERN:
            return "extern";
        case FLOAT:
            return "float";
        case FOR:
            return "for";
        case GOTO:
            return "goto";
        case IF:
            return "if";
        case INLINE:
            return "inline";
        case INT:
            return "int";
        case LONG:
            return "long";
        case REGISTER:
            return "register";
        case RESTRICT:
            return "restrict";
        case RETURN:
            return "return";
        case SHORT:
            return "short";
        case SIGNED:
            return "signed";
        case SIZEOF:
            return "sizeof";
        case STATIC:
            return "static";
        case STRUCT:
            return "struct";
        case SWITCH:
            return "switch";
        case TYPEDEF:
            return "typedef";
        case UNION:
            return "union";
        case UNSIGNED:
            return "unsigned";
        case VOID:
            return "void";
        case VOLATILE:
            return "volatile";
        case WHILE:
            return "while";

        default:
            return NULL;
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-compare"
enum token_type token_from_keyword_str( const char* keyword )
{
    if ( keyword == "auto" ) {
        return AUTO;
    }
    else if ( keyword == "break" ) {
        return BREAK;
    }
    else if ( keyword == "case" ) {
        return CASE;
    }
    else if ( keyword == "char" ) {
        return CHAR;
    }
    else if ( keyword == "const" ) {
        return CONST;
    }
    else if ( keyword == "continue" ) {
        return CONTINUE;
    }
    else if ( keyword == "default" ) {
        return DEFAULT;
    }
    else if ( keyword == "do" ) {
        return DO;
    }
    else if ( keyword == "double" ) {
        return DOUBLE;
    }
    else if ( keyword == "else" ) {
        return ELSE;
    }
    else if ( keyword == "enum" ) {
        return ENUM;
    }
    else if ( keyword == "extern" ) {
        return EXTERN;
    }
    else if ( keyword == "float" ) {
        return FLOAT;
    }
    else if ( keyword == "for" ) {
        return FOR;
    }
    else if ( keyword == "goto" ) {
        return GOTO;
    }
    else if ( keyword == "if" ) {
        return IF;
    }
    else if ( keyword == "inline" ) {
        return INLINE;
    }
    else if ( keyword == "int" ) {
        return INT;
    }
    else if ( keyword == "long" ) {
        return LONG;
    }
    else if ( keyword == "register" ) {
        return REGISTER;
    }
    else if ( keyword == "restrict" ) {
        return RESTRICT;
    }
    else if ( keyword == "return" ) {
        return RETURN;
    }
    else if ( keyword == "short" ) {
        return SHORT;
    }
    else if ( keyword == "signed" ) {
        return SIGNED;
    }
    else if ( keyword == "sizeof" ) {
        return SIZEOF;
    }
    else if ( keyword == "static" ) {
        return STATIC;
    }
    else if ( keyword == "struct" ) {
        return STRUCT;
    }
    else if ( keyword == "switch" ) {
        return SWITCH;
    }
    else if ( keyword == "typedef" ) {
        return TYPEDEF;
    }
    else if ( keyword == "union" ) {
        return UNION;
    }
    else if ( keyword == "unsigned" ) {
        return UNSIGNED;
    }
    else if ( keyword == "void" ) {
        return VOID;
    }
    else if ( keyword == "volatile" ) {
        return VOLATILE;
    }
    else if ( keyword == "while" ) {
        return WHILE;
    }
    return NONE;
}
#pragma clang diagnostic pop
