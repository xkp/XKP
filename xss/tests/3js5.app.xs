property is_shift_down = false;
property is_ctrl_down = false;
property theta = 45;
property mouse_x;
property mouse_y;

on mousemove(){
	mouse_x = ( x / window.innerWidth ) * 2 - 1;
	mouse_y = - ( y / window.innerHeight ) * 2 + 1;
	if(is_shift_down){
		container.rotation.x += 0.02;
		container.rotation.y += 0.02;
		container.rotation.z += 0.02;
	}else if(is_ctrl_down){
		
	}else{
		cylinder_1.rotation.x += 0.02;
		cylinder_1.rotation.y += 0.02;
		cylinder_1.rotation.z += 0.02;
	}
}
on keydown(){
	if(key_code == 16) {
		is_shift_down = true;
	}
	if(key_code == 17) {
		is_ctrl_down = true;
	}
}
on keyup(){
	if(key_code == 16) {
		is_shift_down = false;
	}
	if(key_code == 17) {
		is_ctrl_down = false;
	}
}
on animate(){
	if ( is_ctrl_down ) {
		theta += mouse_x * 3;
	}
	cam1.position.x = 1400 * Math.sin( theta * Math.PI / 360 );
	cam1.position.z = 1400 * Math.cos( theta * Math.PI / 360 );
}