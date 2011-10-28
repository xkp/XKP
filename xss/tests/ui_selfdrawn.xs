property clicks = 0;

on btn1.click()   
{	
	lbl1.caption = "Got " + clicks + " smileys";
	btn1.rotation += 0.2;
	clicks++;	
}

instance sd1
{
	on paint(context)
	{	
		context.fillStyle = 'cyan';
        context.fillRect(-100, -100, 200, 200);
		context.fillStyle = 'green';
        context.beginPath();
        context.lineWidth = 3;
        context.moveTo(-50, -50);
        context.lineTo(50, 50);
        context.stroke();
	}
}

on keydown(keycode)
{
	if(btn1.isVisible()){
		if(keycode == LEFT_ARROW)
			btn1.x -= 2;
		
		if(keycode == RIGHT_ARROW)
			btn1.x += 2;
		
		if(keycode == UP_ARROW)
			btn1.y -= 2;
		
		if(keycode == DOWN_ARROW)
			btn1.y += 2;
	}
}


