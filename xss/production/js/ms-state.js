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
				this.sequences.remove(i);
				return;
			}
		}	
	},
	
	update: function(t, last)
	{
		for(var i = 0; i < this.sequences.length; i++)
		{
			this.sequences[i].update(t, last);
		}
	},	
});

ms.state.State = Class.create(
{
    initialize: function(start, end, handler)
    {
		this.start 	 = start;
		this.end   	 = end;
		this.handler = handler;
    },
});

ms.state.RELATIVE    = 1;
ms.state.USE_NEAREST = 2;
ms.state.BOOLEAN 	 = 4;
ms.state.PUNCTUAL	 = 8;

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

//sort function
function sort_active_states(a, b)
{
	return a.start - b.start;
}

//unit
ms.state.Unit = Class.create(
{
    initialize: function()
    {
		this.states  = [];
		this.actions = [];
    },

	get_state: function(t)
	{
    	for(var i = 0; i < this.states.length; i++)
    	{
    		var st = this.states[i];
    		if (st.start <= t && st.end > t)
    			return st;
    	}

    	return null;
	},

	get_states: function(start, end)
	{
    	result = [];
		for(var i = 0; i < this.states.length; i++)
    	{
    		var st = this.states[i];
    		if (this.collide(st, {start:start, end:end}))
    			result.push(st);
    	}

    	return result;
	},

	initial_value: function()
	{
		if (!this.owner)
			return null;

		if (!this.owner.initial_state)
			return null;

		return this.owner.initial_state[this.channel];
	},

	value: function(t, ignore)
    {
		if (t < 0)
    		return this.initial_value();

		var last_known = null;
    	for(var i = 0; i < this.states.length; i++)
    	{
    		var st = this.states[i];
    		if (st == ignore)
    			continue;

    		if (st.start > t)
    			break;

    		last_known = st;
    	}

    	if (!last_known)
    		return this.initial_value();

    	var tt = (t - st.start)/(st.end - st.start);
    	if (tt > 1)
    		tt = 1;
    	else if (tt < 0)
    		tt = 0;

    	if (!last_known.handler.value)
    	{
    		//debug, this is an error condition
    		return this.initial_value();
    	}

    	return last_known.handler.value(tt);
    },

    update: function(t, pt)
    {
    	if (this.states.length == 0)
    		return;

    	var flags = this.states[0].handler.flags();
		if ((flags & ms.state.BOOLEAN) != 0)
		{
			this.update_boolean(t);
			return;
		}

		if ((flags & ms.state.PUNCTUAL) != 0)
		{
    	    for(var i = 0; i < this.states.length; i++)
    	    {
    		    var st = this.states[i];
                if ((st.start >= pt && st.start <= t) ||
                    (st.start <= pt && st.start >= t)) 
                {
                    st.handler.update(t, pt);
                }
            }

            return;
		}

    	var from 		= null;
    	var to 	 		= null;
    	var use_nearest = false;
    	var last   		= null;
    	for(var i = 0; i < this.states.length; i++)
    	{
    		var st 	  = this.states[i];
    		if ((flags & ms.state.RELATIVE) == 0)
    			from = i;

    		use_nearest = (flags & ms.state.USE_NEAREST) != 0;
    		if (st.start > t)
    		{
        		if (use_nearest)
        		{
        			if (last)
        				last.update(0);
        			else
        				st.handler.update(-1);
        			return;
        		}
           		else
           		{
            		if (from) from--;
           			break;
           		}
    		}

    		to = i;
    		last = st.handler;

    		if (st.end < t)
    			continue;

    		break;
    	}

    	var original = (from == null) || (to == null);

    	if (original)
    	{
    		var accesor = "set_" + this.channel;
    		if (this.owner && accesor in this.owner)
    		{
    			this.owner[accesor](this.initial_value());
    		}

			from = 0;
    	}

    	if (to == null)
    		return;

		for(var i = from; i <= to; i++)
		{
			var st = this.states[i];

			var tt = 1;
			if (i == to)
			{
				if (st.end > st.start)
					tt = (t - st.start)/(st.end - st.start);
				else
					tt = 0;
			}

			if (tt > 1 && !use_nearest)
				tt = 1;

			st.handler.update(t, pt, this.owner); //td: !!! use tt
		}
    },

	update_boolean: function(t)
	{
    	for(var i = 0; i < this.states.length; i++)
    	{
    		var st = this.states[i];
			var tt = (t - st.start)/(st.end - st.start);

			st.handler.update(tt, this.owner);
    	}
	},

    add: function(action, dont_build)
    {
    	this.actions.push(action);
    	this.actions.sort(sort_active_states);

    	if(dont_build)
    		return;

    	this.build(action.start, action.end);
    },

    removeId: function(id, dont_build)
    {
		if (!id)
			return null;

		var start =  10000000;
		var end	  = -10000000;
		for(var i = this.actions.length - 1; i >= 0 ; i--)
    	{
			var action = this.actions[i];
    		if (action.id == id)
    		{
    			if (start > action.start)
    				start = action.start;

    			if (end < action.end)
    				end = action.end;

    			this.actions.splice(i, 1);
    		}
    	}

		var result = {start: start, end: end};
		if(dont_build || end < 0)
    		return result;

    	this.build(start, end);
		return result;
    },

    remove: function(action, dont_build)
    {
    	for(var i = 0; i < this.actions.length; i++)
    	{
    		if (this.actions[i] == action)
    		{
    			this.actions.splice(i, 1);
    			break;
    		}
    	}

    	if(dont_build)
    		return;

    	this.build(action.start, action.end);
    },

    replace: function(action)
    {
    	for(var i = 0; i < this.actions.length; i++)
    	{
    		if (this.actions[i].id == action.id)
    		{
    			this.actions.splice(i, 1);
    			this.actions.splice(i, 0, action);
    			break;
    		}
    	}

    	this.build(action.start, action.end);
    },

    load: function(streamer)
    {
    	for(var i = 0; i < this.actions.length; i++)
    	{
    		var st = this.actions[i];
    		st.handler.load_resources(streamer);
    	}
    },

    collide: function(st1, st2)
    {
    	if (st1.start >= st2.end)
    		return false;

    	if (st1.end <= st2.start)
    		return false;

    	return true;
    },

    build: function(start, end)
    {
    	if (this.actions.length == 0)
    	{
    		this.states = [];
    		return;
    	}

    	var new_states = [];
    	if (!start)
    		start = 0;

    	if (!end)
    	{
    		end = 0;
    		for(var i = 0; i < this.states.length; i++)
    		{
    			var state = this.states[i];
    			if (end < state.end) end = state.end;
    		}

    		for(var i = 0; i < this.actions.length; i++)
    		{
    			var action = this.actions[i];
    			if (end < action.end) end = action.end;
    		}
    	}

    	//and fill the new states with the original actions
    	var outside = [];
    	for(var i = 0; i < this.actions.length; i++)
    	{
    		var act 	   = this.actions[i];
    		var is_outside = false;
    		if (act.start != act.end)
    			is_outside = (act.start >= end || act.end <= start);
    		else
    			is_outside = (act.start > end || act.start < start);

    		if (is_outside)
    		{
    			outside.push(act);
    			continue;
    		}

    		new_states.push(new ms.state.State(act.start, act.end, act.handler));

    		start = Math.min(act.start, start);
    		end   = Math.max(act.end,  end);
    	}

    	//find all interconnected actions surrounding the update area
    	//just to be safe
    	var chain_found = false;
    	while(!chain_found)
    	{
    		chain_found = true;
    		for(var i = outside.length - 1; i >= 0; i--)
    		{
    			var oo = outside[i];
    			for(var j = 0; j < new_states.length; j++)
    			{
    				if (this.collide(oo, new_states[j]))
    				{
    					new_states.push(oo);
    					outside.slice(i, 1);
    					chain_found = false;

    					start = Math.min(oo.start, start);
    					end   = Math.max(oo.end,  end);
    					break;
    				}
    			}

    			if (!chain_found)
    				break;
    		}
    	}

    	//check if we can keep some cached states
    	for(var i = this.states.length - 1; i >= 0; i--)
    	{
    		var st = this.states[i];
    		if (st.start > end || st.end < start)
    			continue;

    		st.handler.deleted();
    		this.states.splice(i, 1);
    	}

    	var finished = false;
    	var safe_idx = 0;
    	while(!finished)
    	{
    		finished = true;
    		for(var i = safe_idx; i < new_states.length - 1; i++)
    		{
    			var st1 = new_states[i];
    			for(var j = i + 1; j < new_states.length; j++)
    			{
    				var st2 = new_states[j];
    				if (this.collide(st1, st2))
    				{
    					var result = this.resolve(st1, st2);
    					new_states.splice(j, 1);
    					new_states.splice(i, 1);

    					for(var k = 0; k < result.length; k++)
    					{
    						new_states.push(result[k]);
    					}

    					new_states.sort(sort_active_states);

    					finished = false;
    					break;
    				}
    			}

    			if (!finished)
    				break;

    			safe_idx = i + 1;
    		}
    	}

    	//composite, there are some edge cases where
    	//some of the new states added will collide with existing states
    	//the existing ones will always take precedence. Note that these new states
    	//are always outside the update area
    	for(var i = 0; i < new_states.length; i++)
    	{
    		var ns    = new_states[i];
    		var found = false;
    		for(var j = 0; j < this.states.length; j++)
    		{
    			if (this.collide(ns, this.states[j]))
    			{
    				found = true;
    				break;
    			}
    		}

    		if (!found)
    			this.states.push(ns);
    	}

    	this.states.sort(sort_active_states);
    },

    resolve: function(st1, st2)
    {
    	var shared_start = Math.max(st1.start, st2.start);
    	var shared_end   = Math.min(st1.end, st2.end);
    	var result       = [];

    	//check bottom case
    	if (st1.start < shared_start)
    	{
    		var handler  = st1.handler.split(0, (shared_start - st1.start)/(st1.end - st1.start));
    		var only_st1 = new ms.state.State(st1.start, shared_start, handler);
    		result.push(only_st1);
    	}
    	else if (st2.start < shared_start)
    	{
    		var handler  = st2.handler.split(0, (shared_start - st2.start)/(st2.end - st2.start));
    		var only_st2 = new ms.state.State(st2.start, shared_start, handler);
    		result.push(only_st2);
    	}

    	//shared
		var handler1 = st1.handler.split((shared_start - st1.start)/(st1.end - st1.start), (shared_end - st1.start)/(st1.end - st1.start));
		var handler2 = st2.handler.split((shared_start - st2.start)/(st2.end - st2.start), (shared_end - st2.start)/(st2.end - st2.start));

		var shared = new ms.state.State(shared_start, shared_end);
    	shared.handler = handler1.mix(handler2);
    	if (!shared.handler)
    	{
    		shared.handler = handler2.mix(handler1);
    		if (!shared.handler)
    		{
    			throw { id: ERROR_ACTION_CONFLICT, action1_id: st1.id, action2_id: st2.id };
    		}
    	}

    	result.push(shared);

    	//top case
    	if (st1.end > shared_end)
    	{
    		var handler  = st1.handler.split((shared_end - st1.start)/(st1.end - st1.start), 1);
    		var only_st1 = new ms.state.State(shared_end, st1.end, handler);
    		result.push(only_st1);
    	}
    	else if (st2.end > shared_end)
    	{
    		var handler  = st2.handler.split((shared_end - st2.start)/(st2.end - st2.start), 1);
    		var only_st2 = new ms.state.State(shared_end, st2.end, st2.handler);
    		result.push(only_st2);
    	}

    	return result;
    },

    init: function()
    {
		//grab the initial value, will be used to fill in voids
		for(var i = 0; i < this.states.length; i++)
    	{
    		var st = this.states[i];
    		if ("init" in st.handler)
    		{
    			st.handler.init(this, i);
    		}
    	}
    }
});

