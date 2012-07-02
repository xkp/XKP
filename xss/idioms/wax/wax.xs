
on pre_process(obj)
{
    if (obj.class_name == "web_service")
    {
        var host = obj.host;
        if (!host)
            compiler.error("Web service methods expect a 'host' parameter");

        var port = obj.port;
        if (!port)
            port = 80;

        for(var wsm in obj.methods)
        {
            wsm.asynch = true;
            wsm.host = host;
            wsm.port = port;
            wsm.renderer = "web_service.xss";
        }
    }
}
