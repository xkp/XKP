var contrast_amount = 1;
var rgb_red = 1;
var rgb_green = 1;
var rgb_blue = 1;
var emboss_height = 1;
var emboss_angle = 135;
var emboss_elevation = 30;
var anoise_amount = 25;
var anoise_density = 1;
var anoise_mono = true;
var needs_update = true; 
on contrast.change()
{
	contrast_amount = contrast.value/100;
	needs_update = true; 
}
on red.change()
{
	rgb_red = red.value/100;
	needs_update = true; 
}
on blue.change()
{
	rgb_blue = blue.value/100;
	needs_update = true; 
}
on green.change()
{
	rgb_green = green.value/100;
	needs_update = true; 
}
on height.change()
{
	emboss_height = height.value/10;
	needs_update = true; 
}
on angle.change()
{
	emboss_angle = angle.value;
	needs_update = true; 
}
on elevation.change()
{
	emboss_elevation = elevation.value;
	needs_update = true; 
}
on noise_amount.change()
{
	anoise_amount = noise_amount.value;
	needs_update = true; 
}
on noise_density.change()
{
	anoise_density = noise_density.value/100;
	needs_update = true; 
}
on mono.change()
{
	anoise_mono = mono.value;
	needs_update = true; 
}