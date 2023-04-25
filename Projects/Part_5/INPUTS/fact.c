
int main()
{
    int i, f;
    f = 1;
    for (i=1; i<13; i++) {
        putint(i);
        putchar(33);
        putchar(32);
        putchar(61);
        putchar(32);
        putint(f *= i);
        putchar(10);
    }
    return 0;
}
