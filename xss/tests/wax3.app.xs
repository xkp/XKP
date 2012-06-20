
on home.render()
{
    var user = {id: "admin"};
    
    user_name.text = user.id;

    var data = [];
    sql()
    {
        data = SELECT * FROM FOLLOWERS
               WHERE USERID = @user.id
    }

    //fun with web services
    for(var follower in data)
    {
        follower.map = google_maps.staticmap(center = follower.address, zoom = 5, sensor = false);
    }

    followers.data = data;
}

on home.followers.render(item)
{
    fname.inner_html = item.name;
    fimg.src = item.image;
}