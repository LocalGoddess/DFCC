#include <stdio.h>
#include <lexer.h>

int main( int argc, const char** argv )
{
    printf( "Hello, World!\n" );
    lex_init("int main(){return 1;}\0");
    lex();


    return 0;
}
