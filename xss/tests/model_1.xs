property mouse_x;
property mouse_y;

on animate(){		
	camera_1.position.x += ( mouse_x - camera_1.position.x ) * .05;
	camera_1.position.y += ( - mouse_y - camera_1.position.y ) * .05;	
}

on mousemove(x,y){
	mouse_x = ( x - window.innerWidth/2 );
	mouse_y = ( y - window.innerHeight/2 );
}

on female_model.click(){
	female_model.position.x -= 10;	
}

on male_model.click(){
	male_model.position.x += 10;	
}