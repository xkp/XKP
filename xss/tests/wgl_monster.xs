property mouse_x;
property mouse_y;

on init()
{
	monster_model.model_skin.start("walk");
}

on update(){		
	camera_1.position.x += ( mouse_x - camera_1.position.x ) * .05;
	camera_1.position.y += ( - mouse_y - camera_1.position.y ) * .05;	
}

on mousemove(x,y){
	mouse_x = ( x - app_width/2 );
	mouse_y = ( y - app_height/2 );
}

on click()
{
	monster_model.model_skin.stop("walk");
}