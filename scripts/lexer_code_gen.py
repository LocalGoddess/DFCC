def gen_token_from_keyword_str(tokens):
    present_if = False
    code = "enum token_type token_from_keyword_str( const char* keyword )\n{\n"
    for i in range(len(tokens)):
        tkn = tokens[i]
        if present_if:
            code += "else "
            present_if = True
        code += "if ( strcmp( keyword, \"" + str(tkn).lower() + "\" ) ) {\n\t return " + str(tkn).upper() + ";\n\t}"
    return code


def gen_str_is_keyword(tokens):
    present_if = False
    code = "uint32_t str_is_keyword( char* c );\n{\n"
    for i in range(len(tokens)):
        tkn = tokens[i]
        if present_if == False:
            code += "else "
        else:
            present_if = True
        code += "if ( strcmp( c, \"" + str(tkn).lower() + "\" ) ) {\n\t return 1;\n\t}"
    code += "\nreturn 0;\n}"
    return code


tkns = ['AUTO',
        'BREAK',
        'CASE',
        'CHAR',
        'CONST',
        'CONTINUE',
        'DEFAULT',
        'DO',
        'DOUBLE',
        'ELSE',
        'ENUM',
        'EXTERN',
        'FLOAT',
        'FOR',
        'GOTO',
        'IF',
        'INLINE',
        'INT',
        'LONG',
        'REGISTER',
        'RESTRICT',
        'RETURN',
        'SHORT',
        'SIGNED',
        'SIZEOF',
        'STATIC',
        'STRUCT',
        'SWITCH',
        'TYPEDEF',
        'UNION',
        'UNSIGNED',
        'VOID',
        'VOLATILE',
        'WHILE'
        ]

if __name__ == '__main__':
    print(gen_token_from_keyword_str(tkns))
