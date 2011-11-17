property comp_mat_array = [];

on pre_process(obj)
{		
	if(obj.class_name == "composite_material"){
			comp_mat_array += obj.id;			
	}
	if (obj.class_name == "scene")
		obj.id = "scene";	
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
}

on render_instances()
{	
	for(var i in instances)
    {		
		compiler.xss("inst_renderer.xss", i);
		if(!i.dont_render)
			compiler.xss("../common-js/instance.xss", i, event_renderer = "../threejs/event.xss");
	}
}

on render_types()
{
	compiler.log("Rendering 3js Types...");

    for(var ut in user_types)
    {
		compiler.xss("class.xss", ut, renderer = "inst_renderer.xss");
    }
}