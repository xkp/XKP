property lvalue;
property yvalue = "string v";
property xvalue = ["value1", "value2", "value3"];

on create()
{
	// how traduce this correctly to java?
	
	//ERROR: assert
	//var o = object(v1 = 2);
	//o.new_property = "value";
	//o.other_new_property = 3.14;

	array <object> types = [
		object(xs_type = "int", output_type = "Integer"),
		object(xs_type = "float", output_type = "Double"),
		object(xs_type = "string", output_type = "String")
	];
	
	//*********************
	//* TESTS IMPLEMENTED *
	//*********************
	
	//[ok]: raise redefined symbol
	//var some_variable = "it's a variable test.";
	//int some_variable;
	
	
	//[ok]: raise assign type mistmatch
	//instance1.px_value = "value";
	
	
	//[ok]: raise unknown identifier
	//value = "value";

	
	//[ok]: raise cannot resolve operator + 'int' and 'var'
	//int vi = 10;
	//vi += 20 + somenotexistsobject.getValue();
	
	
	//[ok]: raise not suitable cast from int to float
	//int c0 = 2;
	//float c1 = 3.14;
	//c1 = c0;
	
	
	//[ok]: deduce type from assign variable declaration value
	var v0 = 20;
	var v1 = instance1.px_value;
	var v2 = "string";
	var v3 = 3.14;
	var v4 = true;
	
	
	//[ok]: deduce type from assign value
	var x0;
	x0 = instance1.px_value;
	var x1;
	x1 = somenotexistsobject.px_value;
	
	
	//[*ok]: deduce property type from assign value
	instance1.ps_value = x0;
	var x2;
	x2 = instance1.ps_value;
	
	instance1.svalue = x0;
	var x3;
	x3 = instance1.svalue;

	lvalue = x0;
	var x4;
	x4 = lvalue;
	
	
	//[ok]: simplest type cast 
	int ivar = 100;
	string svar = ivar as string;
	float fval = svar as float;
	
	
	//[ok]: deduce array type from assign variable declaration value
	object a0 = [1, 2];
	a0.setValue(j);
	
	var a1 = [1, 2];
	a1 = [3, 4, 5, 6, 7];
	
	var a2;
	a2 = [1, 2, 3];
	
	
	//[ok]: typified array with add, remove a reassign elements
	//      Then set new value of an element, resolve other one
	//      and iterate array items.
	array<int> list = [1, 2, 3];
	
	list += 4;
	list -= 2;
	
	list = [5, 6, 7, 8];
	
	//list[0] = 2 * 3;
	var itx = list[2];
	
	var size = list.size;
	
	for(int e in list)
	{
		System.out.println(e);
	}
	
	
    //[ok]: simple variable operations
	int s0 = 20;
	s0 = 5;
	s0 = list[2];

	var s1 = 0;
	s1 = 1;
	s1 += s0;
	s1 -= 2;
	
	int s2 = s1 + s0;
	int s3 = s1 * s2;
	
	if(s1 != 0)
	{
		int s4 = s3 / s1;
	}
	
	
	//[ok]: if conditions with relational operators
	int l1 = 2;
	int l2 = 3;
	int l3 = 3;
	
	if(l1 == l2 && l2 == l3)
	{
		System.out.println("equilateral");
	}
	else
	if(l1 != l2 && l2 != l3 && l3 != l1)
	{
		System.out.println("scalene");
	}
	else
	{
		System.out.println("isosceles");
	}
	
	
	//[ok]: resolve type from method return type
	var rmthd;
	rmthd = app_test(10);
	var thd = application.app_test(2);

	var mthd;
	// 10; op_param; 'app_test'; 1; op_func_call;
	//mthd = app_test(10);
	// 'instance1'; 'test0'; op_dot_call; 10; op_param; 1; op_call;
	mthd = instance1.test0(10);
	// 'instance1'; 'test0'; op_dot_call; 10; op_param; "a"; op_param; -2; op_param; 3; op_call;
	mthd = instance1.test0(10, "a", -2);
	// 'instance1'; 'test0'; op_dot_call; 2; 5; op_mult; op_param; "b"; "a"; op_plus; op_param; 1; op_param; 3; op_call;
	mthd = instance1.test0(2 * 5, "b" + "a", 1);
}

method app_test(int value)
{
	return "string value";
}

instance instance1
{
	property xvalue = 100.2;
	
	property svalue;
	
	method test0(int value)
	{
		var a = value;
		float b = 5.0;
		return a * b;
	}
	
	method test1(int value)
	{
		float b = 5.0;
		return b / 2;
	}
	
	method test2(int value)
	{
		var a = value;
		float b = 5.0;
		return application.app_test(a * b);
	}
	
	method test3(value)
	{
		return value;
	}
	
	method foo(var obj)
	{
		bool has_property = obj has "processed"; //has it been here?
		if(!has_property)
		{
			//ERROR: assert
			//obj.processed = true; //now "has" would be true
		}
	}
}
