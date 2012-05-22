on finished()
{
    string file = project.js_path + "/source.xxx";
    compiler.log(file);
    shell()
    {
        result = javac.exe @file -a --message 'testing'
    }

    compiler.xss("xss14_1.xss", output_file = "xss14.txt");
}
