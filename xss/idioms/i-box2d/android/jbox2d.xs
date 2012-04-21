
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

on render_type_initialization(clazz, bns, app)
{
	//TIPS: copy defaults files of idiom classes and render imports
	clazz = this;
	
	var android_idiom = compiler.idiom_by_id("android");
	android_idiom.initialization(clazz, bns, app);
}

on render_initialization(clazz, bns, app)
{
	//TIPS: copy defaults files of idiom classes and render imports
	clazz = this;
	
	var android_idiom = compiler.idiom_by_id("android");
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
		private android.os.Handler 	mHandler;
	
		private final double jBox2dFreq = 1 / 60.0f;
		private final Runnable mRunnableWorld = new Runnable() <xss:open_brace/>
			public void run() <xss:open_brace/>
				<xss:e value="world.output_id"/>.update();
				mHandler.postDelayed(mRunnableWorld, (long) (jBox2dFreq * 1000));
			<xss:close_brace/>
		<xss:close_brace/>;
	}
	
    var gx = 0;
    var gy = 0;

    if (world.gravity_x)
        gx = world.gravity_x;

    if (world.gravity_y)
        gy = world.gravity_y;

	string sdebug_draw = "";
	if(world.debug_draw)
	{
		sdebug_draw = ", ddView.getDebugDraw()";
		
		out(indent = 1, marker = "declarations")
		{
			private XKPDDView 			ddView;
		}
		
		out(indent = 1, marker = "layout_start")
		{
			//TODO: determine real screen dimensions (width & height)
			ddView = new XKPDDView(this, 320, 480);
			layout<xss:e value="application.output_id"/>.addView(ddView);
		}
	}
	
	out(indent = 1, marker = "layout_start")
	{
        <xss:e value="world.output_id"/> = new XKPJBox2d();
		<xss:e value="world.output_id"/>.createWorld(new Vec2(<xss:e value="gx"/>f, <xss:e value="gy"/>f)<xss:e value="sdebug_draw"/>);
	}
}

on render_types()
{
}

on render_instances(app)
{
	if(world == null)
		compiler.error("JBox2d requires a physics_world object");

	compiler.log("Rendering jBox2d instances... [" + id + "]");
	for(var i in instances)
	{
		compiler.log("jbox2d inst: " + i.id);
		if(i.class_name == "physics_world")
			continue;

        string renderer = i.type.renderer;
        if(renderer)
		{
			renderer = "../" + renderer;
		    compiler.xss(renderer, i, world = world, marker = "layout_start");
		}
		else
		{
            out() { <xss:e v="i.id"/> = }
			compiler.xss("../../java/instantiator.xss", i);
			
			compiler.xss("../../java/instance.xss", i);
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
		if(mHandler != null) mHandler.removeCallbacks(mRunnableWorld);
	}
}

on render_application_stop()
{
	out(indent = 1)
	{
		if(mHandler != null) mHandler.removeCallbacks(mRunnableWorld);
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
	
	compiler.log("[jbox2d] Copying default files...");
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

//android delegates
method start_rendering_body(it, world, vp)
{
	string host = "";
	if(vp)
		host = ", " + vp;
	
	var prop = it.get_property("shape");
	var shape = prop.render_value();
	
	out(indent = 1)
	{
		XKPPhysicBody <xss:e value="it.output_id"/> = new XKPPhysicBody(<xss:e value="world.output_id"/>.getWorld(), 
			<xss:e value="shape"/><xss:e value="host"/>);
	}
	
	bool isSensor = it.sensor;
	if (isSensor == null)
		isSensor = false;
	
	out()
	{
		<xss:e value="it.output_id"/>.setSensor(<xss:e v="isSensor"/>);
	}
}

method end_rendering_body(it, world, vp)
{
}

method get_physical_host(it, vp)
{
	var visual_parent;
	if (vp)
	{
		visual_parent = vp.output_id;
	}
	else 
		visual_parent = null;
	
	return visual_parent;
}

method create_body(it, vp)
{
	string create_params = "";
	if(it.shape == "circle")
	{
		if(vp)
			create_params = vp + ".getRadius()";
		else
			create_params = it.radius as string;
	}
	else
	if(it.shape == "rect")
	{
		if(vp)
			create_params = vp + ".getDX(), " + vp + ".getDY()";
		else
			create_params = (it.width as string) + ", " + (it.height as string);
			
		//TIPS: if parenthesis is omited, an exception error is dispatched
	}
	//TODO: polygon
	
	out(indent = 1)
	{
		<xss:e value="it.output_id"/>.createBody(<xss:e value="create_params"/>);
	}
}

method get_body_type(it)
{
	var prop = it.get_property("body_type");
	var body_type = prop.render_value();

	return body_type;
}

method render_basic_properties(it, density, friction, restitution, body_type, angular_damping, linear_damping)
{
	//output it all
	if(angular_damping)
	{
		out(indent = 1)
		{
			<xss:e value="it.output_id"/>.setAngularDamping(<xss:e v="angular_damping"/>f);
		}
	}
	
	if(linear_damping)
	{
		out(indent = 1)
		{
			<xss:e value="it.output_id"/>.setLinearDamping(<xss:e v="linear_damping"/>f);
		}
	}
	
	out(indent = 1)
	{
		<xss:e value="it.output_id"/>.setDensity(<xss:e v="density"/>f);
		<xss:e value="it.output_id"/>.setFriction(<xss:e v="friction"/>f);
		<xss:e value="it.output_id"/>.setRestitution(<xss:e v="restitution"/>f);
		<xss:e value="it.output_id"/>.setBodyType(<xss:e v="body_type"/>);
	}
}

method render_shape_definition(it, world, vp)
{
}

method render_position(it, vp)
{
	if (vp)
	{
		out(indent = 1)
		{
			<xss:e value="it.output_id"/>.setPosition(<xss:e value="vp"/>.getX1(), <xss:e value="vp"/>.getY1());
		}
	}
	else
	{
		out(indent = 1)
		{
			<xss:e value="it.output_id"/>.setPosition(<xss:e value="it.x"/>, <xss:e value="it.y"/>);
		}
	}
}
