property steering_angle = 0;
property max_steer_angle = 60;
property steer_speed = 0.01;
property engine_speed = 0;
property engine_on = false;
property direction;
property mspeed;
property velocity;
property sideways_axis;
property local_point;
property gear;

on keydown(keycode)
{
	if (keycode == LEFT_ARROW){
		steering_angle = -1 * max_steer_angle;
		steer_speed = 0.01;
	}

	if (keycode == RIGHT_ARROW){
		steering_angle = max_steer_angle;
		steer_speed = 0.01;
	}
		
	if (keycode == UP_ARROW){
		gear = 1;
		start_engine();
	}

	if (keycode == DOWN_ARROW){
		gear = -1;
		start_engine();
	}
}

on keyup(keycode)
{	
	if (keycode == DOWN_ARROW || keycode == UP_ARROW)
		stop_engine();
	if (keycode == RIGHT_ARROW || keycode == LEFT_ARROW){
		steering_angle = 0;
		steer_speed = 1.0;
	}	
}

on update(){
	update_car();
}

method start_engine()
{
	engine_on = true;
	engine_speed = -1 * gear * 0.8;
} 

method stop_engine()
{
	engine_on = false;
	engine_speed = 0;
} 

method update_car()
{
	kill_orthogonal_velocity(left_wheel.physics);
	kill_orthogonal_velocity(right_wheel.physics);
	kill_orthogonal_velocity(left_rear_wheel.physics);
	kill_orthogonal_velocity(right_rear_wheel.physics);

	//Driving
	
	//get the direction in which the wheel is pointing and apply forces
	direction = right_wheel.physics.GetTransform().R.col2.Copy();
	direction.Multiply( engine_speed );
	right_wheel.physics.ApplyForce( direction , right_wheel.physics.GetPosition() );
	
	direction = left_wheel.physics.GetTransform().R.col2.Copy();
	direction.Multiply( engine_speed );
	left_wheel.physics.ApplyForce( direction , left_wheel.physics.GetPosition() );
	
	direction = right_rear_wheel.physics.GetTransform().R.col2.Copy();
	direction.Multiply( engine_speed );
	right_rear_wheel.physics.ApplyForce( direction , right_rear_wheel.physics.GetPosition() );
	
	direction = left_rear_wheel.physics.GetTransform().R.col2.Copy();
	direction.Multiply( engine_speed );
	left_rear_wheel.physics.ApplyForce( direction , left_rear_wheel.physics.GetPosition() );
		
	//Steering
	
	//max speed - current speed , should be the motor speed , so when max speed reached , speed = 0;
	mspeed = steering_angle - right_wheel_joint.joint_angle * 0.1;
	right_wheel_joint.motor_speed = mspeed * steer_speed;
	
	mspeed = steering_angle - left_wheel_joint.joint_angle * 0.1;
	left_wheel_joint.motor_speed = mspeed * steer_speed;	
}

method kill_orthogonal_velocity(b)
{
	local_point = NONE_AXIS;
	velocity = b.GetLinearVelocityFromLocalPoint(local_point);

	sideways_axis = b.GetTransform().R.col2.Copy();

	//multiply vector with a constant;
	sideways_axis.Multiply(velocity.x*sideways_axis.x + velocity.y*sideways_axis.y) ;

	b.SetLinearVelocity(sideways_axis); 
}
