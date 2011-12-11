
on click(x, y){
	var cylinder_1 = my_cylinder(200,100,100,500);	
	cylinder_1.hex = Blue;
	cylinder_1.position.x = x;
	cylinder_1.position.y = y;	
	cylinder_1.rotation.x = 10;
	cylinder_1.rotation.y = 10;
	scene.addObject(cylinder_1);	
}