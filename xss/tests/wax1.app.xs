method foo()
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
        for(var follower in data)
        {
            try
            {
                string img_cache;
                sql() 
                { 
                    img_cache = SELECT fname FROM MapCache
                                WHERE addr = @follower.address
                }
                
                if (!img_cache)
                {
                    var img_data = google_maps.staticmap(center = follower.address, zoom = 5, sensor = false);
                    
                    //do the caching
                    img_cache = 'mapcache/' + Math.random()*1000 + '.png';
                    fs.writeFile(img_cache, img_data);

                    sql()
                    {
                        INSERT INTO   MapCache(addr, fname)
                               VALUES (@follower.address, @img_cache)
                    }
                }
                    
                follower.map = img_cache;
            }
            catch(err)
            {
                follower.map = 'images/nomap.png';
            }
        }

        followers.data = data;
    }
    catch
    {
    }
}
