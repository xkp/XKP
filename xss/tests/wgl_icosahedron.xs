property mouseX = 0;
property mouseY = 0;
on mousemove(x,y){
	mouseX = (x - window.innerWidth/2);
	mouseY = (y - window.innerHeight/2);
}
on updates(){
	camera_1.position.x += ( mouseX - camera_1.position.x ) * 0.05;
	camera_1.position.y += ( - mouseY - camera_1.position.y ) * 0.05;
}
on icosahedron_1.mousein(){
	icosahedron_1.rotation.x += 2;
	icosahedron_1.rotation.y += 2; 
	icosahedron_1.rotation.z += 2; 	
}
on icosahedron_2.mousein(){
	icosahedron_2.rotation.x += 2;
	icosahedron_2.rotation.y += 2; 
	icosahedron_2.rotation.z += 2; 	
}
on icosahedron_3.mousein(){
	icosahedron_3.rotation.x += 2;
	icosahedron_3.rotation.y += 2; 
	icosahedron_3.rotation.z += 2; 	
}