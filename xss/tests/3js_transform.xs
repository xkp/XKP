property is_shift_down = false;
property is_ctrl_down = false;
property theta = 45;
property mouse_x;
property mouse_y;

on mousemove(x, y){
	mouse_x = ( x / app_width ) * 2 - 1;
	mouse_y = - ( y / app_height ) * 2 + 1;
	if(is_shift_down){
		container.rotation.x += 0.5;
		container.rotation.y += 0.5;
		container.rotation.z += 0.5;
	}else if(is_ctrl_down){
		
	}else{
		cylinder_1.rotation.x += 0.5;
		cylinder_1.rotation.y += 0.5;
		cylinder_1.rotation.z += 0.5;
	}
}

on cube_1.mousein()
{
	cube_1.rotation.x += 0.5;
	cube_1.rotation.y += 0.5;
	cube_1.rotation.z += 0.5;
}

on keydown(keycode){
	if(keycode == SHIFT) {
		is_shift_down = true;
	}
	if(keycode == CTRL) {
		is_ctrl_down = true;
	}	
}
on keyup(keycode){
	if(keycode == SHIFT) {
		is_shift_down = false;
	}
	if(keycode == CTRL) {
		is_ctrl_down = false;
	}
}
on updates(){
	if ( is_ctrl_down ) {
		theta += mouse_x * 3;
	}
	camera_1.position.x = 1400 * Math.sin( theta * Math.PI / 360 );
	camera_1.position.z = 1400 * Math.cos( theta * Math.PI / 360 );
}