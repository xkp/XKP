instance t1
{
	
	property typing;
	
    property simple = 50;

    property read_only = 
    {
        return 10.0;
    }

    property writable
    {
        System.out.println(value);
    }

    property mixed = 
    {
        return 10;
    }
    {
        System.out.println("Something");
    }
	
    method check_type()
    {
        var a;
        a = 1;
		
		var b;
		
		typing = a;
		
        return a;
    }
	
    method foobar(int v)
    {
        foo = t2.foo + 10;
        t2.bar = foo + bar + "++";
        complex = t2.bar + bar + this.foo + "--";

        some_int--;
        t2.some_int++;

        some_int += 20;
		
		more_complex++;
		t2.more_complex--;

        array arr = [];
        arr += 10;

        writable = read_only + simple + mixed;
        mixed = writable;
		
		return bar;
    }
	
	on click()
	{
		var list = [1, 2, "item", 3.14, "vowel"];
		for(var i in list)
		{
			System.out.println(i);
		}
	}
	
}
