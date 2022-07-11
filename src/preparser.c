#include <malloc.h>
#include <preparser.h>
#include <string.h>

const char** type_keywords = { "int" };

void clean_preparser();

void preparse_keyword();
void preparse_grammar();
void preparse_literal();

void start_preparser()
{
    preparser = malloc( sizeof( struct preparser ) );
    preparser->lexer = lexer;
    preparser->first = NULL;
    preparser->last = NULL;
    preparser->current_token = NULL;
    preparser->current_type = 0;
}

void preparse()
{
    preparser->current_token = preparser->lexer->first;
    while ( !should_stop_preparse() ) {
        switch ( preparser->current_token->type ) {
            case KEYWORD:
                preparse_keyword();
                break;
            case GRAMMAR:
                preparse_grammar();
            case LITERAL:
                preparse_literal();

            default:
                finish_preparser_token();
        }

        preparser->current_token = preparser->current_token->next;
    }
}

void preparse_keyword()
{
    int i;
    for ( i = 0; i < 1; i++ ) {
        if ( strcmp( preparser->current_token->context, type_keywords[i] ) == 0 ) {
            preparser->current_type = PRE_TYPE;
        }
        else {
            preparser->current_type = PRE_KEYWORD;
        }
    }
    finish_preparser_token();
}

void preparse_grammar()
{
    char grammar = preparser->current_token->context[0];// Just assume its always one char for now unless there is a two
    switch ( grammar ) {
        default:
            finish_preparser_token();
            break;
        case ';':
            preparser->current_type = PRE_STATEMENT_END;
            finish_preparser_token();
            break;
        case '{':
            preparser->current_type = PRE_OPEN_STATEMENT;
            finish_preparser_token();
            break;
        case '}':
            preparser->current_type = PRE_CLOSE_STATEMENT;
            finish_preparser_token();
            break;
    }
}

void preparse_literal()
{
    if (preparser->current_token->context[0] != '\'' || preparser->current_token->context[0] != '"') {
        // number
        if ( strchr(preparser->current_token->context, '.') ) {
            // float
        } else {
            // long or int
            if ( strchr(preparser->current_token->context, 'L') || strchr(preparser->current_token->context, 'l')) {
                // long
            } else {
                preparser->current_type = PRE_INT;
            }
        }
    } else {
        // string or char
    }
    finish_preparser_token();
}


void clean_preparser() { preparser->current_type = 0; }

int should_stop_preparse()
{
    if ( preparser->current_token == NULL ) {
        return 1;
    }
    return 0;
}

enum preparser_token_type lexer_token_to_preparser_token(enum lexer_token_type lexer_type)
{
    switch ( lexer_type ) {
        case IDENTIFIER:
            return PRE_IDENTIFIER;
        case GRAMMAR:
            return PRE_GRAMMAR;
        case KEYWORD:
            return PRE_KEYWORD;
        case OPERATOR:
            return PRE_OPERATOR;
        case LITERAL:
            return PRE_INT;
    }
}

void finish_preparser_token()
{
    if ( preparser->current_type == 0 ) {
        preparser->current_type = lexer_token_to_preparser_token( preparser->current_token->type );
    }

    struct preparser_token token = { .type = preparser->current_type,
            .context = preparser->current_token->context,
            .lexer_token = preparser->current_token,
            .next = NULL,
            .previous = preparser->last };
    if (preparser->first == NULL) {
        preparser->first = &token;
    }
    preparser->last->next = &token;
    preparser->last = &token;
    clean_preparser();
}
