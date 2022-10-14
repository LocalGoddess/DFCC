#include <malloc.h>
#include <parser.h>

struct parser* parser;

void find_node();
void continue_node();

uint32_t get_token_count();
uint32_t get_token_literal_type( struct lexer_token* );
struct lexer_token* get_token_from_index( uint32_t index );


void parse_init( struct lexer* l )
{
    parser = malloc( sizeof( struct parser ) );

    parser->index = 0;
    parser->finding_node = 1;
    parser->lexer = l;

    parser->function_definitions = malloc( sizeof( struct function_definition_node ) );
    parser->function_definition_count = 0;

    parser->current_statement = NULL;

    parser->current_compound_statement = malloc( sizeof( struct statement ) );
    parser->current_compound_statement_size = 0;
}

void parse()
{
    // This might be good to cache in the lexer or have a
    // specific ending token
    uint32_t size = get_token_count();
    while ( size > parser->index ) {
        if ( parser->finding_node ) {
            find_node();
        }
        else {
            continue_node();
        }
        parser->index += 1;
    }


    // when the parser is done we want to free the memory from the lexer
    // in my opinion this isn't really needed as it will all be freed
    // by the OS however, it is just good practice to do so
    lex_clean();
}

void find_node()
{
    struct lexer_token* tkn = get_token_from_index( parser->index );
    switch ( tkn->type ) {
        case RETURN:
                // TODO(Chloe) This will have to be entirely reworked when
                // variables become a thing but it works for now because
                // this is just to parse example_one.c
                ;

            struct statement stmnt = { .kind = SK_RETURN };

            struct lexer_token* peek = get_token_from_index( parser->index + 1 );
            struct return_statement rtn = { .type = get_token_literal_type(peek), (void*) peek->content };

            union statement_value v = {.return_statement = rtn};
            stmnt.value = v;

            parser->current_statement = &stmnt;
            parser->index += 1;
            break;
        case INT:
            // TODO(Chloe) This will have to be entirely reworked when
            // variables become a thing but it works for now because
            // this is just to parse example_one.c



            break;
    }
}

/* Utility */

uint32_t get_token_count()
{
    uint32_t count = 0;
    struct lexer_token* token = parser->lexer->first_token;

    while ( ( token = token->next ) != NULL ) {
        count += 1;
    }
    return count;
}

uint32_t get_token_literal_type( struct lexer_token* token )
{
    switch ( token->type ) {
        case LIT_INT:
            return PARSER_TYPE_INT;
        case LIT_LONG:
            return PARSER_TYPE_LONG;
        case LIT_FLOAT:
            return PARSER_TYPE_FLOAT;
        case LIT_DOUBLE:
            return PARSER_TYPE_DOUBLE;
        case LIT_CHAR:
            return PARSER_TYPE_CHAR;
        case LIT_CHAR_PTR:
            return PARSER_TYPE_POINTER | PARSER_TYPE_CHAR;

        default:
            return PARSER_TYPE_NULL;
    }
}

struct lexer_token* get_token_from_index( uint32_t index )
{
    uint32_t cpy = index;
    struct lexer_token* token = parser->lexer->first_token;
    while ( cpy > 0 ) {
        token = token->next;
        cpy -= 1;
    }

    return token;
}