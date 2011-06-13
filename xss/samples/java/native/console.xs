on pbtn1.click()
{
	array<int> lx = [1, 2, 3];
	lx += 69;
	lx -= 2;
	lx[0] = 2 * 3;							// java traduction: lx.set(0, 2 * 3);
	int leftv = lx[3];
	
	var ex2;
	ex2 = xvalue;
	ex2 = pbtn1.xvalue;
	
	var mthd;
	mthd = test(10); 						// 10; op_param; 'test'; 1; op_func_call;
	mthd = pbtn1.test(10);					// 'pbtn1'; 'test'; op_dot_call; 10; op_param; 1; op_call;
	mthd = pbtn1.test(10, "a", -2);			// 'pbtn1'; 'test'; op_dot_call; 10; op_param; "a"; op_param; -2; op_param; 3; op_call;
	mthd = pbtn1.test(2 * 5, "b" + "a", 1);	//
	
	object obj = [1, 2];
	obj.setValue(j);
	
	int j = 20;
	j = 5;
	//j = lx[3];

	var i = 0;
	i = 1;
	i += j;
	i -= 2;
	
	array<int> list = [1, 2, 3];
	list += 4;
	for(int e in list)
	{
		System.out.println(e);
	}
	list[0] = 2 * 3;

	var xxx = [1, 2, 3];
	xxx = [4, 5, 6, 7];
	
	int a = 10;
	//a += j + obj.getValue();
	
	// how xs traduce this
	var v1 = 10;
	int v2 = 20;
	//v1 = v2 = 30;
	//v1 * v2 = 30;
	if(v1 = 10 * v2)
	{
	}
	
	// test property with assigned data type from classes
	// and assign value of different data type, then
	// assign property value to variant variable...
	
	// this make a assert
	//pbtn1.exploit = "exploit";
	//exploit = "exploit";
	//var ex1 = pbtn1.exploit;
	var ex1 = exploit;
	
	//var ex2;
	//ex2	= xvalue;

	var exp;
	exp = exploit;
	
	//it'snt possible, this make an console error
	//int xyx = 1;
	//for(; xyx < 5; xyx++)
	//{
	//}
	
	//*********************
	//* TESTS IMPLEMENTED *
	//*********************

	//[ok]: test simple variant type with initialization
	var w = 2;
	
	//[ok]: test simplest type cast 
	int ivar = 100;
	string svar = ivar as string;
	float fval = svar as float;

	//[ok]: test variant variable for resolve type 
	// 		from returned method value
	var yyy;
	yyy = test(10);
	
	//[ok]: test redefined variable
	//int v1;
}

//console error with message: "'syntax error': error while compiling xs"
//method pbtn1.testa()

method app_test(int value)
{
	return "exploit";
}

instance pbtn1
{
	//property xvalue : int = 28;
	//property xvalue : string = "one string";
	//property xvalue = "exploit";
	property xvalue = 100.2;
	
	method test(int value)
	{
		var a = value;
		float b = 5;
		//return b / 2;
		return a * a;
		//return "exploit";
		//return application.app_test(2);
	}
}
