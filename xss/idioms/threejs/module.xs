
on render_instances()
{
	for(var i in instances)
    {
        compiler.xss("inst_renderer.xss", i);
		compiler.xss("../common-js/instance.xss", i);
    }
}
