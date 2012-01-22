property timer;
property objects = [];

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
		scene.addObject( sphere_1 );
	}
}

on updates()
{
	timer = g_elapsed * 0.0001;
	camera_1.position.x = Math.cos( timer ) * 1000;
	camera_1.position.z = Math.sin( timer ) * 1000;

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