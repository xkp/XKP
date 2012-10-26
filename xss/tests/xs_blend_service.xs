on blend.render()
{	
    var uploaded_file = post.files[0].path;
	var url = 'upload/' + post.files[0].name;    
    fs.rename(post.files[0].path, url);
	
    shell()
    {
       blender -b -P test.py -- @url test.xml
    }
	
    return redirect(page = "edit_page");
}

on edit_page.render()
{
	initial_script.inner_html = "var dynamic_src = 'model/balldroid/balldroid.dae';";
}
