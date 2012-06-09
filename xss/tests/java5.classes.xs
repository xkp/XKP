class custom_clazz : java5
{
	property x 		= 0;
	property y 		= 0;
	property width 	= 10;
	property height = 10;
	
	property radius : int =
	{
		return value;
	}
	{
		width 	= value * 2;
		height 	= value * 2;
	}
	
	on java_event(value)
	{
		width 	+= 10;
		height 	+= 10;
	}
	
	method set_default()
	{
		width 	= 10;
		height 	= 10;
	}
}
