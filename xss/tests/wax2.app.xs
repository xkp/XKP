property project_db = 
{
    hostname: 'localhost', 
    user: 'root', 
    password: 'xs@2011', 
    database: 'NodeTest'
};

method compile(int project_id)
{
    object project_info;
    sql(connection = project_db)
    {
        project_info = SELECT * FROM  Project
                                WHERE Id = @project_id
    }

    if (!project_info)
        return {success: false};

    return {success: true, folder: project_info.LocalFolder};
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

method compress_service(filename)
{
    var uploaded_file = post.files[0].path;
    var result = compress_file(uploaded_file, filename);
    return {success: true, url: result.url, local_path: result.local_path};
}

on compress_page.render(filename)
{
    var last = cookies.get("last_file");

    lblFile.inner_html = filename;

    var uploaded_file = post.files[0].path;
    var result = compress_file(uploaded_file, filename);
    
    lblLink.href = result.url;
}