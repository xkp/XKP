class my_image : img
{	
	property x_pos;
	property y_pos;
	property img2;
	
	method traslate()
	{
		x += 5;
		y += 5;
		rotation += 10;
	}
	
	on click()
	{
		x_pos = x;
		y_pos = y;
		
		traslate();
		img2 = my_image(src = "images/img1.png");
		
		img2.rect(x_pos + 50, y_pos - 50, 50, 50);
	}
}