#include <malloc.h>
#include <parser.h>

struct parser* parser;

void find_node();
void continue_node();

uint32_t get_token_count();
struct lexer_token* get_token_from_index( uint32_t index );


void parse_init( struct lexer* l )
{
    parser = malloc( sizeof( struct parser ) );

    parser->index = 0;
    parser->finding_node = 1;
    parser->lexer = l;

    parser->function_definitions = malloc(sizeof(struct function_definition_node));
    parser->function_definition_count = 0;

    parser->current_statement = NULL;

    parser->current_compound_statement = malloc(sizeof(struct statement));
    parser->current_compound_statement_size = 0;

}

void parse()
{
    // This might be good to cache in the lexer or have a
    // specific ending token
    uint32_t size = get_token_count();
    while ( size > parser->index ) {
        if (parser->finding_node) {
            find_node();
        } else {
            continue_node();
        }
        parser->index += 1;
    }


    // when the parser is done we want to free the memory from the lexer
    // in my opinion this isn't really needed as it will all be freed
    // by the OS however, it is just good practice to do so
    lex_clean();
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