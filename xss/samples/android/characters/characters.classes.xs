class character : div
{
	property name : string =
	{
		return lname.caption;
	}
	{
		lname.caption = value;
	}
	
	property surname : string =
	{
		return lsurname.caption;
	}
	{
		lsurname.caption = value;
	}
	
	property employment : string =
	{
		return lemployment.caption;
	}
	{
		lemployment.caption = value;
	}
	
	property img_src : string 
	{
		picture.src = value;
	}
	
	method foo()
	{
		name = "foo";
	}
	
	method bar()
	{
		surname = "bar";
	}

	/*
	instance picture
	{
		on click()
		{
			//do something
		}
		
		property color : bool = false;
		
		method str_dimensions()
		{
			string str_width = picture.width as string;
			string str_height = picture.height as string;
			
			return "[" + str_width + "x" + str_height + "]";
		}
	}
	*/
}
