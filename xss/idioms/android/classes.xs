on render_types(path, app)
{
	compiler.log("Rendering Android Types...");

    for(var ut in user_types)
    {
		string output_filename = path + ut.output_id + ".java";
		compiler.xss("definedUserTypes.xss", output_file = output_filename, clazz = ut, appName = app);
		
		output_filename = app + "/res/layout/" + ut.output_id + ".xml";
		compiler.xss("mainLayout.xml.xss", output_file = output_filename, clazz = ut, appName = app);
    }
}
