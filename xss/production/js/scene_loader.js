var loader = {};

loader.AppLoader = Class.extend(
{
	init: function(manager)
	{
        this.manager = manager;
	},	

	load: function(document)
	{
        this.manager.clear();
        load_node(document, null);
	},

    load_node: function(node, parent)
    {
        var loader = this;
        node.children().each(function()
        {
            var child      = $(this);
            var class_name = this.tagName;


        });
    },
});
