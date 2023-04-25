
int modify(int A[])
{
    return A[0]++;
}

int main()
{
    const int fixed[50];

    modify(fixed);  // this should be an error

    return 0;
}
