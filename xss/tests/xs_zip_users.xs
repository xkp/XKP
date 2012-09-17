on home.render()
{
    var logged = session.get("logged");
    if (!logged)
        return redirect(page = "login");

    return redirect(page = "file_list");
}

property compressor_db = 
{
    hostname: 'localhost', 
    user: 'root', 
    password: 'xs@2011', 
    database: 'Compressor'
};

on do_login.render(user, password)
{
    var user_data;
    sql(connection = compressor_db)
    {
        user_data = select * from Users
                    where UserName = '@user'
    }

    if (!user_data)
        return redirect(page = "login");

    if (user_data.Password && user_data.Password == password)
    {
        session.set("logged",   true);
        session.set("user",     user);
        session.set("user_id",  user_data.UserID);
        session.set("user_map", user_data.Map);

        return redirect(page = "file_list");
    }
    else
        return redirect(page = "login");
}

on logout.render()
{
    session.set("logged", false);
    return redirect(page = "login");
}

on do_register.render(user, password, location)
{
    var user_data;
    sql(connection = compressor_db)
    {
        user_data = select * from Users
                    where UserName = '@user'
    }

    //check if registered
    if (user_data)
        return redirect(page = "login");

    //obtain the use locations map
    var map_file = 'usermaps/' + user + '_map.png'; 

    string map_center = location || "Burundi";
    int    zoom       = 8;
            
    if (location) zoom = 12; 
                    
    //do the caching
    var img_data = google_maps.staticmap(center = map_center, zoom = zoom, sensor = false, size="300x200");
    fs.writeFile('files/' + map_file, img_data, 'binary');

    //and remember the user
    sql(connection = compressor_db)
    {
        insert into Users (UserName, Password, Map)
        values ('@user', '@password', '@map_file')
    }

    return redirect(page = "login");
}

on file_list.render()
{
    var logged = session.get("logged");
    if (!logged)
        return redirect(page = "login");

    array files;
    var user_id = session.get("user_id");
    sql(connection = compressor_db)
    {
        files = select * from UserFiles
                where UserId = @user_id
    }

    user_name.inner_html = session.get("user");
    files.data = files;
    user_map.src = session.get("user_map");
}

on file_list.files.render(item)
{
    file_name.inner_html = item.File;
    link.href = item.Url;
}

on compress.render()
{
    var logged = session.get("logged");
    if (!logged)
        return redirect(page = "login");

    var uploaded_file = post.files[0].path;
    shell()
    {
        gzip @uploaded_file
    }

    var filename = post.files[0].name;
    var url = "compressed/" + filename + ".gz";
    fs.rename(uploaded_file + ".gz", 'files/' + url);

    //fill page data
    file.inner_html = filename;
    link.href = url;

    //save to db
    var user_id = session.get("user_id");
    sql(connection = compressor_db)
    {
        insert into UserFiles (UserId, File, Url)
        values (@user_id, '@filename', '@url')
    }
}