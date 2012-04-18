
class image_plane : plane
{	
	property destination : array<float>;
	
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
}

class image_group : transform
{
    array<anim_plane> planes_ = [img1, img2, img3, img4, img5, img6, img7, img8]; 

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