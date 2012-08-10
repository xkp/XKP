property mouse_x;
property mouse_y;
property t = 0;

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
	
	if ( t > 30 ) t = 0;
	if ( monster_model.skin ) {
		for ( var i = 0; i < monster_model.skin.morphTargetInfluences.length; i++ ) {
			monster_model.skin.morphTargetInfluences[ i ] = 0;
		}
		monster_model.skin.morphTargetInfluences[ Math.floor( t ) ] = 1;
		t += 0.5;
	}
}

on mousemove(x, y){
	mouse_x = ( x - app_width/2 );
	mouse_y = ( y - app_height/2 );
}
