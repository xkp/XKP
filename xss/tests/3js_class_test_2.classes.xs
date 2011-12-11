class my_cylinder : cylinder
{	
	property hex
	{
		color = value;
	}
	on mousein(){
		color = Fuchsia;
	}
	on mouseout(){
		color = Red;
	}
}