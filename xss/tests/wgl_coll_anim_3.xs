property mouse_x;
property mouse_y;
property status = "idle";

on init()
{
	model1.char_anim.start("idle");
}

on update(delta, elapsed)
{			
	var time = elapsed * 0.0005;	
	
	light1.position.x = Math.sin( time * 7 ) * 3009;
	light1.position.y = Math.cos( time * 5 ) * 4000;
	light1.position.z = Math.cos( time * 3 ) * 3009;
	light2.position.x = Math.sin( time * 7 ) * 3009;
	light2.position.y = Math.cos( time * 3 ) * 4000;
	light2.position.z = Math.cos( time * 5 ) * 3009;	
	light3.position.x = Math.sin( time * 3 ) * 3009;
	light3.position.y = Math.cos( time * 5 ) * 4000;
	light3.position.z = Math.cos( time * 7 ) * 3009;	
}

on keydown(keycode)
{
	if (keycode == W)
	{
		model1.char_anim.start("walk");
	}
	else if (keycode == R)
	{
		model1.char_anim.start("run");
	}
	else if (keycode == C)
	{
		model1.char_anim.start("conversation");
	}
	else if (keycode == A)
	{
		model1.char_anim.start("alert");
	}	
}

on keyup()
{
	model1.char_anim.start("idle");
}
