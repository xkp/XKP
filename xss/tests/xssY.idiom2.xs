on render_types()
{
	out()
	{
		Rendering User Types on xssY.idiom (2)...
	}
	
	for(var ut in user_types)
	{
	
		//ut.idiom = this;
		
		out()
		{
			USER CLAZZ: <xss:e value="ut.id"/>  IDIOM: <xss:e value="ut.idiom.id"/>
			SUPER USER CLAZZ: <xss:e value="ut.super.id"/>  IDIOM: <xss:e value="ut.super.idiom"/>
		}
		
		for(var i in ut.instances)
		{
			//i.idiom = compiler.idiom_by_class(i.type.id);
			
			i.idiom.render_instance(i);
		}
		
		for(var c in ut.children)
		{
			/*
			out()
			{
				CHILDREN: <xss:e value="c.id"/>  IDIOM: <xss:e value="c.idiom"/>
			}
			*/
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
		render_instance(i);
	}
}

method render_instance(inst)
{
	out()
	{
		==[IDIOM 2]=====================
		INSTANCE: <xss:e value="inst.id"/>  
		TYPE: <xss:e value="inst.type.id"/>  
		PARENT: <xss:e value="inst.parent.id"/>  
		IDIOM: <xss:e value="inst.idiom.id"/>  
	}
}
