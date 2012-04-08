"ms.streamer".namespace();

var RESOURCE_IMAGE			 = 0;
var RESOURCE_AUDIO			 = 1;
var RESOURCE_JSON_MODEL		 = 2;
var RESOURCE_BIN_MODEL		 = 3;
var RESOURCE_SPRITE		 	 = 4;
var RESOURCE_VIDEO			 = 5;
var MAX_SIMULTANEOUS_LOADERS = 3;

function compare_loading(a, b)
{
	if (!a) return  1;
	if (!b) return -1;

	return a.time - b.time;
}

ms.streamer.Loader = Class.create(
{
    initialize: function(streamer, resource_type)
    {
		this.streamer 	   = streamer;
		this.resource_type = resource_type;
		this.queue		   = [];
		this.loading	   = 0;
    },

    push: function(res, time)
    {
    	res.time = time;
    	this.queue.push(res);
    	this.queue.sort(compare_loading);

    	if (this.loading < MAX_SIMULTANEOUS_LOADERS)
    	{
    		this.signal();
    	}
    },

    signal: function()
    {
    	this.loading++;

    	var to_load = this.queue[0];
    	this.queue.splice(0, 1);

    	this.load(to_load);
    },

    load: function(resource)
    {
    	alert("PURE FUNCTION");
    },

    on_loaded: function(resource, data)
    {
    	this.loading--;

    	if (this.queue.length > 0)
    		this.signal();

    	this.streamer.notify_loaded(resource, data, this.resource_type);
    },

    empty: function()
    {
    	return (this.queue.length == 0) && (this.loading == 0);
    }
});

ms.streamer.ImageLoader = Class.create(ms.streamer.Loader,
{
    initialize: function($super, streamer)
    {
		$super(streamer, RESOURCE_IMAGE);
    },

    load: function(resource)
    {
		var img = new Image(); 
		img.src = resource.asset;
		var canvas = document.createElement("canvas");
		
        var this_  = this;
        img.onload = function()
        {			
			canvas.width = img.naturalWidth;
			canvas.height = img.naturalHeight;
			canvas.getContext('2d').drawImage(img, 0, 0);  
			this_.on_loaded(resource, canvas);
        };        
    }
});

ms.streamer.AudioLoader = Class.create(ms.streamer.Loader,
{
    initialize: function($super, streamer)
    {
		$super(streamer, RESOURCE_AUDIO);
    },

    load: function(resource)
    {
		var audio = document.createElement('audio');		
		var this_  = this;
		this_.on_loaded(resource, audio);  
		audio.setAttribute('src', resource.asset);
    }
});

ms.streamer.VideoLoader = Class.create(ms.streamer.Loader,
{
    initialize: function($super, streamer)
    {
		$super(streamer, RESOURCE_VIDEO);
    },

    load: function(resource)
    {
		var video = document.createElement('video');		
		var this_  = this;		 				
		this_.on_loaded(resource, video);
		video.setAttribute('src', resource.asset);
    }
});

ms.streamer.JSonModelLoader = Class.create(ms.streamer.Loader,
{
    initialize: function($super, streamer)
    {
		$super(streamer, RESOURCE_JSON_MODEL);
    },

    load: function(resource)
    {
		var this_  = this;
    	this_.on_loaded(resource, resource.data);        
    }
});

ms.streamer.BinModelLoader = Class.create(ms.streamer.Loader,
{
    initialize: function($super, streamer)
    {
		$super(streamer, RESOURCE_BIN_MODEL);
    },

    load: function(resource)
    {
		var this_  = this;
    	this_.on_loaded(resource, resource.data);        
    }
});

ms.streamer.SpriteLoader = Class.create(ms.streamer.Loader,
{
    initialize: function($super, streamer)
    {
		$super(streamer, RESOURCE_SPRITE);
    },

    load: function(resource)
    {
		var img = new Image(); 
			
        var this_  = this;
        img.onload = function()
        {
    	    this_.on_loaded(resource, img);
        };
        img.src = resource.asset;
    }
});

ms.streamer.StreamProgress = Class.create(
{
	initialize: function()
	{
		this.items 		  		= 0;
		this.items_loaded 		= 0;
		this.items_confirmed	= 0;
		this.items_being_loaded = [];
	},

	progress: function()
	{
		if (this.items == 0)
			return 1;

		for(var i = this.items_being_loaded.length - 1; i >= 0; i--)
		{
			var img = this.items_being_loaded[i];

            if ("complete" in img && img.complete)
            {
	        	this.items_being_loaded.splice(i, 1);
	        	this.items_loaded++;
            }
		}

		this.items_loaded = Math.max(this.items_loaded, this.items_confirmed);

		result = this.items_loaded/this.items;
		return result;
	},

	addLoadInfo: function(li)
	{
		this.items_being_loaded.push(li);
	}
});

