property some_str : string = "Yo yo";
property int_prop : int = 345;

property some_accesors : int = 
{
	target = 500;
	return 20;
}
{
	ss.alert(some_accesors);
}	

property target
{
	//do something here, on the set
	btnSpin.width = 150;
}

on btnSpin.click()
{
	cube1.rotateZ( math.todegree(10 + cube1.position.x*(cube1.size.x + cube1.size.z) ));
	
	int some = 67 + application.some_accesors;
}

method display_me(int what)
{
	out << what;
}