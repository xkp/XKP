property name;

on blend.render()
{   
	name = post.files[0].name;
	var url = 'D:/Pabli/Trabajo/Excess/Projects/file_server/upload/' + post.files[0].name;    
    fs.rename(post.files[0].path, url);
	console.log(url);
	string output;
    shell()
    {
		output =  blender -b -P load.py -- @url
    }
	console.log(output);
    return redirect(page = "edit_page");
}

on edit_page.render()
{
	initial_script.inner_html = "var dynamic_src = 'upload/" + name + ".dae';";
}
