
property world;

on pre_process(obj)
{
    if (obj.class_name == "physics_world")
    {
        world = obj; //td: test for dups		
    }
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on render_js_includes()
{
	out()
	{
        <script type="text/javascript" src="../js/Box2dWeb-2.1.a.3.min.js"></script> 
		<script type="text/javascript" src="../js/joint.js"></script> 
    }

    if (!application.noSpawner)
    {
	    out()
	    {
            <script type="text/javascript" src="../js/spawner.js"></script>
        }
    }
}

on render_initialization()
{
    var gx = 0;
    var gy = 0;

    if (world.gravity_x)
        gx = world.gravity_x;

    if (world.gravity_y)
        gy = world.gravity_y;

	out()
	{
        var   b2Vec2 = Box2D.Common.Math.b2Vec2
        ,  b2AABB = Box2D.Collision.b2AABB
        ,  b2Mat22 = Box2D.Common.Math.b2Mat22
        ,	b2BodyDef = Box2D.Dynamics.b2BodyDef
        ,	b2Body = Box2D.Dynamics.b2Body
        ,	b2FixtureDef = Box2D.Dynamics.b2FixtureDef
        ,	b2Fixture = Box2D.Dynamics.b2Fixture
        ,	b2World = Box2D.Dynamics.b2World
        ,	b2MassData = Box2D.Collision.Shapes.b2MassData
        ,	b2PolygonShape = Box2D.Collision.Shapes.b2PolygonShape
        ,	b2CircleShape = Box2D.Collision.Shapes.b2CircleShape
        ,	b2DebugDraw = Box2D.Dynamics.b2DebugDraw
        ,  	b2MouseJointDef =  Box2D.Dynamics.Joints.b2MouseJointDef
        , 	b2Transform = Box2D.Common.Math.b2Transform
		,	b2RevoluteJointDef = Box2D.Dynamics.Joints.b2RevoluteJointDef
		,	b2Joint = Box2D.Dynamics.Joints.b2Joint
		,	b2PrismaticJointDef = Box2D.Dynamics.Joints.b2PrismaticJointDef
		,	b2PulleyJointDef = Box2D.Dynamics.Joints.b2PulleyJointDef
		,	b2WeldJointDef = Box2D.Dynamics.Joints.b2WeldJointDef
		,	b2LineJointDef = Box2D.Dynamics.Joints.b2LineJointDef
		,	b2DistanceJointDef = Box2D.Dynamics.Joints.b2DistanceJointDef
        ;
         
        var g_world = new b2World(
            new b2Vec2(<xss:e v="gx"/>, <xss:e v="gy"/>)    //gravity
        ,  true                 //allow sleep
        );		
    }

    if (!application.noSpawner)
    {
	    out()
	    {
            var g_spawner_manager = new arcade.SpawnManager();
        }
    }

	out()
	{
        var g_step_collisions = [];

        var contactListener = new Box2D.Dynamics.b2ContactListener;
        contactListener.BeginContact = function(contact, manifold) 
        {
            function get_collider(body)
            {
                if (body.host && ("collision" in body.host.events))
                    return body.host;
                return null;
            }

            var host1 = get_collider(contact.GetFixtureA().GetBody());
            var host2 = get_collider(contact.GetFixtureB().GetBody());

            if (host1)
                g_step_collisions.push({host1: host1, host2:contact.GetFixtureB().GetBody().host, contact: contact, manifold:manifold});
                
            if (host2)
                g_step_collisions.push({host1: host2, host2:contact.GetFixtureA().GetBody().host, contact: contact, manifold:manifold});
        };
        g_world.SetContactListener(contactListener);

        var g_world_scale = <xss:e v="world.scale"/>;
        var fixDef = new b2FixtureDef;
        var bodyDef = new b2BodyDef;

        function update_body_position(host)
        {
            var me = host.physics;
            if (me.xs_updating)
                return;

            var pos = new b2Vec2((host.x + host.w/2)/g_world_scale, (host.y + host.h/2)/g_world_scale);
            var rot = new b2Mat22();
            rot.Set(host.rotation);

            var xform = new b2Transform(pos, rot);
            me.SetTransform(xform);
        }
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

        string renderer = i.type.renderer;
        if (renderer)
		    compiler.xss(renderer, i, world = world);
        else{
            out(){<xss:e v="i.id"/> = }compiler.xss("../common-js/instantiator.xss", i);
			compiler.xss("../common-js/instance.xss", i);
		}
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
    }

    if (!application.noSpawner)
    {
	    out()
	    {
            g_spawner_manager.update(g_delta);
        }
    }

	out()
	{
        //process collisions
        for(var i = 0; i < g_step_collisions.length; i++)
        {
            var info = g_step_collisions[i];
			info.host1.events.dispatch("collision", [info.host2, info.contact, info.manifold]);             
        }
        g_step_collisions = [];

        //process contacts
        function get_contact_handler(body)
        {
            if (body.host && ("contact" in body.host.events))
                return body.host;
            return null;
        }

        var cc = g_world.GetContactList();
        while(cc)
        {
            var host1 = get_contact_handler(cc.GetFixtureA().GetBody());
            var host2 = get_contact_handler(cc.GetFixtureB().GetBody());

            if (host1)
                host1.contact(host2, cc);

            if (host2)
                host2.contact(host1, cc);

            cc = cc.GetNext();
        }
        
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

            if (bb.host)
            {
                var x = pos.x*g_world_scale;
                var y = pos.y*g_world_scale;
                
                bb.host.xs_updating = true;
                bb.host.position(x - bb.host.w/2, y - bb.host.h/2);
                bb.host.set_rotation(angle*180/Math.PI);
                bb.host.xs_updating = false;
            }

            bb = bb.GetNext();
        }
    }
}