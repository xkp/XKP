
on keydown(keycode)
{
	if (keycode == EKeyEvent.left_arrow)
		btn1.x -= 5;

	if (keycode == EKeyEvent.right_arrow)
		btn1.x += 5;
		
	if (keycode == EKeyEvent.up_arrow)
		btn1.y -= 5;

	if (keycode == EKeyEvent.down_arrow)
		btn1.y += 5;
}

/*
	** box2d differences
		- arrow constants are changed for enums types
*/
