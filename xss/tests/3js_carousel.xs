property angle;
property start = 0;
property speed = 0.02;
property step = 2*Math.PI/5;
property max_rotation = 2*Math.PI;
property img_array = [img1, img1_reflect, img2, img2_reflect, img3, img3_reflect, img4, img4_reflect, img5, img5_reflect];

on animate(){
	position_items();
}

method position_items(){
	
		start += speed;
		if (start > max_rotation)
			start = 0;	
				
		for(int i = 0; i < 10; i+=2){	
			angle = start+i*step;
			img_array[i].position.x = 275*Math.cos(angle);
			img_array[i+1].position.x = 275*Math.cos(angle);
			img_array[i].position.z = 275*Math.sin(angle);
			img_array[i+1].position.z = 275*Math.sin(angle);
		}
		
}

on mousemove(var x, var y){
	
	if (x < app_width/2){
		speed = -(0.2) * ( app_width/2 - x) / ( app_width/2);
	}
	if (x > app_width/2){
		speed = -(0.2) * ( app_width/2 - x) / ( app_width/2);
	}
}

on img1.mousein(){
	img1.scale.x = 1.5;
	img1.scale.y = 1.5;
	img1_reflect.scale.x = 1.5;
	img1_reflect.scale.y = 1.5;
	img1_reflect.position.y = -196.8; 
}

on img1.mouseout(){
	img1.scale.x = 1;
	img1.scale.y = 1;
	img1_reflect.scale.x = 1;
	img1_reflect.scale.y = 1;
	img1_reflect.position.y = -131.2; 
}
on img2.mousein(){
	img2.scale.x = 1.5;
	img2.scale.y = 1.5;
	img2_reflect.scale.x = 1.5;
	img2_reflect.scale.y = 1.5;
	img2_reflect.position.y = -196.8; 
}

on img2.mouseout(){
	img2.scale.x = 1;
	img2.scale.y = 1;
	img2_reflect.scale.x = 1;
	img2_reflect.scale.y = 1;
	img2_reflect.position.y = -131.2; 
}
on img3.mousein(){
	img3.scale.x = 1.5;
	img3.scale.y = 1.5;
	img3_reflect.scale.x = 1.5;
	img3_reflect.scale.y = 1.5;
	img3_reflect.position.y = -196.8; 
}

on img3.mouseout(){
	img3.scale.x = 1;
	img3.scale.y = 1;
	img3_reflect.scale.x = 1;
	img3_reflect.scale.y = 1;
	img3_reflect.position.y = -131.2; 
}
on img4.mousein(){
	img4.scale.x = 1.5;
	img4.scale.y = 1.5;
	img4_reflect.scale.x = 1.5;
	img4_reflect.scale.y = 1.5;
	img4_reflect.position.y = -196.8; 
}

on img4.mouseout(){
	img4.scale.x = 1;
	img4.scale.y = 1;
	img4_reflect.scale.x = 1;
	img4_reflect.scale.y = 1;
	img4_reflect.position.y = -131.2; 
}
on img5.mousein(){
	img5.scale.x = 1.5;
	img5.scale.y = 1.5;
	img5_reflect.scale.x = 1.5;
	img5_reflect.scale.y = 1.5;
	img5_reflect.position.y = -196.8; 
}

on img5.mouseout(){
	img5.scale.x = 1;
	img5.scale.y = 1;
	img5_reflect.scale.x = 1;
	img5_reflect.scale.y = 1;
	img5_reflect.position.y = -131.2; 
}
