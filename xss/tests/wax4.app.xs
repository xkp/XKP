method service1(int limit2)
{
    var data;
    int limit = 20;

    sql()
    {
        data =  SELECT  * 
                FROM    DataTable
                WHERE   Column > @limit AND
                        OtherColumn < @limit2
    }    

    for(var d in data)
    {
        d.Column -= limit;
    }

    array<string> lines; 
    shell()
    {   
        lines = git status
    }

    if (lines[1] != "ok")
    {
        object error = object();
        error.rejected = true;
        error.reason = "git failed";
        return error;
    }    

    return data;
}