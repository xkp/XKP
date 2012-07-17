property comp_mat_array = [];

on pre_process(obj)
{		
	if(obj.class_name == "composite_material"){
			comp_mat_array += obj.id;			
	}
	if (obj.class_name == "scene")
		obj.id = "manager.scene";	
	for(var comp_mat in comp_mat_array){
		if(comp_mat == obj.material){
			var value = obj.material;			
			var value_type = compiler.type_of(value);
			obj.add_property("material_array", value, value_type);
			obj.material = "new THREE.MeshFaceMaterial()";				
		}
	}	
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
	if(obj.parent)	
		if(obj.parent.wait_for_package)
			obj.wait_for_package = obj.parent.wait_for_package;
}

on pre_process_type(type_)
{		
	if(type_.super)
	{
		if(type_.super.id == "shader_material")
		{	
			if(!type_.use_default_shader)
				render_non_default_shader(type_);
			else
				render_default_shader(type_);
		}	
		if(type_.super.id == "effect")
		{					
			render_default_shader(type_);
		}
	}	
}

on render_dependencies()
{
    var dependencies = compiler.idiom_dependencies("webgl");

    for(var dep in dependencies)
    {       
        compiler.xss("../common-js/dependency.xss", dep);
    }
}

on render_instances()
{	
	for(var inst in instances)
    {	
		if(inst.wait_for_package)
		{			
			out()
			{				
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>				
			}						
		}	
		compiler.xss("inst_renderer.xss", inst);	
		if(inst.wait_for_package)
			out(){<xss:close_brace/>);}		
    }	
}

on render_inst_elems()
{
	for(var inst in instances)
    {
		if(!inst.dont_render)
		{
			if(inst.wait_for_package)
			{
				out()
				{
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>
				}
			}
			compiler.xss("../common-js/instance.xss", inst);
			if(inst.wait_for_package)
				out(){<xss:close_brace/>);}
		}
	}
}

on render_types()
{
	compiler.log("Rendering WebGL Types...");

    for(var ut in user_types)
    {
		compiler.xss("class.xss", ut, renderer = "inst_renderer.xss");
    }
}

method render_non_default_shader(type_)
{
	for(var child_ in type_.children)
	{						
		var value;
		if(child_.texture)
			value = child_.texture;	
		else
			value = child_.value;
			
		if(!value)
			value = "undefined";
		var value_type = compiler.type_of(value);
		
		var setter = object();
		var getter = object();
		
		if(child_.load == "texture")
		{
			setter.text = "texture_shader_setter({path}, '{value}', {path}.uniforms."+child_.output_id+")";
			setter.global = true;
			getter.text = "uniforms."+child_.output_id+".texture";
		}					
		else if(child_.load == "texture_cube")
		{
			setter.text = "uniforms."+child_.output_id+".texture = '{value}'";								
			getter.text = "uniforms."+child_.output_id+".texture";
		}	
		else
		{
			setter.text = "uniforms."+child_.output_id+".value = {value}";								
			getter.text = "uniforms."+child_.output_id+".value";
		}				
						
		compiler.add_object_property(type_, child_.output_id, type = value_type, value = value, get = getter, set = setter, use_plain_value = true);					
	}
}

method render_default_shader(type_)
{
	for(var child_ in type_.children)
	{						
		var value;
		if(child_.texture)
			value = child_.texture;	
		else
			value = child_.value;
			
		if(!value)
			value = "undefined";
		var value_type = compiler.type_of(value);
		
		var setter = object();
		var getter = object();
		
		if(child_.load == "texture")
		{
			setter.text = "texture_shader_setter({path}, '{value}', {path}.uniforms['"+child_.output_id+"'])";
			setter.global = true;
			getter.text = "uniforms['"+child_.output_id+"'].texture";
		}					
		else if(child_.load == "texture_cube")
		{
			setter.text = "uniforms['"+child_.output_id+"'].texture = {value}";								
			getter.text = "uniforms['"+child_.output_id+"'].texture";
		}
		else
		{
			setter.text = "uniforms['"+child_.output_id+"'].value = {value}";								
			getter.text = "uniforms['"+child_.output_id+"'].value";
		}				
						
		compiler.add_object_property(type_, child_.id, type = value_type, value = value, get = getter, set = setter, use_plain_value = true);					
	}
}