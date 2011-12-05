on render_instances(app)
{

}

on render_types(app)
{
	compiler.log("Begin Rendering Android Types...");

	//compiler.log(this.id);
	//compiler.log(this.user_types.size);
    for(var ut in user_types)
    {
		// set idiom to custom clazz with instances
		ut.idiom = this;
		//compiler.log("type: " + ut.id);
		for(var it in ut.instances)
		{
			//compiler.log("instance: " + it.id);
			it.idiom = this;
		}
		for(var ch in ut.children)
		{
			//compiler.log("child: " + ch.id);
			ch.idiom = this;
		}
		
		string output_filename = app + "/src/com/xkp/android/" + app + "/" + ut.output_id + ".java";
		compiler.xss("script.java.xss", output_file = output_filename, clazz = ut, appName = app, is_type = true);
	
		output_filename = app + "/res/layout/" + ut.output_id + ".xml";
		compiler.xss("layout.xml.xss", output_file = output_filename, clazz = ut, appName = app);
    }
	
	compiler.log("End Rendering Android Types...");
}
