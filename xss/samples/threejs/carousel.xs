property angle;
property start = 0;
property speed = 0.2;
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