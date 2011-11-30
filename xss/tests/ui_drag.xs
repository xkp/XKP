property clicks = 0;

on btn1.click()   
{	
	lbl1.caption = "Got " + clicks + " smileys";
	btn1.rotation += 10;
	clicks++;	
}

on btn1.drag(x, y)
{
	btn1.x = x - btn1.w/2;
	btn1.y = y - btn1.h/2;
}

