on finished()
{
    string file = project.js_path + "/source.xxx";
    compiler.log(file);
	
    shell()
    {
        result = git.exe --version
    }

	for(string line in result)
	{
		compiler.log("EXEC_OUTPUT>> " + line);
	}

    compiler.xss("xss14_1.xss", output_file = "xss14.txt");
}
