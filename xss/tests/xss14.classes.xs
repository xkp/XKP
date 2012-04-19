class test_class
{
}

class test_class1 : test_class
{
    property some_prop = 
    {
        return 10;
    }
    {
        value++;
    }
}

class test_class2 : test_class1
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
}
