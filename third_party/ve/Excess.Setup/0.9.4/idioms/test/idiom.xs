
method render_array(var prop)
{
	out()
	{
		[Begin rendering Array] (<xss:e value="prop.children.size"/>)
	}
	for(var p in prop.children)
	{
		if(p.type.is_object)
		{
			render_object(p);
			continue;
		}
		
		out()
		{
			id: <xss:e value="p.id"/>
			type: <xss:e value="p.type.id"/>
			value: <xss:e value="p.value"/>
		}
	}
	out()
	{
		[End rendering Array]
	}
}

method render_object(var obj)
{
	out()
	{
		[Begin rendering Object] (<xss:e value="obj.children.size"/>)
	}
	for(var c in obj.children)
	{
		if(c.type.is_array)
		{
			render_array(c);
			continue;
		}
		
		out()
		{
			id: <xss:e value="c.id"/>
			type: <xss:e value="c.type.id"/>
			value: <xss:e value="c.value"/>
		}
	}
	out()
	{
		[End rendering Object]
	}
}
