
on pre_process(obj)
{
    if (obj.class_name == "application")
    {
        array<object> services = obj.find_by_type("service");

        //preprocess utility methods (those who aren't services)
        //to determine whether they are forked

        for(var methd in obj.methods)
        {
            //hookup
            var code   = methd.code;
            code.owner = methd;

            //find out whether these are services or utilities
            bool is_service = false;
            for(var service in services)
            {
                if (service.id == methd.id)
                {
                    is_service = true;
                    methd.add_parameter("request");
                    methd.add_parameter("response");

                    //td: !!! params
                    break;
                }
            }

            if (is_service)
            {
                methd.wax_service = true;
                continue;
            }

            code.check_fork(methd);

            if (code.forked)
                methd.add_parameter("return_function");
        }

        //get the pages
        array<object> pages = obj.find_by_type("page");

        for(object page in pages)
        {
            
        }
    }
}
