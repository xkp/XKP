
var JQUComponent = 
{
    set_visible : function(inst, value)
    {
        if (value)
            inst.show();        
        else
            inst.hide();        
    },

    get_visible : function(inst)
    {
        return inst.is(":visible");
    },

    set_enabled : function(inst, value)
    {
        if (value)
            inst.removeAttr("disabled");        
        else
            inst.attr("disabled", "disabled");        
    },

    get_enabled : function(inst)
    {
        return !inst.is(":disabled");
    },
}

var JQUCheckbox = 
{
    get_checked : function(cb)
    {
        return $(cb.__input).attr('checked') != undefined;
    },

    set_checked : function(cb, value)
    {
        return $(cb.__input).attr('checked', value);
    },
}

var JQUGrid = 
{
    set_data : function(grid, data)
    {
        for(var i = 0; i <= data.length; i++)
	        grid.jqGrid('addRowData',i + 1, data[i]);    
    },
}

var JQUMap = 
{
    set_visible : function(map, visible)
    {
        JQUComponent.set_visible(map, visible);
        if (visible)
            map.triggerEvent('resize');
    },
}
