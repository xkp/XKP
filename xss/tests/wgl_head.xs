property mouse_x;
property mouse_y;

on update(){		
	head_model.rotation.y = mouse_x * 0.3;
	head_model.rotation.x = mouse_y * 0.3;
}

on mousemove(x,y){
	mouse_x = ( x - app_width/2 );
	mouse_y = ( y - app_height/2 );
}
