property angle;
property start = 0;
property speed = 0.02;
property step = 2*Math.PI/5;
property max_rotation = 2*Math.PI;
property img_array = ["../images/test1.png", "../images/test2.png", "../images/test3.png", "../images/test4.png", "../images/test5.png"];
property plane_array = [];

on update(){
	position_items();
}

on init(){	
	for(int i = 0; i < 5; i++){	
		plane_array += my_plane();
		plane_array[i].set_image(img_array[i]); 
		scene.add(plane_array[i]);
	}
}

method position_items(){	
		start += speed;
		if (start > max_rotation)
			start = 0;	
		for(int i = 0; i < 5; i++){	
			angle = start+i*step;
			plane_array[i].position.x = 275*Math.cos(angle);			
			plane_array[i].position.z = 275*Math.sin(angle);			
		}		
}

on mousemove(x, y){	
	if (x < app_width/2){
		speed = -(0.2) * ( app_width/2 - x) / ( app_width/2);
	}
	if (x > app_width/2){
		speed = -(0.2) * ( app_width/2 - x) / ( app_width/2);
	}
}

on click(){
	
}
