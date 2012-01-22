on init()
{
    anim_1.target = cylinder_1;
	fade_1.target = cylinder_1;		
}

on cylinder_1.click()
{
	anim_1.start();
	fade_1.start();	
}

