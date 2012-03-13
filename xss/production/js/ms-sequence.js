"ms.state".namespace();

ms.state.Manager = Class.create(
{	
    initialize: function()
    {
		this.sequences = [];
    },

	add: function(sequence)
	{
		for(var i = 0; i < this.sequences.length; i++)
		{
			if (sequence == this.sequences[i])
				return; //fail safe
		}	

		this.sequences.push(sequence);
	},
	
	remove: function(sequence)
	{
		for(var i = 0; i < this.sequences.length; i++)
		{
			if (sequence == this.sequences[i])
			{
				this.sequences.splice(i, 1);
				return;
			}
		}	
	},
	
	update: function(t)
	{
		for(var i = 0; i < this.sequences.length; i++)
		{
			this.sequences[i].update(t);
		}
	},	
});

ms.state.SequenceUpdateEvent = Class.create(
{
initialize: function(t,d)
    {
		this.time = t;
		this.delta = d;
	}

});

ms.state.Sequence = Class.create(
{
    initialize: function(manager,target)
    {
		this.loop = false;
		this.target = target;
	    this.handlers = [];
		this.running = false;
        this.time = 0;
		this.manager = manager;
		this.parent = null;
        this.just_started = false;
		this.events = new ms.event.EventHolder();
    },

    start: function()
    {
		if (this.running)
			return;
		
		if (!this.parent) //top level
			this.manager.add(this);

        this.running = true;
        this.time = 0;
        //this.just_started = true;
		this.events.dispatch("start", null);
		
    },
	
	set_loop: function(value)
	{
	    this.loop = value;
	},

    stop: function()
    {
        this.running = false;
        this.time = 0;

		if (!this.parent)	
			this.manager.remove(this);
		this.events.dispatch("stop", null);
    },
	addHandler: function(handler)
	{
		for(var i = 0; i < this.handlers.length; i++)
		{
			if (handler == this.handlers[i])
				return; //fail safe
		}	

		this.handlers.push(handler);
	},

    update: function(delta)
    {
        if (!this.running)
				return;
            
        var pt = this.time;
		this.time += delta/1000.0;

		this.events.dispatch("update", new ms.state.SequenceUpdateEvent(this.time, delta));
            			
		var finished = true;
        var handlers = this.handlers;

		for(var i = 0; i < handlers.length; i++)
		{
			var hf = handlers[i].update(this.time, pt);
			finished = finished && hf;			  
		}

		if (finished)
		{
			if (this.loop)
				this.time = 0;
			else
				this.stop();
		}
        
    },

	
});

//Handlers
ms.state.Interpolator = Class.create(
{
    initialize: function( interp_fn, assign_fn, keys)
    {	
        this.interp     = interp_fn;
        this.assign     = assign_fn;
        this.keys       = keys;
    },


	update: function(t)				
    {	
        if (!this.keys || this.keys.length < 2)
            return true; //fail

			
         for(var i = 1; i < this.keys.length; i++)
         {
            var k1 = this.keys[i - 1];
            var k2 = this.keys[i];

            if (k1.t <= t && k2.t > t)
            {
                var tt = (t - k1.t)/(k2.t - k1.t)
                var value = this.interp(k1.value, k2.value, tt);

                this.assign(value);
                 return false;
            }
         }
		 return true;
    },
});

ms.state.Caller = Class.create(
{
    initialize: function(fn, time) 
    {
        this.time = time; 
        this.fn = fn;
    },

	update: function(t, pt) 
    {	
        if(this.time)
		{
			if (pt > this.time_)
				return true;
			if (t < this.time_)
				return false;
			this.fn();        
			return true;
		    
		}
    },
});

ms.state.Every = Class.create(
{
    initialize: function(freq, fn)
    {
        this.fn = fn;
        this.freq = freq;
        this.wait = freq;
    },

	update: function(t, pt)				
    {
        this.wait -= (t - pt);
        if (this.wait < 0)
        {
            this.wait = this.freq;
            this.fn();
        }
    },
});

ms.state.SequenceHandler = Class.create(
{
    initialize: function(seq)
    {
        this.seq = seq;
    },

	update: function(t, pt)				
    {
        var delta = t - pt;
        this.seq.update(delta);
    },
});
