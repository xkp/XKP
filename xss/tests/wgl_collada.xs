property mouse_x;
property mouse_y;

on init()
{
	string model_src = "model/monster/monster.dae";
	monster_model.model_skin.start("walk");
}

on update(delta, elapsed){		
	
	var time = elapsed * 0.0005;

	camera_1.position.x += ( mouse_x - camera_1.position.x ) * .05;
	camera_1.position.y += ( - mouse_y - camera_1.position.y ) * .05;	
	light1.position.x = Math.sin( time * 7 ) * 3009;
	light1.position.y = Math.cos( time * 5 ) * 4000;
	light1.position.z = Math.cos( time * 3 ) * 3009;
	light2.position.x = Math.sin( time * 3 ) * 3009;
	light2.position.y = Math.cos( time * 5 ) * 4000;
	light2.position.z = Math.cos( time * 7 ) * 3009;
	light3.position.x = Math.sin( time * 7 ) * 3009;
	light3.position.y = Math.cos( time * 3 ) * 4000;
	light3.position.z = Math.cos( time * 5 ) * 3009;	
}

on mousemove(x, y){
	mouse_x = ( x - app_width/2 );
	mouse_y = ( y - app_height/2 );
}
