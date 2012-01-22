property x;
property y;

on btn.click(){
	for(var i = 0; i < 4; i++ ){
		if(i%2 == 0){
			x = p1.points[i].x + 5;
			y = p1.points[i].y + 5;
		}else{
			x = p1.points[i].x - 5;
			y = p1.points[i].y - 5;
		}
		p1.setPointbyIndex(i, x, y);		
	}
	line1.line_width ++;
}

on r1.click(){
	r1.stroke = "brown";		
}

on c1.click(){		
	c1.line_width ++;
}

