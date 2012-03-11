class anim_plane : plane
{	
	property max_x;
	property min_x;
	property max_y;
	property min_y;	
	property image;
	method set_image(value)
	{
		image = value;
		texture = value;
	}	
	method get_image()
	{
		application.image = image;
	}	
	on mousein(x, y)
	{
		application.selected = anim_plane;
		application.group = application.find_owner(anim_plane);
		
		if(application.phase == 2)
		{
			if(application.find_owner(anim_plane) == application.group_zoomed)
			{
				if(application.get_img_index(anim_plane, application.group_zoomed) != application.group_zoomed.length - 1)
				{
					anim_plane.scale.x = 3;
					anim_plane.scale.y = 3;
					anim_plane.scale.z = 3;	
				}
			}
			else
			{
				application.set_list_scale(application.group, 0.8);
			}
			liquid.disturb(x, y);
		}
	}
	on mouseout()
	{			
		application.group = false;		
		application.selected = false;
		if(application.phase == 2)
		{
			if(application.find_owner(anim_plane) == application.group_zoomed)
			{
				anim_plane.scale.x = 2;
				anim_plane.scale.y = 2;
				anim_plane.scale.z = 2;				
			}
			else 
			{
				if(application.art_planes != application.group_zoomed)
					application.set_list_scale(application.art_planes, 0.7);
				if(application.forms_planes != application.group_zoomed)
					application.set_list_scale(application.forms_planes, 0.7);
				if(application.times_planes != application.group_zoomed)
					application.set_list_scale(application.times_planes, 0.7);
			}
		}
	}	
}