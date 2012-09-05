property mouse_x;
property mouse_y;
  
on init()
{
	model1.anim.start();	
}

on update(delta, elapsed){		
	
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

on switch_cameras()
{
	set_active_camera(model1.model_cam2);
}
