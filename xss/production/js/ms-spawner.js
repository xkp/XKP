"ms.arcade".namespace();

ms.arcade.SpawnManager = Class.create(
{
	initialize: function()
	{
		this.spawners = [];
	},	
	
	add: function(spawner)
	{
        this.spawners.push(spawner);
	},
	
	remove: function(spawner)
	{
        throw {unimplemented: true};
	},

    update: function(elapsed)
    {
        for(var i = 0; i < this.spawners.length; i++)
        {
            var spawner = this.spawners[i];
            if (spawner.active)
            {
                spawner.waiting += elapsed;
                if (spawner.waiting > spawner.next)
                {
                    console.log(spawner.waiting);
                    //calculate next tick
                    spawner.waiting = 0;

                    if (spawner.random_frequency != null)
                        spawner.next = spawner.frequency + (-spawner.random_frequency + 2*spawner.random_frequency*Math.random());
                    else
                        spawner.next = spawner.frequency;

                    spawner.next *= 1000;

                    if (spawner.random_rotation != null)
                    {
                        spawner.rotation = spawner.base_rotation + (-spawner.random_rotation + 2*spawner.random_rotation*Math.random());
                    }

                    //check spawnage
                    var spawned = spawner.spawn();

                    //and add it
                    g_ui_root.addComponent(spawned); //td: cross idiom
                }
            }
        }
    },
});

ms.arcade.Spawner = Class.create(
{
	initialize: function(manager, x, y, rotation, frequency, velocity)
	{
		this.x          = x;
        this.y          = y;
        this.manager    = manager;
        this.creator    = null;
        this.active     = false;
        this.waiting    = 0;
        this.frequency  = frequency? frequency : 1.0;
        this.next       = 0;
        this.rotation   = rotation? rotation : 0;
        this.velocity   = velocity? velocity : 1;
	},	

    start: function()
    {
        if (!this.active)
        {
            this.active = true;
            this.manager.add(this);
        }
    },

    stop: function()
    {
        this.active = false;
        this.manager.remove(this);
    },

    spawn: function()
    {
        var result = this.creator();
        result.position(this.x, this.y);
        
        var vx = this.velocity*Math.sin(this.rotation);
        var vy = this.velocity*Math.cos(this.rotation);

        var vel = new Box2D.Common.Math.b2Vec2(vx, vy);
        result.physics.SetLinearVelocity(vel);

        return result;
    },
});

