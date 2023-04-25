
/*
 * Simple short-circuiting tests
 */

void and(int x)
{
    if ((putchar(65) < x) && (putchar(75) < x) && (putchar(85) < x))
    {
        putchar(33);
    } else {
        putchar(46);
    }
    putchar(10);
}

void or(int x)
{
    if ((putchar(65) > x) || (putchar(75) > x) || (putchar(85) > x))
    {
        putchar(33);
    } else {
        putchar(46);
    }
    putchar(10);
}

int main()
{
    and(60);
    and(70);
    and(80);
    and(90);

    or(60);
    or(70);
    or(80);
    or(90);
    return 0;
}
