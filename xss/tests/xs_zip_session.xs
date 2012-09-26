on home.render()
{
    var logged = session.get("logged");
    if (!logged)
        return redirect(page = "login");

    return redirect(page = "file_list");
}

on do_login.render(user)
{
    session.set("user",   user);
    session.set("logged", true);
    return redirect(page = "file_list");
}

on file_list.render()
{
    var logged = session.get("logged");
    if (!logged)
        return redirect(page = "login");

    var history = session.get("history") || [];

    user_name.inner_html = session.get("user");
    files.data = history;
}

on file_list.files.render(item)
{
    file_name.inner_html = item.file;
    link.href = item.url;
}

function compress_file(file, rename_to)
{
    shell()
    {
        gzip @file
    }

    var new_path = "./files/compressed/" + rename_to;
    fs.rename(file + ".gz", new_path);
    return {local_path : new_path, url : "compressed/" + rename_to};
}

on compress.render()
{
    var logged = session.get("logged");
    if (!logged)
        return redirect(page = "login");

    var filename = post.files[0].name + ".gz";

    var uploaded_file = post.files[0].path;
    var result = compress_file(uploaded_file, filename);

    //fill page data
    file.inner_html = filename;
    link.href = result.url;

    //save to session
    result.file = filename;

    var history = session.get("history") || [];
    history.push(result);
    session.set("history", history);
}