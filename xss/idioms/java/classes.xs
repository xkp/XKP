
on render_instances()
{
	compiler.log("Rendering Java instances...");
	
	for(var i in instances)
	{
		compiler.xss("instance.xss", i);
	}
}
