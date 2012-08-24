property mouse_x;
property mouse_y;
property t = 0;
property cam = 0;

on update(delta, elapsed){		
	
	var time = elapsed * 0.0005;

	light1.position.x = Math.sin( time * 7 ) * 3009;
	light1.position.y = Math.cos( time * 5 ) * 4000;
	light1.position.z = Math.cos( time * 3 ) * 3009;	
}

on keydown(keycode)
{
	if (keycode == C)
	{
		if(cam < 2)
			cam ++;
		else
			cam = 0;
	}
}

on switch_cameras()
{
	if(cam == 0)
		set_active_camera(model1.model_cam);
	else if(cam == 1)
		set_active_camera(model1.model_cam1);
	else
		set_active_camera(model1.model_cam2);
}