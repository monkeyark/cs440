// NORUN

void foo(int x)
{
    putchar(x);
}

int main()
{
    if (foo(33)) {
        putchar(89);
    } else {
        putchar(78);
    }
    return putchar(10);
}
