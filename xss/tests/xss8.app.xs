instance t1
{
    method foobar()
    {
        foo = t2.foo + "++";
        t2.bar = foo + bar + "++";
        complex = t2.bar + bar + this.foo + "--";
    }
}
