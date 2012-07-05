var mcred = 1.1;
var mcgreen = 1.1;
var mcblue = 1.1;
var pcred = 1.45;
var pcgreen = 1.4;
var pcblue = 1.4;
var colorenabled = true;
var bleachenabled = true; 
var needs_update = true;
on init()
{
	color_enabled.checked = true;
	bleach_enabled.checked = true;
}
on mc_red.change()
{
	mcred = mc_red.value/100 + 1;
	needs_update = true; 
}
on mc_blue.change()
{
	mcblue = mc_blue.value/100 + 1;
	needs_update = true; 
}
on mc_green.change()
{
	mcgreen = mc_green.value/100 + 1;
	needs_update = true; 
}
on pc_red.change()
{
	pcred = pc_red.value/100 + 1;
	needs_update = true; 
}
on pc_blue.change()
{
	pcblue = pc_blue.value/100 + 1;
	needs_update = true; 
}
on pc_green.change()
{
	pcgreen = pc_green.value/100 + 1;
	needs_update = true; 
}
on color_enabled.change()
{
	colorenabled = color_enabled.checked;
	needs_update = true; 
}
on bleach_enabled.change()
{
	bleachenabled = bleach_enabled.checked;
	needs_update = true; 
}