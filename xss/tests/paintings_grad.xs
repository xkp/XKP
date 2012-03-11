property art_planes = [];
property forms_planes = [];
property times_planes = [];
property art_img = ["a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8"];
property forms_img = ["f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8"];
property times_img = ["t1", "t2", "t3", "t4", "t5", "t6", "t7"];
property resource;
property group;
property group_zoomed;
property zoomed;
property selected;
property phase = 1;
property temp_index;
property temp_img;
property image;

method init_planes()
{	
	for(int a = 0; a < 8; a++){
		resource = streamer.get_resource(art_img[a]).data;
		art_planes += anim_plane(resource.naturalWidth/13, resource.naturalHeight/13);
		art_planes[a].set_image(art_img[a]); 
		scene.addObject(art_planes[a]);	
		art_planes[a].plane_anim.start();		
	}
	art_planes += anim_plane(79, 24);
	art_planes[8].set_image("images/pictures/art.png"); 
	scene.addObject(art_planes[8]);
	for(int f = 0; f < 8; f++){
		resource = streamer.get_resource(forms_img[f]).data;
		forms_planes += anim_plane(resource.naturalWidth/10, resource.naturalHeight/10);
		forms_planes[f].set_image(forms_img[f]); 
		scene.addObject(forms_planes[f]);
		forms_planes[f].plane_anim.start();		
	}
	forms_planes += anim_plane(85, 23);
	forms_planes[8].set_image("images/pictures/forms.png"); 
	scene.addObject(forms_planes[8]);
	for(int t = 0; t < 7; t++){
		resource = streamer.get_resource(times_img[t]).data;
		times_planes += anim_plane(resource.naturalWidth/10, resource.naturalHeight/10);
		times_planes[t].set_image(times_img[t]); 
		scene.addObject(times_planes[t]);	
		times_planes[t].plane_anim.start();		
	}
	times_planes += anim_plane(98, 26);
	times_planes[7].set_image("images/pictures/times.png"); 
	scene.addObject(times_planes[7]);	
	set_min_max_group(art_planes, -550, -350, 250, 0);
	set_min_max_group(forms_planes,-200, 0, 250, 0);
	set_min_max_group(times_planes, 150, 350, 250, 0);
}

on init()
{	
	art_pack.load();
	forms_pack.load();
	times_pack.load();	
}

instance init_anim
{
	method animate_all()
	{
		application.init_planes();		
	}
}

method find_owner(element)
{	
	for(int i = 0; i < 9; i++)
	{
		if(art_planes[i] == element)
			return art_planes;
		if(forms_planes[i] == element)
			return forms_planes;
		if(i < 8)
			if(times_planes[i] == element)
				return times_planes;
	}	
}

on mousemove(x, y)
{
	if(phase == 1)
	{
		if(group)
		{
			set_list_scale(group, 1.5);	
			liquid.disturb(x, y);
		}
		if(art_planes != group)
		{
			set_list_scale(art_planes, 1);			
		}
		if(forms_planes != group)
		{
			set_list_scale(forms_planes, 1);			
		}
		if(times_planes != group)
		{
			set_list_scale(times_planes, 1);			
		}
	}	
}

on click(x, y)
{		
	if(phase == 3)
	{
		if(selected)
		{
			phase = 2;
			selected.back_to.stop();
			selected.back_to.start();
			zoomed = false;
		}
	}else
	if(phase == 2)
	{
		if(find_owner(selected) == group_zoomed)
		{
			if(get_img_index(selected, group_zoomed) != group_zoomed.length - 1)
			{
				liquid.disturb(x, y);
				phase = 3;
				selected.plane_anim.stop();
				selected.idle_anim.stop();
				selected.show_me.stop();
				selected.show_me.start();
				zoomed = selected;
			}
		}
		else
		{	
			liquid.disturb(x, y);
			if(group)
			{
				group_zoomed = group;
				phase = 2;
			}
			if(art_planes == group)
			{
				set_down_groups(forms_planes, times_planes);
			}
			else if(forms_planes == group)
			{
				set_down_groups(art_planes, times_planes);
			}
			else if(times_planes == group)
			{
				set_down_groups(art_planes, forms_planes);
			}		
			set_list_scale(group, 2);		
			set_min_max_group_quickly(group, -400, 250, 250, 0);
		}
	}else
	if(phase == 1)
	{	
		liquid.disturb(x, y);
		if(group)
		{
			group_zoomed = group;
			phase = 2;
		}
		if(art_planes == group)
		{
			set_down_groups(forms_planes, times_planes);
		}
		else if(forms_planes == group)
		{
			set_down_groups(art_planes, times_planes);
		}
		else if(times_planes == group)
		{
			set_down_groups(art_planes, forms_planes);
		}		
		set_list_scale(group, 2);		
		set_min_max_group_quickly(group, -400, 250, 250, 0);		
	}
}

