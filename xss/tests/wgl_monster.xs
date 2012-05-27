property mouse_x;
property mouse_y;

on update(){		
	camera_1.position.x += ( mouse_x - camera_1.position.x ) * .05;
	camera_1.position.y += ( - mouse_y - camera_1.position.y ) * .05;
	monster_model.rotation.y += 1;
}

on mousemove(x,y){
	mouse_x = ( x - app_width/2 );
	mouse_y = ( y - app_height/2 );
}
