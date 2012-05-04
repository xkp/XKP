class my_image : img
{	
	property x_pos;
	property y_pos;
	property img2;
	
	property image
	{
		src = value;
	}
	
	method traslate()
	{
		this.x += 5;
		this.y += 5;
		this.set_rotation(this.rotation + 10);
	}
	
	on click()
	{
		x_pos = this.x;
		y_pos = this.y;
		
		traslate();
		img2 = my_image(src = "images/img1.png");
		
		img2.rect(x_pos + 50, y_pos - 50, 50, 50);
	}
}