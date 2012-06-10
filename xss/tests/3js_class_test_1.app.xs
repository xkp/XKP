property img_array = ["images/test1.png", "images/test2.png", "images/test3.png", "images/test4.png", "images/test5.png"];
property i = 0;

on click(x, y){
	if(i == 5)
		i = 0;
	var plane_1 = my_plane(200,260);
	plane_1.image = img_array[i];
	plane_1.position.x = x;
	plane_1.position.y = y;
	plane_1.rotation.x = 90;
	scene.add(plane_1);
	i++;
}