property mouse_x;
property mouse_y;
property spheres = [];
property timer;

on init()
{
	for (int i = 0; i < 200; i++ ) 
	{
		var sphere_1 = my_sphere(100,32,16);	
		sphere_1.mat = material_sphere1;
		sphere_1.position.x = Math.random() * 10000 - 5000;
		sphere_1.position.y = Math.random() * 10000 - 5000;
		sphere_1.position.z = Math.random() * 10000 - 5000;
		sphere_1.scale.x = Math.random() * 3 + 1;
		sphere_1.scale.y = sphere_1.scale.x;
		sphere_1.scale.z = sphere_1.scale.x;
		spheres += sphere_1;
		scene.add( sphere_1 );
	}
}

on update()
{	
	timer = g_elapsed * 0.0001;
	camera_1.position.x += ( mouse_x - camera_1.position.x) * 0.05;
	camera_1.position.y += ( - mouse_y - camera_1.position.y) * 0.05;	
	for ( var i = 0; i < spheres.length;  i++ ) {
		var sphere = spheres[ i ];
		sphere.position.x = 5000 * Math.cos( timer + i );
		sphere.position.y = 5000 * Math.sin( timer + i * 1.1 );
	}
}

on mousemove(x, y)
{
	mouse_x = ( x - window.innerWidth / 2 ) * 10;				
	mouse_y = ( y - window.innerHeight / 2 ) * 10;
}