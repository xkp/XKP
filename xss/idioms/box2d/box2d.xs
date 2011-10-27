
property world;

on pre_process(obj)
{
    if (obj.class_name == "physics_world")
    {
        world = obj; //td: test for dups
    }
}

on render_jscripts()
{
	out()
	{
        <script type="text/javascript" src="../js/Box2dWeb-2.1.a.3.min.js"></script>
    }
}

on render_initialization()
{
	out()
	{
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

         var g_world_scale = <xss:e v="world.scale"/>;
         var fixDef = new b2FixtureDef;
         var bodyDef = new b2BodyDef;
	}
}

on render_types()
{
}

on render_instances()
{
    if (!world)
        compiler.error("Box2d requires a physics_world object");

    for(var i in instances)
    {
        if (i.class_name == "physics_world")
          continue;

		compiler.xss("body.xss", i, world);
    }

    //setup debug draw
    if (world.debug_draw)
    {
        out()
        {
            g_ui.fill_background = false; //td: cross idiom
            var debugDraw = new b2DebugDraw();
            var debugDrawCanvas = document.getElementById("<xss:e v="application.canvasElement"/>");
            debugDraw.SetSprite(debugDrawCanvas.getContext("2d"));
            debugDraw.SetDrawScale(30.0);
            debugDraw.SetFillAlpha(0.5);
            debugDraw.SetLineThickness(1.0);
            debugDraw.SetFlags(b2DebugDraw.e_shapeBit | b2DebugDraw.e_jointBit);
            g_world.SetDebugDraw(debugDraw);
        }
    }
}

on render_update()
{
    out()
    {
        g_world.Step(1 / 60, 10, 10);
        g_world.DrawDebugData();
        g_world.ClearForces();

        //synch
        var bb = g_world.GetBodyList();
        while(bb)
        {
            if (!bb.IsActive())
            {
                bb = bb.GetNext();
                continue;
            }    

            var angle = bb.GetAngle();
            var pos   = bb.GetTransform().position;

            if (bb.xs_visual)
            {
                var x = pos.x*g_world_scale;
                var y = pos.y*g_world_scale;
                bb.xs_visual.position(x - bb.xs_visual.w/2, y - bb.xs_visual.h/2);
                bb.xs_visual.set_rotation(angle);
            }

            bb = bb.GetNext();
        }
    }
}