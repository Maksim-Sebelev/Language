int foo(int a, int b)
{
    int a = 3;
    int b = a + 3;

    if (foo(a, a + b))
    {
        return a + b;
    }

    for (int a = 0; a < 0; a = not a + 1)
    {
        a = foo(a, a + 1);
    }

    return a;
}

int govno()
{
    while (int a = 3)
    {
        if (a > 3)
        {
            return 15;
        }
        else
        {
            a = a + 3;
        }
    }
    return 1;
}


int main(int argc)
{
    return foo(argc, argc + 1) * 13 || foo(1 + 2 + foo(3 - 1, 2), 4 && foo(1, argc));
}