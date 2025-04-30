int foo(int a, int b)
{
    int a = 3;
    int b = a + 3;

    if (foo(a, a + b))
    {
        return a + b;
    }

    for (int a = 0; a < 0; a = a + 1)
    {
        a = foo(a, a + 1);
    }

    return a;
}


int main(int argc)
{
    return foo(argc, argc + 1) * 13 || foo(1 + 2 + foo(3 - 1, 2), 4 && foo(1, argc));
}