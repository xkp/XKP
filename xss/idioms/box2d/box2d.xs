
property world;

on pre_process(obj)
{
    if (obj.class_name == "physics_world")
    {
        world = obj; //td: test for dups
    }
}

on render_initialization()
{
	out()
	{
      function init() <xss:open_brace/>
         var   b2Vec2 = Box2D.Common.Math.b2Vec2
            ,  b2AABB = Box2D.Collision.b2AABB
         	,	b2BodyDef = Box2D.Dynamics.b2BodyDef
         	,	b2Body = Box2D.Dynamics.b2Body
         	,	b2FixtureDef = Box2D.Dynamics.b2FixtureDef
         	,	b2Fixture = Box2D.Dynamics.b2Fixture
         	,	b2World = Box2D.Dynamics.b2World
         	,	b2MassData = Box2D.Collision.Shapes.b2MassData
         	,	b2PolygonShape = Box2D.Collision.Shapes.b2PolygonShape
         	,	b2CircleShape = Box2D.Collision.Shapes.b2CircleShape
         	,	b2DebugDraw = Box2D.Dynamics.b2DebugDraw
            ,  b2MouseJointDef =  Box2D.Dynamics.Joints.b2MouseJointDef
            ;
         
         var g_world = new b2World(
               new b2Vec2(0, 10)    //gravity
            ,  true                 //allow sleep
         );

         var fixDef = new b2FixtureDef;
         var bodyDef = new b2BodyDef;
	}
}

on render_types()
{
	compiler.log("BOX2D: Not Yet Rendering Types...");
}

on render_instances()
{
    if (!world)
        compiler.error("Box2d requires a physics_world object");

    for(var i in instances)
    {
        if (i.class_name == "physics_world")
          continue;

        compiler.log(i.id);
		compiler.xss("body.xss", i, world);
    }
}