on click(x, y)
{
	idle_anim_1.target = art1;
	idle_anim_2.target = art2;
	idle_anim_3.target = art3;
	idle_anim_4.target = art4;
	idle_anim_5.target = art5;
	idle_anim_6.target = art6;
	idle_anim_7.target = art7;
	idle_anim_8.target = art8;
	idle_anim_9.target = forms1;
	idle_anim_10.target = forms2;
	idle_anim_11.target = forms3;
	idle_anim_12.target = forms4;
	idle_anim_13.target = forms5;
	idle_anim_14.target = forms6;
	idle_anim_15.target = forms7;
	idle_anim_16.target = forms8;
	idle_anim_17.target = times1;
	idle_anim_18.target = times2;
	idle_anim_19.target = times3;
	idle_anim_20.target = times4;
	idle_anim_21.target = times5;
	idle_anim_22.target = times6;
	idle_anim_23.target = times7;
	idle_anim_seq.start();
}
on init()
{	
	art_pack.load();
	forms_pack.load();
	times_pack.load();	
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
		idle_anim_6.stop();
		idle_anim_6.start();
		idle_anim_7.stop();
		idle_anim_7.start();
		idle_anim_8.stop();
		idle_anim_8.start();
		idle_anim_9.stop();
		idle_anim_9.start();
		idle_anim_10.stop();
		idle_anim_10.start();
		idle_anim_11.stop();
		idle_anim_11.start();
		idle_anim_12.stop();
		idle_anim_12.start();
		idle_anim_13.stop();
		idle_anim_13.start();
		idle_anim_14.stop();
		idle_anim_14.start();
		idle_anim_15.stop();
		idle_anim_15.start();
		idle_anim_16.stop();
		idle_anim_16.start();
		idle_anim_17.stop();
		idle_anim_17.start();
		idle_anim_18.stop();
		idle_anim_18.start();
		idle_anim_19.stop();
		idle_anim_19.start();
		idle_anim_20.stop();
		idle_anim_20.start();
		idle_anim_21.stop();
		idle_anim_21.start();
		idle_anim_22.stop();
		idle_anim_22.start();
		idle_anim_23.stop();
		idle_anim_23.start();
	}
}