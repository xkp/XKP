property mouse_x;
property mouse_y;
property t = 0;
property cam = false;

on update(delta, elapsed){		
	
	var time = elapsed * 0.0005;

	light1.position.x = Math.sin( time * 7 ) * 3009;
	light1.position.y = Math.cos( time * 5 ) * 4000;
	light1.position.z = Math.cos( time * 3 ) * 3009;	
}

on click()
{
	model1.src = "pump";
}

on keydown(keycode)
{
	if (keycode == C)
	{
		cam = !cam;
	}
	if (keycode == LEFT_ARROW){
		model1.position.x -= 1;
	}

	if (keycode == RIGHT_ARROW){
		model1.position.x += 1;
	}
		
	if (keycode == UP_ARROW){
		model1.position.y -= 1;
	}

	if (keycode == DOWN_ARROW){
		model1.position.y += 1;
	}
}

on switch_cameras()
{
	if(cam)
		set_active_camera(model1.model_cam);
	else
		set_active_camera(camera_1);
}