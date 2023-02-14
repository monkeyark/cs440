
char z;

void func1(int y)
{
    if (y)
        if (z)
            for (;z;z++)
                while (y--);
        else
            while (y--)
                z++;
    else
        z--;

    int x;
}

int func2(int w, int v, int u)
{
    if (w<=v) {
        if (w<=u) {
            return w;
        }
        if (u<=w) {
            return u;
        }
    } else {
        if (v<=u) {
            return v;
        }
        if (u<=v) {
            return u;
        }
    }
}

float t;

float func3(float A[], int n)
{
    float sum;
    sum = 0.0;
    for (--n; n>=0; --n)
    {
        sum += A[n];
    }
    return sum;
}
