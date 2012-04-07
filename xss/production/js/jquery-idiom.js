
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
}