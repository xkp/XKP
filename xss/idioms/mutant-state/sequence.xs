
on render_initialization()
{
	out()
	{
		var g_sequence_manager = new ms.state.Manager();
	}
}

on render_instances()
{
	compiler.log("Rendering Sequence...");
	
	//and then instances
    for(var i in instances)
    {
		compiler.xss("sequence.xss", i, false);
    }
}