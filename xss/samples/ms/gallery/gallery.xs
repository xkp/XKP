property page_count = 3;
property page_index = 0;
property animations = [anim1, anim2, anim3];
property fades		= [fade1, fade2, fade3];

on init()
{
	//hook up the animations
	anim1.target = [ln1, ln2, ln3, ln4];
	anim2.target = [ln5, ln6, ln7, ln8];
	anim3.target = [ln9, ln10, ln11, ln12];

	fade1.target = img1;
	fade2.target = img2;
	fade3.target = img3;

	do_transition();
}

method apply_images(package)
{
	var   pos    = pages[page_index].image_location;	
	array images = [img1, img2, img3];
	int   curr   = 0;

	for(img i in images)
	{
		i.image = package.items[curr].resource;
		i.rect(pos[curr].x, pos[curr].y, pos[curr].w, pos[curr].h);
		curr++;
	}
}

method do_transition()
{
	int cnt  = 0;
	var page = pages[page_index];
	for(frame_anim anim in animations)
	{
		anim.dest = page.image_location[cnt];
		
		//shuffle the lines
		int   seed   = Math.floor(Math.random()*4);
		array target = [];
		for(int i = 0; i < 4; i++)
		{
			int idx = seed + i;
			if (idx > 3)
				idx -= 4;
			
			target.push(anim.target[idx]);
		}

		anim.target = target;
		
		//and lets go
		anim.start();
		fades[cnt].start();

		cnt++;
	}

	//load resources
	if (page.package)
	{
		var package = resources.get(page.package); 
		load_package.target = package;

		if (!package.loaded)
		{
			load_package.start();
			loading_bar.visible = true;
			if (!package.loading)
				package.load();
		}
		else
		{
			loading_bar.visible = false;
			load_package.start();
		}
	}
}

on next.click()
{
	if (page_index < pages.length - 1)
		page_index++;
	else
		page_index = 0;

	do_transition();
}

on prev.click()
{
	if (page_index > 0)
		page_index--;
	else
		page_index = page_count - 1;

	do_transition();
}

instance load_package
{
	property target : package;

	method percentage()
	{
		if (!target)
			return;

		float progress = target.progress();
		if (progress >= 1)
		{
			stop();
			loading_bar.visible = false;
			application.apply_images(target);
		}
		else loading_bar.percent = progress*100;
	}
}
