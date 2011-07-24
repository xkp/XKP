
on render_instances()
{
    for(var iii in instances)
    {
	}

	//generate application code	
    compiler.xss("instance.xss", application);

	//and then instances
    for(var i in instances)
    {
        out()
		{
			<xss:file src="instance.xss" output="inline">
				<parameter name="it" value="i"/>
			</xss:file>
		}
    }
}