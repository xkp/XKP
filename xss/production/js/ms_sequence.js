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


//ESC: Esto se puede borrar, no?
ms.state.Handler = Class.create(
{
	load_resources: function(streamer)		{},
	flags: 			function()				{return 0;},
	channel: 		function()				{alert("Handlers of type: " + this.get_class() + " must provide a channel")},
	split: 			function(start, end)    {alert("Handlers of type: " + this.get_class() + " must provide a way to split")},
	mix: 			function(other)			{return null;},
	deleted:		function()				{},
	update:		    function(t, pt)			{},
});





ms.state.Sequence = Class.create(
{
    initialize: function(manager,target)
    {
		this.target = target;
	    this.handlers = [];
		this.running = false;
        this.time = 0;
		this.manager = manager;
		this.parent = null;
        this.just_started = false;
    },

    start: function()
    {
		if (this.running)
			return;
		
		if (!this.parent) //top level
			this.manager.add(this);

        this.running = true;
        this.time = 0;
        this.just_started = true;
		
    },

    stop: function()
    {
        this.running = false;
        this.time = 0;

		if (!this.parent)	
			this.manager.remove(this);
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
        if (this.running)
        {
            if (this.just_started)
            {
                this.just_started = false;
                if (this.on_start)
                    this.on_start();
            }
			 

            var pt = this.time;
			this.time += delta/1000.0;
            if (this.on_update)
			{	
                this.on_update(delta, this.time);   
			}
			

            var handlers = this.handlers;
			for(var i = 0; i < handlers.length; i++)
			{
			    for(var j = 0; i < handlers.length; j++)
				{
				    if(handlers[j].time)
					{
						if(handlers[j].time==this.time)
							handlers[j].update(this.time, pt); //ESC: para que era esto de nuevo?
					}
				} 	
				
                handlers[i].update(this.time, pt); 
			}
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
            return; //fail

			
         for(var i = 1; i < this.keys.length; i++)
         {
            var k1 = this.keys[i - 1];
            var k2 = this.keys[i];

            if (k1.t <= t && k2.t > t)
            {
                var tt = (t - k1.t)/(k2.t - k1.t)
                var value = this.interp(k1.value, k2.value, tt);

                this.assign(value);
                break;
            }
         }
		
    },
});

ms.state.Caller = Class.create(
{
    initialize: function(fn, time) //cuando las propiedades son importantes (como el t aca), es bueno ponerlas en el constructor
    {
        this.time = time; 
        this.fn = fn;
    },

	update: function(t, pt) //ESC: el update trae los dos parametros, el interpolator y eso 
    {	
        if(this.time)
		{
		    if(this.time>=this.pt_at&&this.time<=this.t_at) //que son los t_at? no se supone que sean los parametros (t y pt)? 
			    this.fn();
		}   //preg si algo ademas del at genera un Caller
            //ESC: Si, por ahora son los unicos

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

	channel: function()				
    {
        return null;
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
