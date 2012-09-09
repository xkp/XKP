class test_class
{
    method test1()
    {
        return c;
    }
    
    method test2()
    {
        var a = test1();
        if (a > 10)
        {
            int b = 30;
            return b;
        }

        return aa;
    }

    int aa = 20;

    property c
    {
        aa = value;
        foo.bar = aa;
    }
}

class test_class_xs_only
{
    property some_prop = 10;
}