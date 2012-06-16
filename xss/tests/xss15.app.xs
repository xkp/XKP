property some_property : string = "some existing value";

on keydown(keycode, value)
{
	//string myvar = xss15_app.some_property;
	string myvar = application.some_property;
	
	//this work fine
	string other_var = some_property;
}
