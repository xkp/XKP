property x_target_rotation = 0;
property x_target_rotation_on_mouse_down = 0;
property mouse_x = 0;
property mouse_x_on_mouse_down = 0;
property y_target_rotation = 0;
property y_target_rotation_on_mouse_down = 0;
property mouse_y = 0;
property mouse_y_on_mouse_down = 0;
property window_half_x = window.innerWidth / 2;
property window_half_y = window.innerHeight / 2;
property is_mouse_down = false;

on mousedown(x, y){
	mouse_x_on_mouse_down = x - window_half_x;
	x_target_rotation_on_mouse_down = x_target_rotation;
	mouse_y_on_mouse_down = y - window_half_y;
	y_target_rotation_on_mouse_down = y_target_rotation;
}

on drag(x, y){	
	mouse_x = x - window_half_x;
	x_target_rotation = x_target_rotation_on_mouse_down + ( mouse_x - mouse_x_on_mouse_down ) * 0.3;
	mouse_y = y - window_half_y;
	y_target_rotation = y_target_rotation_on_mouse_down + ( mouse_y - mouse_y_on_mouse_down ) * 0.3;	
}

on animate(){
	cube_1.rotation.y += ( x_target_rotation - cube_1.rotation.y ) * 0.2 ;
	plane_1.rotation.z = cube_1.rotation.y;
	cube_1.rotation.x += ( y_target_rotation - cube_1.rotation.x ) * 0.2 ;
}