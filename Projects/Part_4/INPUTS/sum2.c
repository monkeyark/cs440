
int main()
{
    float a, b;
    putstring("Enter two reals\n");
    a = getfloat();
    b = getfloat();
    putfloat(a);
    putstring(" + ");
    putfloat(b);
    putstring(" = ");
    putfloat(a+b);
    putchar(10);
    return 0;
}
