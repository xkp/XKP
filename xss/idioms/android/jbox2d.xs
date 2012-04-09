
property world = null;

on pre_process(obj)
{
    if(obj.class_name == "physics_world")
    {
		if(world != null)
			compiler.error("Only one world per application is permitted!");
		
        world = obj;
    }
	
	if(obj.id == '')
	{
		obj.id = compiler.genid(obj.class_name);
	}
}

on render_initialization(clazz, bns, app)
{
	//TIPS: copy defaults files of idiom classes and render imports
	clazz = this;
	
	var android_idiom = compiler.get_idiom("android");
	android_idiom.initialization(clazz, bns, app);
	
	//TIPS: assign needed libraries
	if(!application.libraries)
		application.libraries = [];
		
	array<string> libs = [
		"jbox2d-library-2.1.2.2.jar",
		"slf4j-api-1.6.4.jar"
	];
	
	for(var lib in libs)
	{
		application.libraries += lib;
	}
	
	out(indent = 1, marker = "declarations")
	{
		private XKPJBox2d 			<xss:e value="world.output_id"/>;
		private XKPDDView 			ddView;
		private android.os.Handler 	mHandler;
	
		private final double jBox2dFreq = 1 / 60.0f;
		private final Runnable mRunnableWorld = new Runnable() <xss:open_brace/>
			public void run() <xss:open_brace/>
				<xss:e value="world.output_id"/>.update();
				mHandler.postDelayed(mRunnableWorld, (long) (jBox2dFreq * 1000));
			<xss:close_brace/>
		<xss:close_brace/>;
	}
	
	string debug_draw = "";
	if(world.debug_draw)
		debug_draw = ", ddView.getDebugDraw()";
	
    var gx = 0;
    var gy = 0;

    if (world.gravity_x)
        gx = world.gravity_x;

    if (world.gravity_y)
        gy = world.gravity_y;

	out(indent = 1, marker = "layout_start")
	{
		//TODO: determine real screen dimensions (width & height)
		ddView = new XKPDDView(this, 320, 480);
		layout<xss:e value="application.output_id"/>.addView(ddView);
		
        <xss:e value="world.output_id"/> = new XKPJBox2d();
		<xss:e value="world.output_id"/>.createWorld(new Vec2(<xss:e value="gx"/>f, <xss:e value="gy"/>f)<xss:e value="debug_draw"/>);
	}
}

on render_types()
{
}

on render_instances()
{
	if(world == null)
		compiler.error("JBox2d requires a physics_world object");

	for(var i in instances)
	{
		if(i.class_name == "physics_world")
			continue;

        string renderer = i.type.renderer;
        if(renderer)
		{
		    //compiler.xss(renderer, i, world = world, marker = "layout_start");
			
			var density = 1.0;
			if(i.density)
				density = i.density;
			else if(world)
				density = world.density;

			var friction = 0.5;
			if(i.friction)
				friction = i.friction;
			else if(world)
				friction = world.friction;

			var restitution = 0.2;
			if(i.restitution)
				restitution = i.restitution;
			else if(world)
				restitution = world.restitution;
			
			var rotation = 0.0;
			if(i.rotation)
				rotation = i.rotation;
			
			string create_shape = "";
			string create_size = "";
			if(i.shape == "circle")
			{
				create_shape = "Circle";
				create_size = ", " + i.radius + "f";
			}
			else
			if(i.shape == "rect")
			{
				create_shape = "Box";
				create_size = ", " + i.width + "f, " + i.height + "f";
			}
			else
				compiler.error("Invalid shape", object = i.id, shape = i.shape);
			
			var body_type = "";
			if(i.body_type == "static")
				body_type = "BodyType.STATIC";
			else
			if(i.body_type == "dynamic")
				body_type = "BodyType.DYNAMIC";
			else
			if(i.body_type == "kinematic")
				body_type = "BodyType.KINEMATIC";
			else
				compiler.error("Invalid body_type", object = i.id, body_type = i.body_type);
				
			//var prop = i.query_properties("body_type");
			//var body_type = compiler.compile_expression(prop); //[no]
			//var body_type = compiler.evaluate_property(i.body_type);
			//var body_type = compiler.render_expression(i.body_type, i);
			//var body_type = i.body_type.value.render_value();
			
			out(indent = 1, marker = "layout_start")
			{
				XKPPhysicBody <xss:e value="i.output_id"/> = new XKPPhysicBody(<xss:e value="world.output_id"/>.getWorld());
				
				<xss:e value="i.output_id"/>.create<xss:e value="create_shape"/>Body(<xss:e value="i.x"/>f, <xss:e value="i.y"/>f<xss:e value="create_size"/>, 
					<xss:e value="rotation"/>f, <xss:e value="body_type"/>, 
					<xss:e value="density"/>f, <xss:e value="friction"/>f, <xss:e value="restitution"/>f);
			}
        } else {
			compiler.error("[IMPLEMENT ME] idiom: jbox2d; method: render_instances()");
		}
    }
	
	out(indent = 1, marker = "layout_start")
	{
        mHandler = new android.os.Handler();
        mHandler.post(mRunnableWorld);
	}
}

on render_update()
{
}

on render_application_pause()
{
	out(indent = 1)
	{
		mHandler.removeCallbacks(mRunnableWorld);
	}
}

on render_application_stop()
{
	out(indent = 1)
	{
		mHandler.removeCallbacks(mRunnableWorld);
	}
}

on render_application_resume()
{
	out(indent = 1)
	{
		if(mHandler != null) mHandler.post(mRunnableWorld);
	}
}

on copy_default_files(app, bns, plibs)
{
	// file list for copy
	array<string> cp_files = [
		"/lib/jbox2d-library-2.1.2.2.jar",
		"/lib/slf4j-api-1.6.4.jar"
	];
	
	compiler.log("Copying default files...");
	for(string f1 in cp_files)
	{
		string srcf1 = compiler.full_path("/copy.defaults" + f1);
		string dstf1 = app + f1;

		//TRACE: log
		//compiler.log("Copying default file: " + srcf1);
		
		compiler.copy_file(src = srcf1, dst = dstf1);
	}
}

on render_idiom_scripts(main)
{

}
