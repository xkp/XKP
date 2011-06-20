on pbtn1.click()
{
	// how traduce this?
	array <object> types = [
		object(xs_type = "int", output_type = "Integer"),
		object(xs_type = "float", output_type = "Double"),
		object(xs_type = "string", output_type = "String")
	];
	
	//*********************
	//* TESTS IMPLEMENTED *
	//*********************

	//[ok]: test property with assigned data type from classes
	// 		and assign value of different data type, then
	// 		assign property value to variant variable...
	pbtn1.exploit = 20;
	var ex1 = pbtn1.exploit;
	var exp;
	exp = exploit;
	// and correct raise type mistmatch 
	//pbtn1.exploit = "exploit";
	//exploit = "exploit";
	
	//[ok]: test some assigned object
	object obj = [1, 2];
	obj.setValue(j);
	
	//[ok]: test declaration of array type with assign, 
	//      add, remove and reasign elements. Then
	//		set the value of a element and resolve other one.
	array<int> list = [1, 2, 3];
	list += 4;
	list -= 2;
	for(int e in list)
	{
		System.out.println(e);
	}
	list = [5, 6, 7, 8];
	list[0] = 2 * 3;
	var itx = list[2];

	int j = 20;
	j = 5;
	j = list[2];

	var i = 0;
	i = 1;
	i += j;
	i -= 2;
	
	int a = 10;
	a += j + obj.getValue();
	
	//[ok]: test declaration of array variant with assign.
	var adyn = [1, 2, 3];
	adyn = [4, 5, 6, 7];
	
	//[ok]: test simple variant type with initialization
	var w = 2;
	
	//[ok]: test simplest type cast 
	int ivar = 100;
	string svar = ivar as string;
	float fval = svar as float;

	//[ok]: test variant variable for resolve type 
	// 		from returned method value and 
	//		test operator op_func_call
	var rmthd;
	rmthd = test(10);
	var thd = application.app_test(2);

	var mthd;
	mthd = test(10); 						// 10; op_param; 'test'; 1; op_func_call;
	mthd = pbtn1.test(10);					// 'pbtn1'; 'test'; op_dot_call; 10; op_param; 1; op_call;
	mthd = pbtn1.test(10, "a", -2);			// 'pbtn1'; 'test'; op_dot_call; 10; op_param; "a"; op_param; -2; op_param; 3; op_call;
	mthd = pbtn1.test(2 * 5, "b" + "a", 1);	//
	
	//[ok]: test variant variable for resolve type 
	// 		from returned property value and 
	//		test operator op_dot
	var ex2;
	ex2 = xvalue;
	ex2 = pbtn1.xvalue;
	var ex3 = application.yvalue;
	
	//[ok]: test assign property with a type 
	//      and some resolved casts
	application.yvalue = "new string";
	xvalue = 20.2;
	
	//[ok]: allow cast from int to float
	xvalue = 10;
	
	//[ok]: test redefined variable
	//int v1;
}

//console error with message: "'syntax error': error while compiling xs"
//method pbtn1.testa()

property yvalue = "string v";

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
