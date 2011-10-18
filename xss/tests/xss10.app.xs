property counter = 0;

instance it1
{
	property getting =
	{
		return application.counter * 10;
	}
	
	on create()
	{
		application.counter = getting;
		
		//ERROR 1:
		//classy.mthd_1( classy.mthd_2( 3 ) );
		//classy.mthd_1( 2 );
		
		//ERROR 2: not recognize int type of array 
		//       on variable_ methods
		//array<int> a;
		//a += 2;
		
		//ERROR 3: op_index is not implemented
		//a[0] = 4;
	}
}

instance it2
{
	property counter = 0;

	property getting = 
	{
		return application.counter / 10;
	}
	
	property setting
	{
		counter = value + getting;
	}
	
	method returning()
	{
		it1.mthd_1();
		
		if (it1.size > it2.size) 
		{
			return application.counter;
		} 
		else 
		{
			return 0;
		}
	}
	
	on click()
	{
		setting = getting - 12;
	}
}
