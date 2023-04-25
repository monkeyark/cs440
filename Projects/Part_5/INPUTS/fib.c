/*
  Computes and prints the first 40 Fibonacci numbers.
*/

int main()
{
    int i, f1, f2, f3;
    f1 = 0;
    f2 = 1;
    putint(0);
    putchar(32);
    putchar(58);
    putchar(32);
    putint(0);
    putchar(10);
    i=40;
    while (i) {
        putint(40 - --i);
        putchar(32);
        putchar(58);
        putchar(32);
        putint(f2);
        putchar(10);
        f3 = f1 + f2;
        f1 = f2;
        f2 = f3;
    }
    return 0;
}