ms.streamer.Streamer = Class.create(
{
    initialize: function()
    {
		this.loading_queues =
		[
		 	new ms.streamer.ImageLoader(this),
			new ms.streamer.AudioLoader(this),			
			new ms.streamer.JSonModelLoader(this),
			new ms.streamer.BinModelLoader(this),
			new ms.streamer.SpriteLoader(this),
			new ms.streamer.VideoLoader(this),
		];
    },

    begin: function()
    {
    	if (this.progress)
    	{
    		alert("Already a load operation in progress");
    		return null;
    	}

    	this.progress = new ms.streamer.StreamProgress();
    	return this.progress;
    },

    end: function()
    {
    	this.progress = null;
    },
	
	all_loaded: function()
	{
        for(var i = 0; i < this.resources.length; i++)
        {
            var res = this.resources[i];
            if(!res.loaded)
				return false
        }
		return true;
	},
	
	get_resource: function(res_id)
	{
        for(var i = 0; i < this.resources.length; i++)
        {
            var res = this.resources[i];
            if (res_id == res.id)
            {
                return res; 
            }
			if (res_id == res.asset)
            {
                return res; 
            }
        }
	},
	
    request: function(resources, listener, time)
    {
    	var req;
    	for(var i = 0; i < this.requests.length; i++)
    	{
    		var r = this.requests[i];
    		if (r.listener == listener)
    		{
    			req = r;
    			break;
    		}
    	}

    	if (!req)
    		req = {listener: listener, time: time, waiting: 0};

    	for(var i = 0; i < resources.length; i++)
    	{
    		var res 	 = resources[i];
    		var url_info = this.queue_resource(res, time);
			if (url_info.loaded)
			{
				var data = url_info.data;
				res.asset = res.url;
				listener.resource_loaded(res, data);
			}
			else
			{
				req.waiting++;
				url_info.requests.push(req);
			}
    	}

    	if (req.waiting == 0 && listener)
    	{
    		listener.finished_loading();
    	}

    	return req;
    },

    queue_resource: function(resource, time)
	{
    	if (!resource.url)
    	{
    		alert("What are you trying to load?");
    		return null;
    	}

    	var result;
    	if (!resource.dup)
    	{
        	for(var i = 0; i < this.resources.length; i++)
        	{
        		var res = this.resources[i];
        		if (res.asset == resource.url)
        		{
        			result = res;
        			result.time = Math.min(result.time, time);
        			break;
        		}
        	}
    	}

		if (!result)
		{
			result =
			{	
				id:		  		resource.id, 	 
				asset:    		resource.url,
				type:  			resource.type,
				loaded:   		false,
				requests: 		[],
				progress: 		this.progress,
				frame_width:	resource.frame_width,
				frame_height:	resource.frame_height,
				animations:		resource.animations
			};

			if (this.progress)
				this.progress.items++;

			this.resources.push(result);
			this.loading_queues[resource.type].push(result, time);
		}

		return result;
	},

	notify_loaded: function(resource, data, type)
	{
		if (resource.progress)
		{
			resource.progress.items_confirmed++;
		}

		resource.loaded = true;
		resource.data   = data;
		for(var i = resource.requests.length - 1; i >= 0; i--)
		{
			var req = resource.requests[i];
			resource.requests.splice(i, 1);
			req.listener.resource_loaded(resource, data, type);

			req.waiting--;
			if (req.waiting <= 0)
				req.listener.finished_loading();
		}

		if (this.on_loaded)
		{
			for(var i = 0; i < this.loading_queues.length; i++)
			{
				var lq = this.loading_queues[i];
				if (!lq.empty())
					return;
			}

			this.on_loaded();
		}
	},

    resources: 		 [],
    requests:  		 [],
    loading_queues:  [],
    current_request: null

});

var STATE_UNLOADED  = 0;
var STATE_LOADING   = 1;
var STATE_LOADED    = 2;

ms.streamer.Package = Class.create(
{
    initialize: function(streamer, items)
    {
        this.items 		= items? items : [];
        this.state    	= STATE_UNLOADED;
        this.streamer 	= streamer;
        this.job		= null;
		this.events		= new ms.event.EventHolder();
		this.all_loaded = false;
    },
	
	add_item: function(item)
	{
		this.items.push(item);
		this.state  = STATE_UNLOADED;
		this.all_loaded = false;
	},
	
    load: function()
    {
        if (this.state != STATE_UNLOADED)
            return;
        
        this.state = STATE_LOADING;
        this.job = this.streamer.begin();
        var resources = [];
        for(var i = 0; i < this.items.length; i++)
        {				
			resources.push({id: this.items[i].id, type: this.items[i].resource_type, url: this.items[i].src,
								frame_width: this.items[i].frame_width, frame_height: this.items[i].frame_height,
								animations: this.items[i].animations
								});
        }
		
       	this.streamer.request( resources, this );
			
        this.streamer.end();    
    },
	
	get_item: function(item_id)
	{
        for(var i = 0; i < this.items.length; i++)
        {
            var item = this.items[i];
            if (item_id == item.id)
            {
                return item; 
            }
			if (item_id == item.src)
            {
                return item; 
            }
        }
	},

	resource_loaded: function(res, data)
	{
        for(var i = 0; i < this.items.length; i++)
        {
            var item = this.items[i];            
            if (res.asset == item.src)
            {
                item.resource = data; 
            }
        }
	},

    finished_loading: function()
    {
		this.all_loaded = true;
		this.events.dispatch("loaded", []); 		
    },

    progress: function()
    {
        if (this.state == STATE_UNLOADED)
            return 0;

        if (this.state == STATE_LOADED)
            return 1;
        
        var result = this.job.progress();
        if (result >= 1)
        {
            this.state = STATE_LOADED;
            if (this.on_loaded)
                this.on_loaded();
        }

        return result;
    },
});

ms.streamer.PackageContainer = Class.create(
{
    initialize: function()
    {
        this.items = [];
    },

    register: function(id, pack)
    {
        this.items.push({id:id, pack:pack});
    },

    get: function(id)
    {
        for(var i = 0; i < this.items.length; i++ )
        {
            var itm = this.items[i];
            if (itm.id == id)
                return itm.pack;
        }
        return null;
    },
});

ms.streamer.ResourceUtils = Class.create(
{
	get_resource: function(id)
	{
		return streamer.get_resource(id).data;
	}
});
