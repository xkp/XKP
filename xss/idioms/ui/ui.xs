on pre_process(obj)
{
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on compile_dependency(dep)
{
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "ui")
        return;

    var path = project.js_path;
    if (!path)
        path = "../js";

    dep.href = path + '/' + dep.href;
}

on render_initialization()
{
	out()
	{
		if(drawingCanvas){
			var client =
			{
				width: <xss:e value="application.width"/>,
				height: <xss:e value="application.height"/>,
				canvas: drawingCanvas
			};            						
			var ui_ = new ui.Manager(client, streamer);			
			var g_ui = ui_;
			var g_ui_root = ui_.root;
			var mouse_pressed = false;
		}
	}
}

on render_types()
{
	compiler.log("Rendering UI Types...");
    for(var ut in user_types)
    {		
        compiler.log(ut.id);
        var full_path = compiler.full_path("component.xss");		
		compiler.xss("../common-js/resig-class.xss", ut, renderer = full_path, context = ut);			
    }
}

on render_instances()
{
	compiler.log("Rendering UI...");
	out(){	
	drawingCanvas.onmousemove = function(ev)
	{
		ui_.mousemove(ev.clientX - canvas_position.x, ev.clientY - canvas_position.y);                  
	};
	drawingCanvas.onmousedown = function(ev)
	{
		ui_.mousedown(ev.clientX - canvas_position.x, ev.clientY - canvas_position.y);                  
	};
	drawingCanvas.onmouseup = function(ev)
	{
		ui_.mouseup(ev.clientX - canvas_position.x, ev.clientY - canvas_position.y);                  
	};		
	ui_.events.addListener("mousedown", function(x,y)
	{    
		application.events.dispatch("mousedown", [x, y]);
	});
	ui_.events.addListener("mouseup", function(x,y)
	{    
		application.events.dispatch("mouseup", [x, y]);
	});
	ui_.events.addListener("mousemove", function(x,y)
	{    
		application.events.dispatch("mousemove", [x, y]);
	});
	ui_.events.addListener("click", function(x,y)
	{    
		application.events.dispatch("click", [x, y]);
	});
	ui_.events.addListener("drag", function(x,y)
	{    
		application.events.dispatch("drag", [x, y]);
	});
	ui_.events.addListener("dragend", function(x,y)
	{    
		application.events.dispatch("dragend", [x, y]);
	});
	ui_.events.addListener("keyup", function(keycode)
	{    
		application.events.dispatch("keyup", [keycode]);
	});
	ui_.events.addListener("keydown", function(keycode)
	{    
		application.events.dispatch("keydown", [keycode]);
	});
	ui_.events.addListener("keypress", function(keycode)
	{    
		application.events.dispatch("keydown", [keycode]);
	});
	}
	
    for(int i = 0; i < instances.size; i++)
    {			
		var inst = instances[i];		
		        
        if(inst.wait_for_package)
		{			
			out()
			{
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>				
			}
			compiler.xss("component.xss", inst, false);
			for(var child in inst.children)
			{
				if(child.renderer)
					compiler.xss(child.renderer, child);			
				else
					compiler.xss("component.xss", child, false);
				i++;
			}
			out(){<xss:close_brace/>);}
		}
		else if(inst.renderer)
			compiler.xss(inst.renderer, inst);			
		else
			compiler.xss("component.xss", inst, false);
    }
}