on btn1.click()
{		
	if(zoomed)
	{
		temp_img = zoomed;
		temp_index = get_img_index(zoomed, group_zoomed);
		if(temp_index == 0)
			temp_index = group_zoomed.length - 1;
		group_zoomed[temp_index - 1].plane_anim.stop();
		group_zoomed[temp_index - 1].idle_anim.stop();
		zoomed = group_zoomed[temp_index - 1];
		temp_img.scale.x = 2;
		temp_img.scale.y = 2;
		temp_img.scale.z = 0;
		temp_img.position.z = 0;
		temp_img.plane_anim.stop();
		temp_img.plane_anim.start();
		temp_img.idle_anim.stop();
		temp_img.idle_anim.start();		
		zoomed.scale.x = 12;
		zoomed.scale.y = 12;
		zoomed.scale.z = 10;
		zoomed.position.x = -55;
		zoomed.position.y = 50;
		zoomed.position.z = 100;	
		zoomed.rotation.y = 0;
	}
}

on btn2.click()
{
	if(zoomed)
	{
		temp_img = zoomed;
		temp_index = get_img_index(zoomed, group_zoomed);
		if(temp_index == group_zoomed.length - 2)
			temp_index = - 1;
		group_zoomed[temp_index + 1].plane_anim.stop();
		group_zoomed[temp_index + 1].idle_anim.stop();
		zoomed = group_zoomed[temp_index + 1];
		temp_img.scale.x = 2;
		temp_img.scale.y = 2;
		temp_img.scale.z = 0;
		temp_img.position.z = 0;
		temp_img.plane_anim.stop();
		temp_img.plane_anim.start();
		temp_img.idle_anim.stop();
		temp_img.idle_anim.start();		
		zoomed.scale.x = 12;
		zoomed.scale.y = 12;
		zoomed.scale.z = 10;
		zoomed.position.x = -55;
		zoomed.position.y = 50;
		zoomed.position.z = 100;	
		zoomed.rotation.y = 0;
	}
}

method set_list_scale(list, scale)
{
	for(int j = 0; j < list.length; j++)
	{
		list[j].scale.x = scale;
		list[j].scale.y = scale;
		list[j].scale.z = scale;
	}
}

method set_scale(element, scale)
{
	element.scale.x = scale;
	element.scale.y = scale;
	element.scale.z = scale;
}

method set_min_max_group(list, min_x, max_x, min_y, max_y)
{
	for(int k = 0; k < list.length; k++)
	{
		if(k != list.length - 1)
		{
			list[k].min_x = min_x;
			list[k].max_x = max_x;
			list[k].min_y = min_y;
			list[k].max_y = max_y;
			list[k].plane_anim.stop();
			list[k].idle_anim.stop();
			list[k].plane_anim.start();
			list[k].idle_anim.start();
		}
		else
		{
			list[k].position.x = min_x + (max_x - min_x)/2;
			list[k].position.y = min_y + (min_y - max_y)/4;
		}
	}
}

method set_min_max_group_quickly(list, min_x, max_x, min_y, max_y)
{
	for(int k = 0; k < list.length; k++)
	{
		if(k != list.length - 1)
		{
			list[k].min_x = min_x;
			list[k].max_x = max_x;
			list[k].min_y = min_y;
			list[k].max_y = max_y;
			list[k].plane_anim.stop();
			list[k].idle_anim.stop();
			list[k].quickly_anim.stop();
			list[k].quickly_anim.start();			
		}
		else
		{
			list[k].position.x = min_x + (max_x - min_x)/2;
			list[k].position.y = min_y + (min_y - max_y)/4;
		}
	}	
}

method set_down_groups(group_1, group_2)
{
	set_list_scale(group_1, 0.7);
	set_list_scale(group_2, 0.7);
	set_min_max_group_quickly(group_1, -450, -350, -100, -200);		
	set_min_max_group_quickly(group_2, 150, 250, -100, -200);
}

method get_img_index(img_param, list)
{
	for(int l = 0; l < list.length; l++)
	{
		if(img_param == list[l])
			return l;
	}	
	return -1;
}