
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
}

class image_group : transform
{
	property destination : object;
	
	property images : array <string>	
	{
        var dist      = 150; 
        var planes_   = [img1, img2, img3, img4, img5, img6, img7, img8, img9]; 
        var positions = [{x: -dist, y: -dist}, {x: -dist, y: 0}, {x: -dist, y: dist}, 
                         {x: 0,     y: -dist}, {x: 0,     y: 0}, {x: 0,     y: dist},
                         {x: dist,  y: -dist}, {x: dist,  y: 0}, {x: dist,  y: dist}]; 

		for(int i = 0; i < value.length; i++)
        {
            var pos = positions[i];

            planes_[i].position.x = pos.x;
            planes_[i].position.y = pos.y;
			planes_[i].set_image(value[i]);
		}
	}

    property destination : array
    {
        var planes_ = [img1, img2, img3, img4, img5, img6, img7, img8, img9]; 
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