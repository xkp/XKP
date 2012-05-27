on init()
{
	var cylinder_1 = my_cylinder(150, 100, 400, 20, 20);
	cylinder_1.position.x = 0;
	cylinder_1.position.y = 0;	
	cylinder_1.rotation.x = 30;
	cylinder_1.rotation.y = 10;	
}

on click(x, y){
	var cylinder_1 = my_cylinder(150, 100, 400, 20, 20);	
	cylinder_1.hex = red;
	cylinder_1.position.x = x;
	cylinder_1.position.y = y;	
	cylinder_1.rotation.x = 30;
	cylinder_1.rotation.y = 10;
	scene.addObject(cylinder_1);	
}