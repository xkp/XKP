property curr_pict;
property one_in = false;
property camera_in = false;

on init()
{
	idle_anim_1.target = plane_1;
   	idle_anim_2.target = plane_2;    
	idle_anim_3.target = plane_3;    
	idle_anim_4.target = plane_4;    
	idle_anim_5.target = plane_5;   
}

on mousemove(x, y)
{
	box_cont.disturb(x, y);
}

on plane_1.click()
{
        if(!one_in)
        {
            one_in = true;
            curr_pict = pict1;
            p1_in.stop();
            p1_in.start();
        }
}
on plane_2.click()
{
        if(!one_in)
        {
            one_in = true;
            curr_pict = pict2;
            p2_in.stop();
            p2_in.start();
        }
}
on plane_3.click()
{
        if(!one_in)
        {
            one_in = true;
            curr_pict = pict3;
            p3_in.stop();
            p3_in.start();
        }
}
on plane_4.click()
{
        if(!one_in)
        {
            one_in = true;
            curr_pict = pict4;
            p4_in.stop();
            p4_in.start();
        }
}
on plane_5.click()
{
        if(!one_in)
        {
            one_in = true;    
            curr_pict = pict5;
            p5_in.stop();
            p5_in.start();
        }
}

on camerawl.click()
{
    if(!one_in)
    {
        one_in = true;
        camera_in = true;
        cam_in.stop();
        cam_in.start();
    }
}

on close.click()
{
    if(camera_in)
    {
        camera_in = false;
        one_in = false;
        cam_out.stop();
        cam_out.start(); 
    }
    else
    {
        one_in = false;
        pic_out.target = curr_pict;
        pic_out.stop();
        pic_out.start();
        close_fade_in.stop();
        close_fade_in.start();
    }
}

on plane_1.mousein()
{
	plane_1.scale.x = 0.7;
	plane_1.scale.y = 0.7;
	plane_1.scale.z = 0.7;
	idle_anim_1.stop();
	plane_1.texture = "images/pictures/pict1.jpg";
}

on plane_1.mouseout()
{
	plane_1.scale.x = 0.5;
	plane_1.scale.y = 0.5;
	plane_1.scale.z = 0.5;
	idle_anim_1.start();
	plane_1.texture = "images/pictures/pict1_neg.jpg";
}

on plane_2.mousein()
{
	plane_2.scale.x = 0.7;
	plane_2.scale.y = 0.7;
	plane_2.scale.z = 0.7;
	idle_anim_2.stop();
	plane_2.texture = "images/pictures/pict2.jpg";
}

on plane_2.mouseout()
{
	plane_2.scale.x = 0.5;
	plane_2.scale.y = 0.5;
	plane_2.scale.z = 0.5;
	idle_anim_2.start();
	plane_2.texture = "images/pictures/pict2_neg.jpg";
}

on plane_3.mousein()
{
	plane_3.scale.x = 0.7;
	plane_3.scale.y = 0.7;
	plane_3.scale.z = 0.7;
	idle_anim_3.stop();
	plane_3.texture = "images/pictures/pict3.jpg";
}

on plane_3.mouseout()
{
	plane_3.scale.x = 0.5;
	plane_3.scale.y = 0.5;
	plane_3.scale.z = 0.5;
	idle_anim_3.start();
	plane_3.texture = "images/pictures/pict3_neg.jpg";
}

on plane_4.mousein()
{	
	plane_4.scale.x = 0.7;
	plane_4.scale.y = 0.7;
	plane_4.scale.z = 0.7;
	idle_anim_4.stop();
	plane_4.texture = "images/pictures/pict4.jpg";
}

on plane_4.mouseout()
{
	plane_4.scale.x = 0.5;
	plane_4.scale.y = 0.5;
	plane_4.scale.z = 0.5;
	idle_anim_4.start();
	plane_4.texture = "images/pictures/pict4_neg.jpg";
}

on plane_5.mousein()
{
	plane_5.scale.x = 0.7;
	plane_5.scale.y = 0.7;
	plane_5.scale.z = 0.7;
	idle_anim_5.stop();
	plane_5.texture = "images/pictures/pict5.jpg";
}

on plane_5.mouseout()
{
	plane_5.scale.x = 0.5;
	plane_5.scale.y = 0.5;
	plane_5.scale.z = 0.5;
	idle_anim_5.start();
	plane_5.texture = "images/pictures/pict5_neg.jpg";
}

instance idle_anim_seq
{
	method animate_all()
	{
		idle_anim_1.stop();
		idle_anim_1.start();
		idle_anim_2.stop();
		idle_anim_2.start();
		idle_anim_3.stop();
		idle_anim_3.start();
		idle_anim_4.stop();
		idle_anim_4.start();
		idle_anim_5.stop();
		idle_anim_5.start();
	}
}