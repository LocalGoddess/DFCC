# DFCC

This file will contain the basic info for the DiamondFire C Compiler

## Road Blocks

### Main Function

In the C Programming langauge the main function is defined as:

```c
int main(void)
{
    
}
```

However, DiamondFire has no such entry point for plots. This brings us to our first issue... How do we implement a main
function? A simple workaround is to run the main function as soon as the plot starts which is satisfactory.

### Pointers

The C Programming Language has a feature known as pointers. Pointers point to memory address which contain values.
DiamondFire has no such thing thus Roadblock #2. A solution I came up which should work for now is to make pointers
represent DiamondFire lists or %var() depending on the way it was initialized. For example: char* my_pointer = &my_var 
will be compiled to DiamondFire as my_pointer = %var(my_var), and dereferencing my_pointer will give the value of my_var

