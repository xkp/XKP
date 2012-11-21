property timer;
property objects = [];
bool flag = false;

on init()
{
	for (int i = 0; i < all_in_one.length; i++ ) 
	{
		var sphere_1 = my_sphere(70,32,16);	
		sphere_1.mat = all_in_one[ i ];
		sphere_1.position.x = ( i % 4 ) * 200 - 400;
		sphere_1.position.z = Math.floor( i / 4 ) * 200 - 200;
		sphere_1.rotation.x = Math.random() * 200 - 100;
		sphere_1.rotation.y = Math.random() * 200 - 100;
		sphere_1.rotation.z = Math.random() * 200 - 100;
		objects += sphere_1;
		scene.add( sphere_1 );
	}
}

on mousedown()
{
	flag = true;
}

on mouseup()
{
	flag = false;
}

on update()
{
	timer = g_elapsed * 0.0001;	

	camera_1.position.x = Math.cos( timer ) * 1000;
	camera_1.position.z = Math.sin( timer ) * 1000;
	if(flag)
		camera_1.look_at = objects[1].position;
	else
		camera_1.look_at = scene.position;
		
	all_in_one[9].emissive.setHSV( 0.54, 1, 0.7 * ( 0.5 + 0.5 * Math.sin( 35 * timer ) ) );	
				
	for ( int i = 0; i < objects.length; i++ ) {
		var object = objects[ i ];
		object.rotation.x += 0.01;
		object.rotation.y += 0.005;
	}
	particle_1.position.x = Math.sin( timer * 7 ) * 300;
	particle_1.position.y = Math.cos( timer * 5 ) * 400;
	particle_1.position.z = Math.cos( timer * 3 ) * 300;

	point_light_1.position.x = particle_1.position.x;
	point_light_1.position.y = particle_1.position.y;
	point_light_1.position.z = particle_1.position.z;
}