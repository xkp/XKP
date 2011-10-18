property is_ctrl_down = false;
property is_shift_down = false;

on keydown(){
	if(key_code == SHIFT) {
		is_shift_down = true;
		is_ctrl_down = false;
	}
	if(key_code == CTRL) {
		is_ctrl_down = true;
		is_shift_down = false;
	}
}

on switch_cameras(){
	if ( is_ctrl_down ) 
	{
		set_active_camera( camera_1 );			
	}else
	{
		set_active_camera( camera_2 );	
	}
}