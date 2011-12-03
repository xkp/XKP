on render_instances(app)
{

}

on render_types(app)
{
	compiler.log("Begin Rendering Android Types...");

    for(var ut in user_types)
    {
		string output_filename = app + "/src/com/xkp/android/" + app + "/" + ut.output_id + ".java";
		compiler.xss("script.java.xss", output_file = output_filename, clazz = ut, appName = app, is_type = true);
	
		output_filename = app + "/res/layout/" + ut.output_id + ".xml";
		compiler.xss("layout.xml.xss", output_file = output_filename, clazz = ut, appName = app);
    }
	
	compiler.log("End Rendering Android Types...");
}
