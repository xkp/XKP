
class image_plane : plane
{	
    property group;
    property next_image;
    property image;

    method set_image(value)
	{		
        image = value;
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
        application.select(group);
    }

    method switch_images()
    {
        set_image(next_image);
    }
}

class image_group : transform
{
	property destination : object;
	
	property images : array <string>	
	{
		for(int i = 0; i < value.length; i++)
        {
            planes_[i].group = this;
			planes_[i].set_image(value[i]);
		}
	}

    method do_idle()
    {
        if (Math.random() > 0.5)
            return;

        var idx1 = Math.floor(Math.random()*8);
        var idx2 = Math.floor(Math.random()*8);

        if (planes_[idx1].idle.running || planes_[idx2].idle.running)
            return;

        planes_[idx1].next_image = planes_[idx2].image;
        planes_[idx2].next_image = planes_[idx1].image;
        planes_[idx1].idle.start();
        planes_[idx2].idle.start();
    }

    private array<image_plane> planes_ = [img1, img2, img3, img4, img5, img6, img7, img8, img9]; 
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