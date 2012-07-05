on update(){			
	head_model.rotation.y += 0.3;
	if(host.needs_update)
	{
		effect_bleach.enabled = host.bleachenabled;
		effect_color.enabled = host.colorenabled;
		
		host.needs_update = false;
	}
}

	
