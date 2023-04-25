int foo(char a);

int foo(char x)
{
    int a, b=x;
    int const c=0;
    return c++;
}

int foo(char b);
