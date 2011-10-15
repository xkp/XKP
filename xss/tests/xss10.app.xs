instance instance1
{
	on create()
	{
		//ERROR 1:
		classy.mthd_1( classy.mthd_2( 3 ) );
		classy.mthd_1( 2 );
		
		//ERROR 2: not succed this condition in vm, 
		//       see on xss implementation
		bool flag = true;
		if(flag == true)
		{
			flag = false;
		}
		
		//ERROR 3: not recognize int type of array 
		//       on variable_ methods
		array<int> a;
		a += 2;
		
		//ERROR 4: op_index is not implemented
		//a[0] = 4;
		
		//ERROR 5: assert exploit
		//object o;
		//o.prop = 2;
	}
}
