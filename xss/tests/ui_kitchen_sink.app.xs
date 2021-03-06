property clicks = 0;
property do_rotation_plus = false;
property do_rotation_minus = false;
property do_drag = false;
property px;
property py;
property left_key_down = false;
property right_key_down = false;
property increase_pol_size = false;
property decrease_pol_size = false;

on lbl_event.mousein()
{
	lbl_event.caption = "The mouse is over me...";
}

on lbl_event.mouseout()
{
	lbl_event.caption = "Pass the mouse over this text...";
}

on btnNext.click()
{
	if(do_rotation_plus)
		btnNext.rotation += 5;
	if(do_rotation_minus)
		btnNext.rotation -= 5;
	if (main.active < main.count - 1)
        main.active++;	
}

on btnNext.drag(x, y)
{
	if(do_drag)
	{
		btnNext.x = x - btnNext.w/2;
		btnNext.y = y - btnNext.h/2;
	}
}

on title0.drag(x, y)
{
	if(do_drag)
	{
		title0.x = x - title0.w/2;
		title0.y = y - title0.h/2;
	}
}

on btnPrev.click()
{
	if(do_rotation_plus)
		btnPrev.rotation += 5;
	if(do_rotation_minus)
		btnPrev.rotation -= 5;
    if(main.active > 0)
        main.active--;	
}

on btnPrev.drag(x, y)
{
	if(do_drag)
	{
		btnPrev.x = x - btnPrev.w/2;
		btnPrev.y = y - btnPrev.h/2;
	}
}

on btn1.click()   
{	
	if(do_rotation_plus)
		btn1.rotation += 5;
	if(do_rotation_minus)
		btn1.rotation -= 5;
	lbl1.caption = "Got " + clicks + " smileys";
	clicks++;	
}

on btn1.drag(x, y)
{
	if(do_drag)
	{
		btn1.x = x - btnNext.w/2;
		btn1.y = y - btnNext.h/2;
	}
}

on btn2.click()
{
	if(do_rotation_plus)
		btn2.rotation += 5;
	if(do_rotation_minus)
		btn2.rotation -= 5;
    pb1.value--;
}

on btn2.drag(x, y)
{
	if(do_drag)
	{
		btn2.x = x - btnNext.w/2;
		btn2.y = y - btnNext.h/2;
	}
}

on btn3.click()
{
	if(do_rotation_plus)
		btn3.rotation += 5;
	if(do_rotation_minus)
		btn3.rotation -= 5;
    pb1.value++;
}

on btn3.drag(x, y)
{
	if(do_drag)
	{
		btn3.x = x - btnNext.w/2;
		btn3.y = y - btnNext.h/2;
	}
}

on stateBtn.click()
{
	if(stateBtn.is_active)
		lbl3.caption = "Wow I'm happy !!!";	
	else
		lbl3.caption = "Bad I'm sad... Click again on me";	
	if(do_rotation_plus)
		stateBtn.rotation += 5;
	if(do_rotation_minus)
		stateBtn.rotation -= 5;
}

on stateBtn.drag(x, y)
{
	if(do_drag)
	{
		stateBtn.x = x - btnNext.w/2;
		stateBtn.y = y - btnNext.h/2;
	}
}

on keydown(keycode)
{
	if(btn1.isVisible()){
		if(keycode == LEFT_ARROW){
			btn1.x -= 2;
			monster.animation = "m_run_l";			
			left_key_down = true;
		}
		if(keycode == RIGHT_ARROW){
			btn1.x += 2;
			monster.animation = "m_run_d";
			right_key_down = true;
		}
		if(keycode == UP_ARROW)
			btn1.y -= 2;
		
		if(keycode == DOWN_ARROW)
			btn1.y += 2;		
	}
	if(ca1.isVisible()){
		if(keycode == LEFT_ARROW)
			ca1.x -= 2;
		
		if(keycode == RIGHT_ARROW)
			ca1.x += 2;
		
		if(keycode == UP_ARROW)
			ca1.y -= 2;
		
		if(keycode == DOWN_ARROW)
			ca1.y += 2;
	}
	if(ca2.isVisible()){
		if(keycode == A)
			ca2.x -= 2;
		
		if(keycode == D)
			ca2.x += 2;
		
		if(keycode == W)
			ca2.y -= 2;
		
		if(keycode == S)
			ca2.y += 2;
	}
	if(keycode == NUMPAD_PLUS)
		do_rotation_plus = true;
	if(keycode == NUMPAD_MINUS)
		do_rotation_minus = true;
	if(keycode == CTRL)
		do_drag = true;
	if(keycode == SHIFT)
		increase_pol_size = true;
	if(keycode == ALT)
		decrease_pol_size = true;
}

on keyup(keycode)
{
	if(keycode == LEFT_ARROW)
	{			
		left_key_down = false;
		monster.animation = "m_idle";	
	}
	if(keycode == RIGHT_ARROW)
	{			
		right_key_down = false;
		monster.animation = "m_idle";	
	}
	if(keycode == NUMPAD_PLUS)
		do_rotation_plus = false;
	if(keycode == NUMPAD_MINUS)
		do_rotation_minus = false;
	if(keycode == CTRL)
		do_drag = false;
	if(keycode == SHIFT)
		increase_pol_size = false;
	if(keycode == ALT)
		decrease_pol_size = false;
}

on btn5.click(){	
	if(do_rotation_plus)
		btn5.rotation += 5;
	if(do_rotation_minus)
		btn5.rotation -= 5;
	line1.line_width ++;
}

on btn5.drag(x, y)
{
	if(do_drag)
	{
		btn5.x = x - btn5.w/2;
		btn5.y = y - btn5.h/2;
	}
}

on monster.drag(x, y)
{
	if(do_drag)
	{
		monster.x = x - monster.w/2;
		monster.y = y - monster.h/2;
	}
}

on r1.drag(x, y)
{
	if(do_drag)
	{
		r1.x = x - r1.w/2;
		r1.y = y - r1.h/2;
	}
}

on c1.drag(x, y)
{
	if(do_drag)
	{
		c1.x = x - c1.w/2;
		c1.y = y - c1.h/2;
	}
}

on p1.drag(x, y)
{
	if(do_drag)
	{
		p1.x = x - p1.w/2;
		p1.y = y - p1.h/2;
	}
}

on r1.click(){
	r1.stroke = "brown";	
	if(do_rotation_plus)
		r1.rotation += 5;
	if(do_rotation_minus)
		r1.rotation -= 5;
}

on c1.click(){
	if(do_rotation_plus)
		c1.line_width ++;
	if(do_rotation_minus)
		c1.line_width --;	
}

on p1.click(){
	if(increase_pol_size)
	{
		p1.width += 5;
		p1.height += 5;
	}else
	if(decrease_pol_size)
	{
		p1.width -= 5;
		p1.height -= 5;
	}else
	{
		if(do_rotation_plus)
			p1.rotation += 5;
		else if(do_rotation_minus)
			p1.rotation -= 5;
		else 
		for(var i = 0; i < 4; i++ ){
			if(i%2 == 0){
				px = p1.points[i].x + 5;
				py = p1.points[i].y + 5;
			}else{
				px = p1.points[i].x - 5;
				py = p1.points[i].y - 5;
			}
			p1.setPointbyIndex(i, px, py);		
		}
	}	
}

on update()
{	
	if(left_key_down)
		monster.x -= 0.2;
	if(right_key_down)
		monster.x += 0.2;	
}


