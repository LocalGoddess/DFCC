#include <stdio.h>
#include <lexer.h>

int main( int argc, const char** argv )
{
    printf( "Hello, World!" );
    lex_init("int\0");
    lex();


    return 0;
}
