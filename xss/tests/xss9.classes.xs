class test_class : t_xss9_1
{
}

class test_class1 : t_xss9_2
{
    property some_prop = 
    {
        return 10;
    }
    {
        value++;
    }
}

class test_class2 : t_xss9_2
{
	property some_prop = 
	{
		return 10;
	}
	{
		value++;
	}
	
	method foo()
	{
		some_prop++;
	}
	
	instance base
	{
		property collect : string = "garbage";
		
		method bar()
		{
			collect += " collect ";
		}
		
		on create()
		{
			bar();
		}
	}
}
