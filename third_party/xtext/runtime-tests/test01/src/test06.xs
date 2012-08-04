class my_cylinder : cylinder
{	
	property hex
	{
		color = value;
	}
	on mousein() {
		color = lightgreen;
	}
	on mouseout() {
		color = red;
	}
}
