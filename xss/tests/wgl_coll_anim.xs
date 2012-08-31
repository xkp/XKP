property mouse_x;
property mouse_y;
property t = 0;
property cam = 0;
  
property timestamp; 
property lastTimestamp;

on init()
{
	start();
	timestamp = lastTimestamp;
}

on update(delta, elapsed){		
	
	var time = elapsed * 0.0005;
	var frameTime = ( timestamp - lastTimestamp ) * 0.001;
	
	light1.position.x = Math.sin( time * 7 ) * 3009;
	light1.position.y = Math.cos( time * 5 ) * 4000;
	light1.position.z = Math.cos( time * 3 ) * 3009;	
	
	if ( t >= model1.anim.start_kf && t <= model1.anim.end_kf ) {

		for ( var i = 0; i < model1.anim.length; ++i ) {

			model1.anim[ i ].update( frameTime );

		}

	} else if ( t > model1.anim.end_kf ) {

		for ( var j = 0; j < model1.anim.length; ++j ) {

			model1.anim[ j ].stop();

		}

		t = 0;
		start();
	}

	t += frameTime;	
	lastTimestamp = timestamp;
	
}

on keydown(keycode)
{
	if (keycode == C)
	{
		if(cam < 2)
			cam ++;
		else
			cam = 0;
	}
}

on switch_cameras()
{
	if(cam == 0)
		set_active_camera(model1.model_cam);
	else if(cam == 1)
		set_active_camera(model1.model_cam1);
	else
		set_active_camera(model1.model_cam2);
}

method start() 
{
	for ( var k = 0; k < model1.anim.length; ++k ) {

		var animation = model1.anim[k];
		var hl = animation.hierarchy.length;
		
		for ( var h = 0; h < hl; h++ ) {

			var keys = animation.data.hierarchy[ h ].keys;
			var sids = animation.data.hierarchy[ h ].sids;
			var obj = animation.hierarchy[ h ];

			if ( keys.length && sids ) {

				for ( var s = 0; s < sids.length; s++ ) {

					var sid = sids[ s ];
					var next = animation.getNextKeyWith( sid, h, 0 );

					if ( next ) next.apply( sid );

				}

				obj.matrixAutoUpdate = false;
				animation.data.hierarchy[ h ].node.updateMatrix();
				obj.matrixWorldNeedsUpdate = true;

			}

		}

		animation.play( false, 0 );		
		lastTimestamp = Date.now();
	}

}