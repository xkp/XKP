property is_quake_camera = false;

on keyup(keycode){
	if(keycode == SHIFT) {
		is_quake_camera = !is_quake_camera;
	}	
}

on switch_cameras(){
	if ( is_quake_camera ) 
	{
		set_active_camera( camera_1 );			
	}else
	{
		set_active_camera( camera_2 );	
	}
}