property clicks = 0;

on btnNext.click()
{
    if (main.active < main.count - 1)
        main.active++;
	btnNext.x ++;
	btnNext.rotation += 0.2;
}

on btnPrev.click()
{
    if (main.active > 0)
        main.active--;	
}

on btn1.click()   
{
	
	lbl1.caption = "Got " + clicks + " smileys";
	btn1.rotation += 0.2;
	clicks++;
	
}

on btn2.click()
{
    pb1.value--;
}

on btn3.click()
{
    pb1.value++;
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


