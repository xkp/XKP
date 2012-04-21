on pre_process(obj)
{
	if(obj.id == "")
		obj.output_id = compiler.genid(obj.class_name);
	
	//compiler.log("PRE_PROCESS xssY_idiom2: " + obj.output_id + " " + obj.class_name);
}

on render_types()
{
	out()
	{
		Rendering User Types on xssY.idiom (2)...
	}
	
	for(var ut in user_types)
	{
	
		ut.idiom = this;
		
		out()
		{
			USER CLAZZ: <xss:e value="ut.id"/>  IDIOM: <xss:e value="ut.idiom.id"/>
			SUPER USER CLAZZ: <xss:e value="ut.super.id"/>  IDIOM: <xss:e value="ut.super.idiom"/>
		}
		
		for(var i in ut.instances)
		{
			i.idiom = compiler.idiom_by_class(i.type.id);
			
			out()
			{
				INSTANCE: <xss:e value="i.id"/>  IDIOM: <xss:e value="i.idiom.id"/>
			}
		}
		
		for(var c in ut.children)
		{
			out()
			{
				CHILDREN: <xss:e value="c.id"/>  IDIOM: <xss:e value="c.idiom"/>
			}
		}
	}
}

on render_instances()
{
	out()
	{
		Rendering Instances on xssY_idiom (2)
	}
	
	for(var i in instances)
	{
		out()
		{
			INSTANCE: <xss:e value="i.id"/>  TYPE: <xss:e value="i.type.id"/>  IDIOM: <xss:e value="i.idiom"/>  PARENT: <xss:e value="i.parent.id"/>  CHILDRENS: <xss:e value="i.children.size"/>
		}
	}
}
