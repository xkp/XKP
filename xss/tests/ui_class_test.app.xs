property test;
property x_pos = 100;
property y_pos = 100;
property X_MAX = 600;
property Y_MAX = 400;
property img2;

method init()
{
    test = 1;
}

on img1.click()
{
    if(x_pos >= (X_MAX - 110))
		x_pos = 0;
	x_pos += 60;
	if(y_pos >= (Y_MAX - 110))
		y_pos = 0;
	y_pos += 60;
    img1.rotation += 10;
	img1.x += 5;
	img1.y += 5;
	img2 = my_image("images/img1.png");
	img2.rect(x_pos,y_pos,50,50);
}


