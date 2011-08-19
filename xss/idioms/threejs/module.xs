
on render_instances()
{
	for(var i in instances)
    {
        compiler.xss("../common-js/instance.xss", i);
    }
}
