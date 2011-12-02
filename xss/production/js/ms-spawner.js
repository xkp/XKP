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
        for(var i =0; i < this.spawners.length; i++)
        {
            if (this.spawners[i] == spawner)
            {
                this.spawners.splice(i, 1);
                return;
            }
        }
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
                    spawner.spawn();
                }
            }
        }
    },
});

ms.arcade.Spawner = Class.create(
{
	initialize: function(manager, x, y, frequency, velocity)
	{
		this.x          = x;
        this.y          = y;
        this.manager    = manager;
        this.creator    = null;
        this.active     = false;
        this.waiting    = 0;
        this.frequency  = frequency? frequency : 1.0;
        this.next       = 0;
        this.rotation   = 0;
        this.velocity   = velocity? velocity : 0;
	},	

	set_rotation: function(value)
    {
        this.rotation = value * Math.PI/180;        
    },

	get_rotation: function()
	{
		return this.rotation * 180/Math.PI;
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

        var x = this.x;
        var y = this.y;
        var r = this.get_rotation();
        if (this.parent)
        {
            x += this.parent.x;
            y += this.parent.y;
            r += this.parent.get_rotation();
        }

        result.position(x, y);

        if (!result.physics)
            result.ev_resized(result);
        
        var vx = this.velocity*Math.sin(r);
        var vy = this.velocity*Math.cos(r);

        var vel = new Box2D.Common.Math.b2Vec2(vx, vy);
        result.physics.SetLinearVelocity(vel);

        return result;
    },
});