class anim_plane : plane
{	
	property max_x;
	property min_x;
	property max_y;
	property min_y;	
	
	property parent_;
	property title = false;
	
	method set_image(value)
	{		
		texture = value;
	}	
	on mousein(x, y)
	{
		application.selected = anim_plane;			
		if(application.phase == 2)
		{			
			if(parent_ == application.group_zoomed)
			{	
				if(!title)
				{
					anim_plane.scale.x = 3;
					anim_plane.scale.y = 3;
					anim_plane.scale.z = 3;	
				}
			}
			else
			{
                parent_.set_individual_scale(0.8);
			}
			liquid.disturb(x, y);
		}
	}
	on mouseout()
	{
		application.selected = false;
		if(application.phase == 2)
		{
			if(parent_ == application.group_zoomed)
			{
				anim_plane.scale.x = 2;
				anim_plane.scale.y = 2;
				anim_plane.scale.z = 2;				
			}
			else 
			{
				if(application.art_planes != application.group_zoomed)
					application.art_planes.set_individual_scale(0.7);
				if(application.forms_planes != application.group_zoomed)
					application.forms_planes.set_individual_scale(0.7);
				if(application.times_planes != application.group_zoomed)
					application.times_planes.set_individual_scale(0.7);
			}
		}
	}
}

class planes_group : transform
{
	property selected = false;
	property plane_array = [];
	property images : array <string>	
	{
		var resource;
		for(int index = 0; index < images.length - 1; index++){
			resource = streamer.get_resource(images[index]).data;
			plane_array += anim_plane(resource.naturalWidth/13, resource.naturalHeight/13);
			plane_array[index].set_image(images[index]);
			plane_array[index].parent_ = this;
		}
		resource = streamer.get_resource(images[images.length - 1]).data;
		plane_array += anim_plane(resource.naturalWidth, resource.naturalHeight);
		plane_array[images.length - 1].set_image(images[images.length - 1]);
		plane_array[images.length - 1].parent_ = this;
		plane_array[images.length - 1].title = true;
	}

    property group_scale : float
    {
        set_individual_scale(value);
    }

    bool selected_ = false;
    method selected()
    {
        if (selected_)
            return;
        
        selected_ = true;
        shrink_plane.stop();
        grow_plane.start();
    }

    method unselected()
    {
        if (!selected_)
            return;
        
        selected_ = false;
        shrink_plane.start();
        grow_plane.stop();
    }

	method set_limits(min_x, max_x, min_y, max_y)
	{
		for(int index = 0; index < plane_array.length; index++)
		{
			if(index != plane_array.length - 1)
			{
				plane_array[index].min_x = min_x;
				plane_array[index].max_x = max_x;
				plane_array[index].min_y = min_y;
				plane_array[index].max_y = max_y;
				plane_array[index].plane_anim.stop();
				plane_array[index].idle_anim.stop();
				plane_array[index].quickly_anim.stop();
				plane_array[index].plane_anim.start();
				plane_array[index].idle_anim.start();
			}
			else
			{
				plane_array[index].position.x = min_x + (max_x - min_x)/2;
				plane_array[index].position.y = min_y + (min_y - max_y)/4;
			}
		}
	}
	method set_limits_quickly(min_x, max_x, min_y, max_y)
	{
		for(int index = 0; index < plane_array.length; index++)
		{
			if(index != plane_array.length - 1)
			{
				plane_array[index].min_x = min_x;
				plane_array[index].max_x = max_x;
				plane_array[index].min_y = min_y;
				plane_array[index].max_y = max_y;
				plane_array[index].plane_anim.stop();
				plane_array[index].idle_anim.stop();
				plane_array[index].quickly_anim.stop();
				plane_array[index].quickly_anim.start();
			}
			else
			{
				plane_array[index].position.x = min_x + (max_x - min_x)/2;
				plane_array[index].position.y = min_y + (min_y - max_y)/4;
			}
		}
	}
	method set_individual_scale(value)
	{
		for(var pl in plane_array)
		{
			pl.scale.x = value;
			pl.scale.y = value;
			pl.scale.z = value;
		}
	}
	method find(item)
	{
		for(int index = 0; index < plane_array.length; index++)
		{
			if(plane_array[index] == item)
				return index;
		}
		return -1;
	}
    method set_selected(bool value)
    {
        if (value)
        {
            grow.start();
            shrink.stop();
        }
        else
        {
            grow.stop();
            shrink.start();
        }
    }
}

class social_button : img
{
    //keep initialization value
    float ox;
    float oy;
    float owidth;
    float oheight;

    on init()
    {
        ox = x;
        oy = y;
        owidth = width;
        oheight = height;
    }
}