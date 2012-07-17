on update(){			
	head_model.rotation.y += 0.3;
	if(host.needs_update)
	{
		eb.enabled = host.bleachenabled;
		ec.enabled = host.colorenabled;
		ec.pow_rgb = three_utils.vector3(host.pcred, host.pcgreen, host.pcblue);
		ec.mul_rgb = three_utils.vector3(host.mcred, host.mcgreen, host.mcblue);
		host.needs_update = false;
	}
}

	
