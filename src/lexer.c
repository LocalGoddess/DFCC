#include <lexer.h>
#include <malloc.h>
#include <string.h>

#define DFCC_DEBUG
#ifdef DFCC_DEBUG
#include <stdio.h>
#endif

char valid_number_chars[13] = "0123456789-.\0";
char valid_identifier_chars[54] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_\0";

uint32_t has_decimal = 0;

struct lexer* lexer;

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
    lexer->current_content = ( char* ) calloc( 1, sizeof( char ) );
    lexer->current_content_size = 0;

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
            create_single_char_token( SEMICOLON );
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

            if ( !is_valid_number( peek() ) ) {
                finish_token();
            }

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
            append_context_char( lexer->current );
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
    lexer->current_token.content = lexer->current_content;
    lexer->current_token.previous = lexer->last_token;

    lexer->last_token->next = &lexer->current_token;

    if ( lexer->first_token == NULL ) {
        lexer->first_token = &lexer->current_token;
    }

    lexer->last_token = &lexer->current_token;

#ifdef DFCC_DEBUG
    printf( "[DEBUG] Finished Token! Type: %d, Content: %s, Line: %u, Column: %u\n", lexer->current_token.type,
            lexer->current_token.content, lexer->current_token.line, lexer->current_token.column );
#endif

    lexer->finding_token = 1;
    lexer->current_token = create_empty();
    has_decimal = 0;

    lexer->current_content = ( char* ) calloc( 1, sizeof( char ) );
    lexer->current_content_size = 0;
}

void create_single_char_token( enum token_type type )
{
    lexer->finding_token = 0;
    lexer->current_token.line = lexer->line;
    lexer->current_token.column = lexer->column;
    lexer->current_token.type = type;
    append_context_char( lexer->current );
    finish_token();
}

void continue_number()
{
    append_context_char( lexer->current );
    if ( !is_valid_number( peek() ) ) {
        finish_token();
        return;
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
    if ( is_number_end( peek() ) ) {
        if ( peek() == 'l' || peek() == 'L' ) {
            lexer->current_token.type = LIT_LONG;
            finish_token();
        }
        if ( peek() == 'f' || peek() == 'F' ) {
            lexer->current_token.type = LIT_FLOAT;
            finish_token();
        }
    }
}

void continue_char()
{
    if ( peek() == '\'' ) {
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
    append_context_char( lexer->current );
    if ( token_from_keyword_str( lexer->current_content ) != NONE ) {
        lexer->current_token.type = token_from_keyword_str( lexer->current_content );
        finish_token();
        return;
    }

    if ( !is_valid_identifier( peek() ) ) {
        finish_token();
    }
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

uint32_t is_line_ending( char c )
{
    switch ( c ) {
        case '\0':
        case -1:
            return 1;
        default:
            return 0;
    }
}

void append_context_char( char c )
{
    lexer->current_content_size += 1;
    char* realloced = ( char* ) realloc( lexer->current_content, ( lexer->current_content_size + 1 ) * sizeof( char ) );
    if ( !realloced ) {
        fprintf( stderr, "REALLOCED IS NULL" );
    }
    lexer->current_content = realloced;
    lexer->current_content[lexer->current_content_size - 1] = c;
    lexer->current_content[lexer->current_content_size] = '\0';
}

struct lexer_token create_empty()
{
    struct lexer_token empty = {};
    return empty;
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

enum token_type token_from_keyword_str( const char* keyword )
{
    // !DO NOT EDIT!
    // This function was generated using code
    // if you would like to change it please change its contents in scripts/lexer_code_gen.py
    // !DO NOT EDIT!

    if ( strcmp( keyword, "auto" ) == 0 ) {
        return AUTO;
    }
    if ( strcmp( keyword, "break" ) == 0 ) {
        return BREAK;
    }
    if ( strcmp( keyword, "case" ) == 0 ) {
        return CASE;
    }
    if ( strcmp( keyword, "char" ) == 0 ) {
        return CHAR;
    }
    if ( strcmp( keyword, "const" ) == 0 ) {
        return CONST;
    }
    if ( strcmp( keyword, "continue" ) == 0 ) {
        return CONTINUE;
    }
    if ( strcmp( keyword, "default" ) == 0 ) {
        return DEFAULT;
    }
    if ( strcmp( keyword, "do" ) == 0 ) {
        return DO;
    }
    if ( strcmp( keyword, "double" ) == 0 ) {
        return DOUBLE;
    }
    if ( strcmp( keyword, "else" ) == 0 ) {
        return ELSE;
    }
    if ( strcmp( keyword, "enum" ) == 0 ) {
        return ENUM;
    }
    if ( strcmp( keyword, "extern" ) == 0 ) {
        return EXTERN;
    }
    if ( strcmp( keyword, "float" ) == 0 ) {
        return FLOAT;
    }
    if ( strcmp( keyword, "for" ) == 0 ) {
        return FOR;
    }
    if ( strcmp( keyword, "goto" ) == 0 ) {
        return GOTO;
    }
    if ( strcmp( keyword, "if" ) == 0 ) {
        return IF;
    }
    if ( strcmp( keyword, "inline" ) == 0 ) {
        return INLINE;
    }
    if ( strcmp( keyword, "int" ) == 0 ) {
        return INT;
    }
    if ( strcmp( keyword, "long" ) == 0 ) {
        return LONG;
    }
    if ( strcmp( keyword, "register" ) == 0 ) {
        return REGISTER;
    }
    if ( strcmp( keyword, "restrict" ) == 0 ) {
        return RESTRICT;
    }
    if ( strcmp( keyword, "return" ) == 0 ) {
        return RETURN;
    }
    if ( strcmp( keyword, "short" ) == 0 ) {
        return SHORT;
    }
    if ( strcmp( keyword, "signed" ) == 0 ) {
        return SIGNED;
    }
    if ( strcmp( keyword, "sizeof" ) == 0 ) {
        return SIZEOF;
    }
    if ( strcmp( keyword, "static" ) == 0 ) {
        return STATIC;
    }
    if ( strcmp( keyword, "struct" ) == 0 ) {
        return STRUCT;
    }
    if ( strcmp( keyword, "switch" ) == 0 ) {
        return SWITCH;
    }
    if ( strcmp( keyword, "typedef" ) == 0 ) {
        return TYPEDEF;
    }
    if ( strcmp( keyword, "union" ) == 0 ) {
        return UNION;
    }
    if ( strcmp( keyword, "unsigned" ) == 0 ) {
        return UNSIGNED;
    }
    if ( strcmp( keyword, "void" ) == 0 ) {
        return VOID;
    }
    if ( strcmp( keyword, "volatile" ) == 0 ) {
        return VOLATILE;
    }
    if ( strcmp( keyword, "while" ) == 0 ) {
        return WHILE;
    }
    return NONE;
}
