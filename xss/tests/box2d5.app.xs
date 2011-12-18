
on btn1.drag(x, y)
{
	btn1.x = x - btn1.w/2;
	btn1.y = y - btn1.h/2;
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



