
class image_plane : plane
{	
	method set_image(value)
	{		
		texture = value;
	}	

	on mousein(x, y)
	{
        unselected.stop();
        selected.start();
	}

	on mouseout()
	{
        unselected.start();
        selected.stop();
	}

    on click()
    {
        dispatch application.image_selected(this);
    }
}

class image_group : transform
{
	property destination : object;
	
	property images : array <string>	
	{
		for(int i = 0; i < value.length; i++)
        {
			planes_[i].set_image(value[i]);
		}
	}

    property destination : array
    {
		for(int i = 0; i < value.length; i++)
        {
			planes_[i].destination = value[i];
            planes_[i].move.start();
		}
    }

    array<anim_plane> planes_ = [img1, img2, img3, img4, img5, img6, img7, img8]; 
}

class social_button : img
{
    //keep initialization value
    float ox;
    float oy;
 
    on mousein()
    {
        ox = x;
        oy = y;
        shrink.stop();
        grow.start();
    }

    on mouseout()
    {
        grow.stop();
        shrink.start();
    }
}