ms.state.Sequence = Class.create(
{
    initialize: function(manager)
    {
	    this.units = [];
        this.running = false;
        this.time = 0;
		this.manager = manager;
		this.parent = null;
    },

    start: function()
    {
		if (this.running)
			return;
		
		if (!this.parent) //top level
			this.manager.add(this);

        this.running = true;
        this.time = 0;
        
        if (this.on_start)
            this.on_start();
    },

    stop: function()
    {
        this.running = false;
        this.time = 0;

		if (!this.parent)	
			this.manager.remove(this);
    },

    update: function(delta)
    {
        if (this.running)
        {
            var pt = this.time;
            this.time += delta/1000.0;
            this.seek(this.time, pt);
        }
    },

	addAction: function(owner, channel, action)
	{
		var channel = this.get_channel(owner, channel);
		channel.add(action);
	},

	addActions: function(action_id, target, actions)
	{
		var affected_units = [];
		for(var i = 0; i < this.units.length; i++)
		{
			var unit = this.units[i];
			if (unit.owner == target)
			{
				var extents = unit.removeId(action_id, true);
				this.add_extents(affected_units, unit, extents);
			}
		}

		for(var i = 0; i < actions.length; i++)
		{
			var action = actions[i];
			var unit   = this.get_channel(target, action.handler.channel());
			var au 	   = this.add_extents(affected_units, unit, {start: action.start, end: action.end});

			au.actions.push(action);
		}

		var min_time =  10000000000;
		var max_time = -10000000000;
		for(var i = 0; i < affected_units.length; i++)
		{
			var au = affected_units[i];
			if (min_time > au.start)
				min_time = au.start;

			if (max_time < au.end)
				max_time = au.end;

			var unit = au.unit;
			for(var j = 0; j < au.actions.length; j++)
			{
				var action = au.actions[j];
				unit.add(action, true);
			}

			unit.build(); //au.start, au.end
			unit.init();
		}
	},

	get: function(owner, channel, time)
	{
		time = time || 0;

		var channel = this.get_channel(owner, channel, true);
		if (!channel)
			 return undefined;

		return channel.value(time);
	},
	
	get_handler: function(owner, channel, time)
	{
		time = time || 0;

		var channel_unit = this.get_channel(owner, channel, true);
		if (!channel_unit)
			 return null;
		
		var state = channel_unit.get_state(time);
		if (!state)
			 return null;

		return state.handler;
	},
	
	get_handlers: function(owner, channel, start, end)
	{
		var channel_unit = this.get_channel(owner, channel, true);
		if (!channel_unit)
			 return [];
		
		var states = channel_unit.get_states(start, end);
		var result = [];
		
		for(var i = 0; i < states.length; i++)
		{
			result.push(states[i].handler);
		}

		return result;
	},

	removeId: function(id)
	{
		for(var i = 0; i < this.units.length; i++)
		{
			var unit = this.units[i];
			var res  = unit.removeId(id, true);

			if (res.end > 0)
			{
				unit.build();
				unit.init();
			}
		}
	},

	removeAction: function(owner, channel, action)
	{
		var channel = this.get_channel(owner, channel);
		channel.remove(action);
	},

	build: function()
	{
		for(var i = 0; i < this.units.length; i++)
		{
			var unit = this.units[i];
			unit.build();
			unit.init();
		}
	},

	move: function(from, to)
	{
		this.seek(to, from);
	},

	seek: function(time, previous_time)
	{
		for(var i = 0; i < this.units.length; i++)
		{
			var unit = this.units[i];
			unit.update(time, previous_time);
		}
	},

	load: function(streamer)
	{
		for(var i = 0; i < this.units.length; i++)
		{
			this.units[i].load(streamer);
		}
	},

	//detail
	get_channel: function(owner, channel, dont_create)
	{
        if (owner != null)
        {
		    for(var i = 0; i < this.units.length; i++)
		    {
			    var unit = this.units[i];
			    if (unit.owner == owner && unit.channel == channel)
			    {
				    return unit;
			    }
		    }
        }

		if (dont_create)
			return null;

		var result 	   = new ms.state.Unit();
		result.owner   = owner;
		result.channel = channel;
		result.init();

		this.units.push(result);

		return result;
	},

	add_extents: function(container, unit, extents)
	{
		for(var i = 0; i < container.length; i++)
		{
			var au = container[i];
			if (au.unit == unit)
			{
				au.start = Math.min(au.start, extents.start);
				au.end   = Math.max(au.end,   extents.end);
				return au;
			}
		}

		var result = {unit: unit, start: extents.start, end: extents.end, actions: []};
		container.push(result);

		return result;
	},

	dump_actions: function()
	{
		for(var i = 0; i < this.units.length; i++)
		{
			var unit = this.units[i];
			console.log("CHANNEL: " + unit.channel);

			for(var j = 0; j < unit.actions.length; j++)
			{
				var action = unit.actions[j];
				console.log("start:   " + action.start);
				console.log("end:     " + action.end);
				console.log("handler: " + action.handler.get_class());

				if ("dump" in action.handler)
					action.handler.dump();
			}
		}
	},

	dump_states: function()
	{
		for(var i = 0; i < this.units.length; i++)
		{
			var unit = this.units[i];
			console.log("CHANNEL: " + unit.channel);

			for(var j = 0; j < unit.states.length; j++)
			{
				var state = unit.states[j];
				console.log("start:   " + state.start);
				console.log("end:     " + state.end);
				console.log("handler: " + state.handler.get_class());

				if ("dump" in state.handler)
					state.handler.dump();
			}
		}
	}
});

//Handlers
ms.state.Interpolator = Class.create(ms.state.Handler,
{
    initialize: function(channel, interp_fn, assign_fn, keys)
    {
        this.channel    = channel;
        this.interp     = interp_fn;
        this.assign     = assign_fn;
        this.keys       = keys;
    },

	channel: function()				
    {
        return this.channel;
    },
	
    split: function(start, end)	
    {
        alert("TO IMPLEMENT: splitting iterpolators")
    },

	mix: function(other)
    {   
        throw("TO IMPLEMENT: mixing iterpolators")
    },
	
	update: function(t)				
    {
        if (!this.keys || this.keys.length < 2)
            return; //bail

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

ms.state.Caller = Class.create(ms.state.Handler,
{
    initialize: function(fn)
    {
        this.fn = fn;
    },

	flags: function()				
    {
        return ms.state.PUNCTUAL;
    },

	channel: function()				
    {
        return null;
    },
	
    split: function(start, end)	
    {
        throw("Invalid state, there is no reason to split a caller")
    },

	mix: function(other)
    {   
        throw("Invalid state, there is no reason to mix a caller")
    },
	
	update: function(t)				
    {
        this.fn();
    },
});

ms.state.Every = Class.create(ms.state.Handler,
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
