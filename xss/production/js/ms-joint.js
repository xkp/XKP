"ms.joint".namespace();

ms.joint.DistanceJoint = Class.create(
{
	initialize: function(body_a , body_b, anchor_a, anchor_b)
	{
		this.joint_def = new b2DistanceJointDef();
		this.joint_def.Initialize(body_a , body_b, anchor_a, anchor_b);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	damping_ratio: function(damping_ratio)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.dampingRatio = damping_ratio;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	length: function(length)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.length = length;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	frequency: function(frequency)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.frequency = frequency;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
});

ms.joint.RevoluteJoint = Class.create(
{
	initialize: function(body_a , body_b, anchor)
	{
		this.joint_def = new b2RevoluteJointDef();
		this.joint_def.Initialize(body_a , body_b, anchor);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	enable_limit: function(enable_limit)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.enableLimit = enable_limit;
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
	enable_motor: function(enable_motor)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.enableMotor = enable_motor;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	set_lower_angle: function(lower_angle)
	{
		g_world.DestroyJoint(this.joint);		
		this.joint_def.lowerAngle = lower_angle * Math.PI/180;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	get_lower_angle: function()
	{
		return this.joint_def.lowerAngle * 180 / Math.PI;
	},
	set_upper_angle: function(upper_angle)
	{
		g_world.DestroyJoint(this.joint);		
		this.joint_def.upperAngle = upper_angle * Math.PI/180;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	get_upper_angle: function()
	{
		return this.joint_def.upperAngle * 180 / Math.PI;
	},
	set_reference_angle: function(reference_angle)
	{
		g_world.DestroyJoint(this.joint);		
		this.joint_def.referenceAngle = reference_angle * Math.PI/180;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	get_reference_angle: function()
	{
		return this.joint_def.referenceAngle * 180 / Math.PI;
	},
	motor_speed: function(motor_speed)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.motorSpeed = motor_speed;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	max_motor_torque: function(max_motor_torque)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.maxMotorTorque = max_motor_torque;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
});

ms.joint.PrismaticJoint = Class.create(
{
	initialize: function(body_a , body_b, anchor, axis)
	{
		this.joint_def = new b2PrismaticJointDef();
		this.joint_def.Initialize(body_a , body_b, anchor, axis);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	enable_limit: function(enable_limit)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.enableLimit = enable_limit;
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
	enable_motor: function(enable_motor)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.enableMotor = enable_motor;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	motor_speed: function(motor_speed)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.motorSpeed = motor_speed;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	max_motor_force: function(max_motor_force)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.maxMotorForce = max_motor_force;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	lower_translation: function(lower_translation)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.lowerTranslation = lower_translation;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	upper_translation: function(upper_translation)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.upperTranslation = upper_translation;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	set_reference_angle: function(reference_angle)
	{
		g_world.DestroyJoint(this.joint);		
		this.joint_def.referenceAngle = reference_angle * Math.PI/180;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	get_reference_angle: function()
	{
		return this.joint_def.referenceAngle * 180 / Math.PI;
	},
});

ms.joint.PulleyJoint = Class.create(
{
	initialize: function(body_a , body_b, ground_a, ground_b, anchor_a, anchor_b)
	{
		this.joint_def = new b2PulleyJointDef();
		this.joint_def.Initialize(body_a , body_b, ground_a, ground_b, anchor_a, anchor_b);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	max_length_a: function(max_length_a)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.maxLengthA = max_length_a;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	max_length_b: function(max_length_b)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.maxLengthB = max_length_b;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	length_a: function(length_a)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.lengthA = length_a;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	length_b: function(length_b)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.lengthB = length_b;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	ratio: function(ratio)
	{
		g_world.DestroyJoint(this.joint);
		this.joint_def.ratio = ratio;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
});

ms.joint.WeldJoint = Class.create(
{
	initialize: function(body_a , body_b, anchor)
	{
		this.joint_def = new b2WeldJointDef();
		this.joint_def.Initialize(body_a , body_b, anchor);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	set_reference_angle: function(reference_angle)
	{
		g_world.DestroyJoint(this.joint);		
		this.joint_def.referenceAngle = reference_angle * Math.PI/180;
		this.joint = g_world.CreateJoint(this.joint_def);
	},
	get_reference_angle: function()
	{
		return this.joint_def.referenceAngle * 180 / Math.PI;
	},
});

ms.joint.Utils = Class.create(
{
	physics_center : function (path){
		return path.physics.GetWorldCenter();
	},
	box2d_vector : function (x, y){
			return new b2Vec2(x, y);
	},
});





		