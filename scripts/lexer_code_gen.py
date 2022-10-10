def gen_token_from_keyword_str(tokens):
    code = "enum token_type token_from_keyword_str( const char* keyword )\n{\n"
    code += "\t// !DO NOT EDIT!\n"
    code += "\t// This function was generated using code!\n"
    code += "\t// If you would like to change it please change its contents in scripts/lexer_code_gen.py\n"
    code += "\t// !DO NOT EDIT!\n\n"
    for i in range(len(tokens)):
        tkn = tokens[i]
        code += "\tif ( strncmp( keyword, \"" + str(tkn).lower() + "\" ) == 0 ) {\n"
        code += "\t\treturn " + tkn.upper() + ";\n"
        code += "\t}\n"
    code += "\treturn NONE;\n}"
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
