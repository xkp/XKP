on pre_process(obj)
{
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on render_js_includes()
{
	out()
	{		
		<script type="text/javascript" src="../js/ms-ui.js"></script>
	}
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
            var streamer = new ms.streamer.Streamer();							
			var ui = new ms.ui.Manager(client, streamer);
		
			var g_ui = ui;
			var g_ui_root = ui.root;
		}
	}
}

on render_types()
{
	compiler.log("Rendering UI Types...");

    for(var ut in user_types)
    {
        var full_path = compiler.full_path("component.xss");
		compiler.xss("../common-js/class.xss", ut, renderer = full_path, context = ut);
    }
}

on render_ui_instances()
{
	compiler.log("Rendering UI...");
	
	out(){	
	drawingCanvas.onmousemove = function(ev)
	{
		ui.mousemove(ev.clientX, ev.clientY);                  
	};
	drawingCanvas.onmousedown = function(ev)
	{
		ui.mousedown(ev.clientX, ev.clientY);                  
	};
	drawingCanvas.onmouseup = function(ev)
	{
		ui.mouseup(ev.clientX, ev.clientY);                  
	};	
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