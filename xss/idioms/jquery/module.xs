
on render_instances()
{
	compiler.log("Rendering jQuery...");
    
	//generate application code	
	//compiler.xss("../common-js/instance.xss", application);

	//and then instances
    for(var i in instances)
    {
        compiler.xss("../common-js/instance.xss", i);
    }
}