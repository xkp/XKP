class my_image : img
{	
	on click()
	{
		x_pos = this.x;
		y_pos = this.y;
		
		traslate();
		img2 = my_image(src = "images/img1.png");
		
		img2.rect(x_pos + 50, y_pos - 50, 50, 50);
	}
}