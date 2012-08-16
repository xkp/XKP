property project_db = 
{
    hostname: 'localhost', 
    user: 'root', 
    password: 'xs@2011', 
    database: 'NodeTest'
};

/*
function check_changes()
{
    var changes;
    shell()
    {
        cd @project_info.LocalFolder;
        changes = git diff master origin/master --summary
    }

    return changes.size > 0;
}
*/

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

method list_projects()
{
    var result;
    shell(cwd = "/root/nodejs/samples/")
    {
        result = ls 
    }

    return result.output;
}
