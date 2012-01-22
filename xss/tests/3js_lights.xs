property time;

on updates()
{
	time = g_elapsed * 0.0005;

	sphere1.rotation.y -= 0.5;
	sphere1.rotation.x -= 0.5;

	particle1.position.x = Math.sin( time * 0.7 ) * 30;
	particle1.position.y = Math.cos( time * 0.5 ) * 40;
	particle1.position.z = Math.cos( time * 0.3 ) * 30;

	light1.position.x = particle1.position.x;
	light1.position.y = particle1.position.y;
	light1.position.z = particle1.position.z;

	particle2.position.x = Math.cos( time * 0.3 ) * 30;
	particle2.position.y = Math.sin( time * 0.5 ) * 40;
	particle2.position.z = Math.sin( time * 0.7 ) * 30;

	light2.position.x = particle2.position.x;
	light2.position.y = particle2.position.y;
	light2.position.z = particle2.position.z;

	particle3.position.x = Math.sin( time * 0.7 ) * 30;
	particle3.position.y = Math.cos( time * 0.3 ) * 40;
	particle3.position.z = Math.sin( time * 0.5 ) * 30;

	light3.position.x = particle3.position.x;
	light3.position.y = particle3.position.y;
	light3.position.z = particle3.position.z;
}