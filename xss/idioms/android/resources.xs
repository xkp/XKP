on pre_process(obj)
{
	if(obj.id == "")
		obj.id = compiler.genid(obj.class_name);
		//obj.output_id = compiler.genid(obj.class_name);
	
	//compiler.log("[resources]: " + obj.id + " - " + obj.output_id);
}

on render_instances(app)
{
	compiler.log("Copying resource files...");
	
	//TRACE: log
	//compiler.log("Begin Rendering Resources Instances...");
	var android_idiom = compiler.idiom_by_id("android");
	
	string pattern_numbers = "0123456789";
	string pattern_string  = "abcdefghijklmnopqrstuvwxyz";
	string pattern_others  = "._";
	string pattern_all     = pattern_numbers + pattern_string + pattern_others;

	string project_res_path = "resources/";
	
	//TODO: iterate through resources for make collection to render
	for(var inst in instances)
	{
		compiler.xss("../java/instance.xss", marker = "handlers", it = inst);
		
		//compiler.log(inst.id + " - " + inst.output_id);
		string parent = "resources";
		if(inst.parent.id != "")
			parent = inst.parent.id;
		
		//TIP: patch??? ;)
		if(inst.res_container == true)
		{
			string sClassName = "";
			if(inst.type)
				sClassName = inst.type.output_id;
			
			out(indent = 1, marker = "declarations")
			{
				private <xss:e value="sClassName"/> <xss:e value="inst.output_id"/>;
			}
		
			string load_main_resource = "";
			if(inst.res_main_container == true)
			{
				load_main_resource = ", true";
			}
			
			out(indent = 1)
			{
				<xss:e value="inst.output_id"/> = new <xss:e value="sClassName"/>(this, 
					mResources_<xss:e value="inst.output_id"/>_XKPName, mResources_<xss:e value="inst.output_id"/>_DroidName, 
					mResources_<xss:e value="inst.output_id"/>_Type, mResources_<xss:e value="inst.output_id"/>_Id
					<xss:e value="load_main_resource"/>);
				util.addXKPPackage(<xss:e value="inst.output_id"/>);
			}
		}
		
		android_idiom.process_event_handler(inst);
		
		if(!inst.resrc)
			continue;
		
		//TIPS: stupid stripped string... live is hard :(
		string project_path = compiler.project_path() + "/";
		string output_path = app + "/res/" + inst.droid_location + "/";
		
		string simg = inst.src;
		if(!simg)
			continue;
		
		int sz = String.size(simg);
		
		int pos = String.find_last(simg, "\\/");
		int rpos = pos + 1;
		int npos = sz - pos - 1;
		string fn_img = String.substr(simg, rpos, npos);
		
		//check resources filename and
		int fd1 = String.find_first_not(fn_img, pattern_all);
		int fd2 = String.find_first(fn_img, pattern_numbers);

		if(fd1 != -1 || fd2 == 0)
			compiler.error("Resource filename is incorrect", filename = fn_img);
		
		//then
		//string srcf = project_path + simg;
		string srcf = project_path + project_res_path + simg;
		string dstf = output_path + fn_img;
		
		//copying resource
		//TRACE: log
		//compiler.log("Copying resource file " + srcf);
		compiler.copy_file(src = srcf, dst = dstf);
		
		pos = String.find_last(fn_img, ".");
		string droidId = String.substr(fn_img, 0, pos);
		inst.droidId = String.substr(fn_img, 0, pos);
		inst.assignValue = "@" + inst.droid_location + "/" + inst.droidId;
	}
	
	//TIP: well, need to iterate through instances any more
	for(var instRes in instances)
	{
		render_resource(instRes);
	}
	
	//TRACE: log
	//compiler.log("End Rendering Resources Instances...");
}

method render_instance(it, app, clazz)
{
}

on render_types(app)
{
}

on render_type_initialization(clazz, bns, app)
{
}

on render_initialization(clazz, bns, app)
{
	clazz = this;
	
	var android_idiom = compiler.idiom_by_id("android");
	android_idiom.initialization(clazz, bns, app);
}

method render_resource(resource)
{
	if(resource.resource_idiom_processed)
		return;
	
	//compiler.log(resource.id);
	
	var xkpName 	= [];
	var droidName 	= [];
	var typeName 	= [];
	var identifier 	= [];
	
	//compiler.log(resource.children.size);
	if(resource.children.size > 0)
	{
		for(var res in resource.children)
		{
			//compiler.log(res.resrc);
			if(res.resrc == true)
			{
				xkpName 	+= res.id;
				droidName 	+= res.src;
				typeName 	+= res.droid_type;
				identifier	+= res.droid_location + "." + res.droidId;
			}
			else
			{
				render_resource(res);
			}
		}
		
		string comma = ",";
		
		//TIP: declare mResourcesXKPName constant array
		out(indent = 1, marker = "declarations")
		{
			private String [] mResources_<xss:e value="resource.output_id"/>_XKPName = <xss:open_brace/>
		}
		
		for(int i1 = 0; i1 < xkpName.size; i1++)
		{
			var item1 = xkpName[i1];
			if(i1 == xkpName.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				"<xss:e value="item1"/>"<xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
		
		//TIP: declare mResourcesDroidName constant array
		out(indent = 1, marker = "declarations")
		{
			private String [] mResources_<xss:e value="resource.output_id"/>_DroidName = <xss:open_brace/>
		}
		
		comma = ",";
		for(int i2 = 0; i2 < droidName.size; i2++)
		{
			var item2 = droidName[i2];
			if(i2 == droidName.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				"<xss:e value="item2"/>"<xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
		
		//TIP: declare mResourcesType constant array
		out(indent = 1, marker = "declarations")
		{
			private Integer [] mResources_<xss:e value="resource.output_id"/>_Type = <xss:open_brace/>
		}
		
		comma = ",";
		for(int i3 = 0; i3 < typeName.size; i3++)
		{
			var item3 = typeName[i3];
			if(i3 == typeName.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				<xss:e value="resource.type.output_id"/>.<xss:e value="item3"/><xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
		
		//TIP: declare mResourcesId constant array
		out(indent = 1, marker = "declarations")
		{
			private Integer [] mResources_<xss:e value="resource.output_id"/>_Id = <xss:open_brace/>
		}
		
		comma = ",";
		for(int i4 = 0; i4 < identifier.size; i4++)
		{
			var item4 = identifier[i4];
			if(i4 == identifier.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				R.<xss:e value="item4"/><xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
	}
	
	resource.resource_idiom_processed = true;
}

on copy_default_files(app, bns, plibs)
{
}

on render_idiom_scripts(main)
{

}
