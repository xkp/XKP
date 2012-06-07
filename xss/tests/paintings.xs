property art_planes;
property forms_planes;
property times_planes;
property art_img = ["a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "art_title"];
property forms_img = ["f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "forms_title"];
property times_img = ["t1", "t2", "t3", "t4", "t5", "t6", "t7", "times_title"];
property resource;
property group_zoomed;
property zoomed;
property selected;
property phase;
property temp_index;
property temp_img;
property waiting_for_load = false;

instance init_anim
{
	method enter_if_loaded()
	{
		if(everybody_pack.all_loaded)
		{			
			init_anim.stop();
			init_anim2.stop();
			line1.setVisible(false);
			line2.setVisible(false);
			line3.setVisible(false);
			line4.setVisible(false);
			rect1.setVisible(true);			
			go_up.start();
			enter_to.setVisible(false);
			application.init_planes();
		}
        else
		{
			application.waiting_for_load = true;
		}
	}
}

method init_planes()
{
	art_planes = planes_group();
	art_planes.images = art_img;
	forms_planes = planes_group();
	forms_planes.images = forms_img;
	times_planes = planes_group();
	times_planes.images = times_img;
	art_planes.set_limits(-550, -350, 250, 0);
	forms_planes.set_limits(-200, 0, 250, 0);
	times_planes.set_limits(150, 350, 250, 0);
	phase = 1;
}

on click(x, y)
{
	if(phase == 1 && selected)
	{
		liquid.disturb(x, y);
		phase = 2;
		if(art_planes.find(selected) != -1)
			init_phase_2(art_planes, forms_planes, times_planes);
		if(forms_planes.find(selected) != -1)
			init_phase_2(forms_planes, art_planes, times_planes);
		if(times_planes.find(selected) != -1)
			init_phase_2(times_planes, art_planes, forms_planes);
	}else
	if(phase == 2)
	{
		if(selected.parent_ == group_zoomed)
		{
			if(!selected.title)
			{
				liquid.disturb(x, y);
				SoundUtils.play("sounds/water-droplet-1.mp3");
				phase = 3;
				selected.plane_anim.stop();
				selected.idle_anim.stop();
				selected.quickly_anim.stop();
				selected.show_me.stop();
				selected.show_me.start();
				zoomed = selected;
			}
		}
		else
		{	
			liquid.disturb(x, y);
			SoundUtils.play("sounds/water-droplet-1.mp3");
			if(art_planes.find(selected) != -1)
				init_phase_2(art_planes, forms_planes, times_planes);
			if(forms_planes.find(selected) != -1)
				init_phase_2(forms_planes, art_planes, times_planes);
			if(times_planes.find(selected) != -1)
				init_phase_2(times_planes, art_planes, forms_planes);
		}
	}else
	if(phase == 3)
	{		
		if(selected == zoomed)
		{
			phase = 2;
			zoomed.back_to.stop();
			zoomed.back_to.start();
			zoomed = false;
		}	
	}
}

on init()
{
	everybody_pack.load();
}

on everybody_pack.loaded()
{
	loading_seq.stop();
	idle_load.stop();
	loading.setVisible(false);
	enter_to.setVisible(true);	
	enterto_in.start();	
	
    if (waiting_for_load)
	{
		init_anim.stop();
		init_anim2.stop();
		line1.setVisible(false);
		line2.setVisible(false);
		line3.setVisible(false);
		line4.setVisible(false);
		rect1.setVisible(true);			
		go_up.start();
		enter_to.setVisible(false);
		init_planes();
	}
}

on enter_to.mousein()
{
	enter_to.width += 3;
	enter_to.height += 3;
}

on enter_to.mouseout()
{
	enter_to.width -= 3;
	enter_to.height -= 3;	
}

on enter_to.mousedown()
{
	enter_to.x += 2;
	enter_to.y += 2;
}

on enter_to.mouseup()
{
	enter_to.x -= 2;
	enter_to.y -= 2;
	init_anim.stop();
	init_anim2.stop();
	line1.setVisible(false);
	line2.setVisible(false);
	line3.setVisible(false);
	line4.setVisible(false);
	rect1.setVisible(true);
	linetop.x2 = 905;
	go_up.start();
	enter_to.setVisible(false);
	application.init_planes();	
}

method init_phase_2(group1, group2, group3)
{
	group_zoomed = group1;
	group1.set_individual_scale(2);		
	group1.set_limits_quickly(-400, 250, 250, 0);
	group2.set_individual_scale(0.7);		
	group2.set_limits(-450, -350, -100, -200);
	group3.set_individual_scale(0.7);		
	group3.set_limits(150, 250, -100, -200);
}

on mousemove(x, y)
{	
	if(phase == 1)
	{
		if(art_planes.find(selected) != -1)
		{
			liquid.disturb(x, y);
            art_planes.selected();
			//art_planes.set_individual_scale(1.5);
		}
		else 
            art_planes.unselected();
            //art_planes.set_individual_scale(1);
		
        if(forms_planes.find(selected) != -1)
		{
			liquid.disturb(x, y);
			//forms_planes.set_individual_scale(1.5);
            forms_planes.selected();
		}
		else 
            forms_planes.unselected();
            //forms_planes.set_individual_scale(1);
		
        if(times_planes.find(selected) != -1)
		{
			liquid.disturb(x, y);
			//times_planes.set_individual_scale(1.5);
            times_planes.selected();
		}
		else 
            times_planes.unselected();
            //times_planes.set_individual_scale(1);
	}
}

on btn1.click()
{		
	if(phase==3)
	{
		temp_img = zoomed;
		temp_index = group_zoomed.find(zoomed);
		if(temp_index == 0)
			temp_index = group_zoomed.plane_array.length - 1;
		group_zoomed.plane_array[temp_index - 1].plane_anim.stop();
		group_zoomed.plane_array[temp_index - 1].idle_anim.stop();
		zoomed = group_zoomed.plane_array[temp_index - 1];
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
	if(phase==3)
	{
		temp_img = zoomed;
		temp_index = group_zoomed.find(zoomed);
		if(temp_index == group_zoomed.plane_array.length - 2)
			temp_index = - 1;
		group_zoomed.plane_array[temp_index + 1].plane_anim.stop();
		group_zoomed.plane_array[temp_index + 1].idle_anim.stop();
		zoomed = group_zoomed.plane_array[temp_index + 1];
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

instance rain
{
    property probablity = 0.5;

    void drop()
    {
        if (Math.random() < probablity)
        {
            float x = 200 + Math.random()*600;
            float y = 200 + Math.random()*400;
 		    liquid.disturb(x, y);

            SoundUtils.play("sounds/water-droplet-1.mp3");
        }
    }
}