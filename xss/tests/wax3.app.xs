
on home.render()
{
    user_name.text = user.id;         //changes the text inside an html <p>here goes user.id</p>  
    user_image.src = user.image_url;  //changes an attribute of an html element <img src="here goes user.image_url"></img>  

    var data = [];
    sql()
    {
        data = SELECT * FROM FOLLOWERS
               WHERE USERID = @user.id
    }

    //
    followers.data = data;
}

on home.followers.render(item)
{
    follower.inner_html = item.name;
}