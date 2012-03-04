on render_initialization()
{
}

on render_instances(app)
{
	compiler.log("Copying resource files...");
	
	//TRACE: log
	//compiler.log("Begin Rendering Resources Instances...");
	
	string pattern_numbers = "0123456789";
	string pattern_string  = "abcdefghijklmnopqrstuvwxyz";
	string pattern_others  = "._";
	string pattern_all     = pattern_numbers + pattern_string + pattern_others;

	for(var inst in instances)
	{
		//TIPS: stupid stripped string... live is hard :(
		string project_path = compiler.project_path() + "/";
		string output_path = app + inst.droid_location;
		
		string simg = inst.src;
		int sz = String.size(simg);
		
		int pos = String.find_last(simg, "\\/");
		int rpos = pos + 1;
		int npos = sz - pos - 1;
		string fn_img = String.substr(simg, rpos, npos);
		
		//check resources filename and
		int fd1 = String.find_first_not(fn_img, pattern_all);
		int fd2 = String.find_first(fn_img, pattern_numbers);

		if(fd1 != -1 || fd2 == 0)
			compiler.error("Resource filename is incorrect", filename = fn_img);
		
		//then
		string srcf = project_path + simg;
		string dstf = output_path + fn_img;
		
		//coping resource
		//TRACE: log
		//compiler.log("Copying resource file " + srcf);
		compiler.copy_file(src = srcf, dst = dstf);
		
		//TODO: like classes.xml idiom on property res_id of image view
		//pos = String.find_last(fn_img, ".");
		//assignValue = "@" + inst.droid_type + "/" + String.substr(fn_img, 0, pos);
		//inst.assignValue = assignValue;
	}
	
	//TRACE: log
	//compiler.log("End Rendering Resources Instances...");
}

on render_types(app)
{
}
