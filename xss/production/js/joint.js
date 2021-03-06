var joint = {};

joint.DistanceJoint = Class.extend(
{
	init: function(body_a , body_b, anchor_a, anchor_b)
	{
		this.joint_def = new b2DistanceJointDef();
		this.joint_def.Initialize(body_a.physics , body_b.physics, body_a.physics.GetWorldCenter(), body_b.physics.GetWorldCenter());	
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
});

joint.RevoluteJoint = Class.extend(
{
	init: function(body_a , body_b)
	{
		this.joint_def = new b2RevoluteJointDef();
		this.joint_def.Initialize(body_a.physics, body_b.physics, body_b.physics.GetWorldCenter());	
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
	set_lower_angle: function(lower_angle)
	{
		this.joint.SetLimits(lower_angle * Math.PI/180, this.joint.GetUpperLimit());
	},
	get_lower_angle: function()
	{
		return this.joint.GetLowerLimit() * 180 / Math.PI;
	},
	set_upper_angle: function(upper_angle)
	{
		this.joint.SetLimits(this.joint.GetLowerLimit(), upper_angle * Math.PI/180);
	},
	get_upper_angle: function()
	{
		return this.joint.GetUpperLimit() * 180 / Math.PI;
	},
	set_joint_angle: function(angle)
	{
		this.joint.SetJointAngle(angle * Math.PI/180);
	},
	get_joint_angle: function()
	{
		return this.joint.GetJointAngle() * 180 / Math.PI;
	},	
});

joint.PrismaticJoint = Class.extend(
{
	init: function(body_a , body_b, x, y)
	{
		this.joint_def = new b2PrismaticJointDef();
		axis = new Box2D.Common.Math.b2Vec2(x, y);
		this.joint_def.Initialize(body_a.physics , body_b.physics, body_b.physics.GetWorldCenter(), axis);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
	set_lower_limit: function(lower_limit)
	{
		this.joint.SetLimits(lower_limit, this.joint.GetUpperLimit());
	},
	set_upper_limit: function(upper_limit)
	{
		this.joint.SetLimits(this.joint.GetLowerLimit(), upper_limit)
	},
});

joint.PulleyJoint = Class.extend(
{
	init: function(body_a , body_b, ground_xa, ground_ya, ground_xb, ground_yb, ratio)
	{
		this.joint_def = new b2PulleyJointDef();
		ground_a = new Box2D.Common.Math.b2Vec2(ground_xa, ground_ya);
		ground_b = new Box2D.Common.Math.b2Vec2(ground_xb, ground_yb);
		this.joint_def.Initialize(body_a.physics , body_b.physics, ground_a, ground_b, body_a.physics.GetWorldCenter(), body_b.physics.GetWorldCenter(), ratio);	
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
});

joint.WeldJoint = Class.extend(
{
	init: function(body_a , body_b)
	{
		this.joint_def = new b2WeldJointDef();
		this.joint_def.Initialize(body_a.physics , body_b.physics, body_b.physics.GetWorldCenter());	
		this.joint = g_world.CreateJoint(this.joint_def);
	},	
});

function create_2d_vector(x, y)
{
	return new Box2D.Common.Math.b2Vec2(x, y);
}


		