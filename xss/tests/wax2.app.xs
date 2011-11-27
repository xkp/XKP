method util1()
{
    var c = testwax.foo_bar();
    c += testwax.foo_bar();

    if (c < 20)
    {
        return c;
    }
    else
    {
        c += 20;
        return testwax.bar_foo(c);
    }
}

method service1()
{
    if (testwax.bar(10, 12))
    {
        return util1();
    }
    else
    {
        testwax.bar_foo(20);
    }

    var x = 10;
    testwax.bar();
    
    return x;
}
