int main()
{
    int a;
    for (a=48 ; a<=55; a = a+1)
    {
        putchar(a);
    }
    putchar(10);
    break;  // should be an error
    return 0;
}

