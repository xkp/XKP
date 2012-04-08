on pre_process(obj)
{			
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on render_js_includes()
{
	out()
	{
		<script type="text/javascript" src="../js/pixastic/pixastic.core.js"></script>
		<script type="text/javascript" src="../js/pixastic/actions.js"></script>	
		<script type="text/javascript" src="../js/pixastic/pixastic-manager.js"></script>
		<script type="text/javascript" src="../js/pixastic/pixastic.js"></script>
	}
}

on render_instances()
{
	for(var inst in instances)
    {
		compiler.xss("pixastic.xss", inst);
    }
}

