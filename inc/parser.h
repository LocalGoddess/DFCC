#ifndef DFCC_PARSER_H
#define DFCC_PARSER_H

#include <lexer.h>

struct return_statement {
    uint32_t type;
    void* value;
};

enum statement_kind {
    SK_RETURN
};

union statement_value
{
    // different statements here
    struct return_statement return_statement;
};

struct statement {
    union statement_value value;
    enum statement_kind kind;
};

struct function_argument {
    uint32_t type;
    char* name;

    struct function_argument* previous;
    struct function_argument* next;
};

struct function_definition_node {
    uint32_t return_type;
    char* label;

    struct function_argument* first_argument;
    struct function_argument* last_argument;

    struct statement* body;
    uint32_t statement_size;
};

struct parser {

    uint32_t index;
    uint32_t finding_node;
    struct lexer* lexer;

    struct function_definition_node* function_definitions;
    uint32_t function_definition_count;

    struct statement* current_statement;

    struct statement* current_compound_statement;
    uint32_t* current_compound_statement_size;
};

void parse_init( struct lexer* l );
void parse();
void parse_clean();


#endif// DFCC_PARSER_H