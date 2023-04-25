
int a;

int func(int b, int c)
{
    return a+b*c;
}

int main()
{
    a = 3;
    putint(func(3+1, 20/4));
    return putchar(10);
}
