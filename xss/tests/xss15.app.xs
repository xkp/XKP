property some_property : string = "some existing value";

on keydown(keycode, value)
{
	string myvar = application.some_property;
	
	//this work fine
	string other_var = some_property;
	
	//and this not work
	//string myvar = xss15_app.some_property;
}
