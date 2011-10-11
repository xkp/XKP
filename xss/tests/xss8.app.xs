instance t1
{
    property simple = 50;

    property read_only = 
    {
        return 10.0;
    }

    property writable
    {
        console.log(value);
    }

    property mixed = 
    {
        return 10;
    }
    {
        console.log("Something");
    }

    method check_type()
    {
        var a;
        a = 1;
        return a;
    }

    method foobar(int v)
    {
        foo = t2.foo + "++";
        t2.bar = foo + bar + "++";
        complex = t2.bar + bar + this.foo + "--";

        some_int--;
        t2.some_int++;

        some_int += 20;

        array arr = [];
        arr += 10;

        writable = read_only + simple + mixed;
        mixed = writable;

        more_complex++;
        t2.more_complex--;
    }
}
