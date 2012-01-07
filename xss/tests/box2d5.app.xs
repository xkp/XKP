
on mouse_drag()
{
	
}

on keydown(keycode)
{
	if (keycode == LEFT_ARROW)
		btn1.x -= 5;

	if (keycode == RIGHT_ARROW)
		btn1.x += 5;
		
	if (keycode == UP_ARROW)
		btn1.y -= 5;

	if (keycode == DOWN_ARROW)
		btn1.y += 5;
}



