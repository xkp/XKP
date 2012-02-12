method service1(string file)
{
    array data;
    sql()
    {
        data =  SELECT  * 
                FROM    Registry
                WHERE   file = @file AND
                        visited = 0
    }    

    int count = 0;
    for(var d in data)
    {
        string curr_file = d.file;
        count += d.views;
        shell()
        {
            git commit @curr_file
        }

        count--;

        sql()
        {
            INSERT INTO Registry(file_name, visited)
            VALUES              (@file, 1)
        }

        if (count > 5)
        {
            while (count > 0)
            {
                shell()
                {
                    git mark @curr_file @count
                }

                count--;
            }
        }
        else 
        {
            object result = object();
            result.desc   = "Not enough views";
            result.status = "suspended";
            return result;
        } 
    }

    return true;
}