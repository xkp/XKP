method myService()
{
    try 
    {
        var data = [];
        sql()
        {
            data = SELECT * FROM FOLLOWERS
                   WHERE USERID = @user.id
        }

        //fun with web services and caching
        for(var user in data)
        {
            try
            {
                string img_cache;
                sql() 
                { 
                    img_cache = SELECT fname FROM MapCache
                                WHERE addr = @user.address
                }
                
                if (!img_cache)
                {
                    var img_data = google_maps.staticmap(center = user.address, zoom = 5, sensor = false);
                    
                    //do the caching
                    img_cache = 'mapcache/' + Math.random()*1000 + '.png';
                    fs.writeFile(img_cache, img_data);

                    sql()
                    {
                        INSERT INTO   MapCache(addr, fname)
                               VALUES (@user.address, @img_cache)
                    }
                }
                    
                user.map = img_cache;
            }
            catch(err)
            {
                user.map = 'images/nomap.png';
            }
        }

        return {success: true, users: data};
    }
    catch
    {
        return {success: false};
    }
}
