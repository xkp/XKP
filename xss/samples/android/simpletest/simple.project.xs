
on finished()
{
	//rem set JAVA_HOME = c:\Progra~1\Java\jdk1.6.0_22\
	//rem set PATH = %PATH%; ..\android-sdk-windows\tools; ..\android-sdk-windows\platforms-tools;
	//rem set the base directory of android
	
	var appOptions = compiler.options;
	int targetAPILevel = application.targetAPILevel;
	string appName = application.appName;
	
	if(appOptions.android_project_type != "apk")
		return;

	compiler.log("Creating bundle apk file...");
	
	//td: 
	string sdkBase = "d:/android/sdk/";
	string winExt = ".bat";
	
	string appOutputPath = compiler.output_path();
	appOutputPath = appOutputPath + appName;

	string apiPlatform = sdkBase + "platforms/android-" + targetAPILevel + "/android.jar";
	
	//td: make that some parameters of shell work well, like that:
	//    shell(working_path="...", environment_var="...") { ... }
	shell()
	{
		aapt p=xkp.android.@appName -f -M @appOutputPath/AndroidManifest.xml -A @appOutputPath/assets -S @appOutputPath/res -m -J @appOutputPath/gen -I @apiPlatform
	}
	
	//ERROR: on concat expression javaFileList when string declaration is not initialized
	string javaFileList = "";
	var arrayJavaFile = compiler.find_files(appOutputPath, ".*\.java");
	for(var f in arrayJavaFile)
	{
		string file = f.filename;
		javaFileList = javaFileList + " " + file;
		break;
	}
	
	//TIPS: i need that the string expression scape some characters like quotes
	//string javacParams = "-encoding ascii -target 1.5 -bootclasspath " + apiPlatform + " -classpath " + apiPlatform + " -sourcepath " + appOutputPath + "/src -sourcepath " + appOutputPath + "/gen";
	//compiler.log(javacParams);
	//compiler.log(appOutputPath);
	
	/*
	var arrayJavaFile = compiler.find_files(appOutputPath, ".*\.java");
	for(var file in arrayJavaFile)
	{
		string filename = file.filename;
		compiler.log(filename);
		shell()
		{
			javac -encoding ascii -target 1.5 -d @appOutputPath/bin -bootclasspath @apiPlatform -classpath @apiPlatform -sourcepath "@appOutputPath/src;@appOutputPath/gen" @filename
		}
	}
	*/

	string libsParams = "-rj " + appOutputPath + "/libs";
	libsParams = "";
	
	shell()
	{
		javac -encoding ascii -target 1.5 -d @appOutputPath/bin -bootclasspath @apiPlatform -classpath @apiPlatform -sourcepath "@appOutputPath/src;@appOutputPath/gen" @javaFileList~
		dx@winExt --dex --output=@appOutputPath/bin/classes.dex @appOutputPath/bin~
		aapt p=xkp.android.@appName -f -M @appOutputPath/AndroidManifest.xml -S @appOutputPath/res -I @apiPlatform -F @appOutputPath/bin/@appName.ap_~
		apkbuilder@winExt @appOutputPath/bin/@appName.apk -z @appOutputPath/bin/@appName.ap_ -f @appOutputPath/bin/classes.dex -rf @appOutputPath/src @libsParams
	}
}
