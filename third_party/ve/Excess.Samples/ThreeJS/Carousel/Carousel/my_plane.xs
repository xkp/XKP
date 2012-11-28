class my_plane : transform
{	
	method set_image(value)
	{
		texture = value;
	}
	on mousein(){
		scale.x = 1.5;
		scale.y = 1.5;	
	}	
	on mouseout(){
		scale.x = 1;
		scale.y = 1;
	}	
}