on click()
{	
	img1.f1.rect(1, 10, 60, 60);
}

on keyup(keycode)
{	
	if(keycode == NUMPAD_PLUS)
	{
		image1.bfres.brightness += 10;
		image1.bfres.contrast += 0.1;
	}
	if(keycode == NUMPAD_MINUS)
	{
		image1.bfres.brightness -= 10;
		image1.bfres.contrast -= 0.1;
	}
}
on plane1.mousein()
{
	plane1.bf2.brightness = 20;
	plane1.bf2.contrast = 0.2;
}
on plane1.mouseout()
{
	plane1.bf2.brightness = 80;
	plane1.bf2.contrast = 0.8;
}
on sphere1.mousein()
{
	sphere1.rotation.x ++;
	sphere1.rotation.y ++;
	sphere1.rotation.z ++;
	sphere_material.bf0.brightness = 20;
	sphere_material.bf0.contrast = 0.2;
}
on sphere1.mouseout()
{
	sphere_material.bf0.brightness = 80;
	sphere_material.bf0.contrast = 0.8;
}
on sphere2.mousein()
{
	sphere2.rotation.x ++;
	sphere2.rotation.y ++;
	sphere2.rotation.z ++;
	sphere_material.bf0.brightness = 20;
	sphere_material.bf0.contrast = 0.2;
}
on sphere2.mouseout()
{
	sphere_material.bf0.brightness = 80;
	sphere_material.bf0.contrast = 0.8;
}