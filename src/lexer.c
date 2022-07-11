#include <lexer.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

const char* disallowed_identifier_chars = "!@#$%^&*()-=+[{]}\\|;:\'\",<.>/?";
const char* allowed_number_chars = "01234566789.";

const char** supported_keywords = { "int", "return" };

void continue_token_identifier();
void continue_token_number();

void start_lexer( const char* code, int debug )
{
    lexer = malloc( sizeof( struct lexer ) );
    lexer->code = code;
    lexer->debug = debug;

    lexer->current_pos = 0;
    lexer->previous_char = '\0';
    lexer->current_line = 0;
    lexer->current_column = 0;
    lexer->token_count = 0;
    lexer->findingToken = 0;
    lexer->context_len = 0;
    lexer->current_context_data = malloc( 1 * sizeof( char ) );

    lexer->first = NULL;
    lexer->last = NULL;
}

void lex()
{
    unsigned long long code_length = strlen( lexer->code );
    while ( lexer->current_pos < code_length ) {
        if ( should_stop() ) {
            break;
        }
        if ( lexer->findingToken ) {
            continue_token();
        }
        else {
            search_for_token();
        }

        // Very end
        lexer->current_column += 1;
        lexer->previous_char = lexer->current_char;
        lexer->current_char = lexer->code[lexer->current_pos];
    }

    if ( lexer->debug ) {
        printf( "Finished lexical analysis" );
    }
}

char look_ahead() { return lexer->code[lexer->current_pos + 1]; }

void clean()
{
    lexer->findingToken = 0;
    lexer->current_context_data = malloc( 1 * sizeof( char ) );
    lexer->context_len = 0;
}

void search_for_token()
{
    switch ( lexer->current_char ) {
        case '\n':
            handle_new_line();
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        {
            // TODO(Chloe): Remember negatives exist
            lexer->findingToken = 1;
            lexer->current_token_type = LITERAL;
            append_char( lexer->current_char );
            break;
        }
        case '(':
        case ')':
        case '{':
        case '}':
        case ';':
        {
            append_single_char_token( lexer->current_char );
            break;
        }


        default: /* for identifiers and keywords */
            if ( is_identifier_compatible( lexer->current_char ) ) {
                lexer->findingToken = 1;
                lexer->current_token_type = IDENTIFIER;
                append_char( lexer->current_char );
            }
            break;
    }
}

void continue_token()
{
    switch ( lexer->current_token_type ) {
        case IDENTIFIER:
            continue_token_identifier();
            break;
        case LITERAL:
            // some checks to estimate the literal
            if ( lexer->current_context_data[0] != '"' || lexer->current_context_data[0] != '\'' ) {
                continue_token_number();
            }
            break;
    }
}

void continue_token_identifier()
{
    if ( !is_identifier_compatible( lexer->current_char ) || !is_identifier_compatible( look_ahead() ) ) {
        for ( int i = 0; i < 2 /* temp value since we only have two keywords  */; i++ ) {
            if ( strcmp( lexer->current_context_data, supported_keywords[i] ) == 0 ) {
                lexer->current_token_type = KEYWORD;
            }
            else {
                lexer->current_token_type = IDENTIFIER;
            }
        }
        create_token();
    }
    else {
        append_char( lexer->current_char );
    }
}

void continue_token_number()
{
    if ( !is_numeral_compatible( lexer->current_char ) || !is_numeral_compatible( look_ahead() ) ||
         is_numeral_ending( lexer->current_char ) )
    {
        // this will allow multiple decimals but that's fine for now
        lexer->current_token_type = LITERAL;
        create_token();
    }
    else {
        append_char( lexer->current_char );
    }
}

void handle_new_line()
{
    lexer->current_line += 1;
    lexer->current_column = 0;
}

int should_stop()
{
    switch ( lexer->current_char ) {
        case '\0':  // Null char
        case -1:    // End of line
        case '\032':// ^Z End of File
            return 1;
        default:
            return 0;
    }
}

void append_char( char c )
{
    lexer->current_context_data = realloc( lexer->current_context_data, ( lexer->context_len + 2 ) * sizeof( char ) );
    lexer->current_context_data[lexer->context_len + 1] = c;
    lexer->context_len += 1;
}

int is_identifier_compatible( char c )
{
    for ( int i = 0; i < strlen( disallowed_identifier_chars ); i++ ) {
        if ( c == disallowed_identifier_chars[i] ) return 0;
    }
    return 1;
}

int is_numeral_compatible( char c )
{
    for ( int i = 0; i < strlen( allowed_number_chars ); i++ ) {
        if ( c == allowed_number_chars[i] ) return 1;
    }
    return 0;
}

int is_numeral_ending( char c )
{
    // Prob a better way to do this
    if ( c == 'L' || c == 'l' || c == 'f' || c == 'F' ) {
        return 1;
    }
    return 0;
}

void create_token()
{
    if ( lexer->debug ) {
        printf( "Creating new token: %s", lexer->current_token_type );
    }

    struct lexer_token token = { .type = lexer->current_token_type,
                                 .context = lexer->current_context_data,
                                 .line = lexer->current_line,
                                 .column = lexer->current_column,
                                 .previous = lexer->last,
                                 .next = NULL };
    if ( lexer->first == NULL ) {
        lexer->first = &token;
        lexer->last = &token;
    }
    else {
        lexer->last->next = &token;
        lexer->last = &token;
    }
    lexer->token_count += 1;
    clean();
}

void append_single_char_token( char c )
{
    lexer->current_token_type = GRAMMAR;// this prob won't change
    append_char( c );

    create_token();
}