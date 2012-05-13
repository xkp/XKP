
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
	shell(working_path = appOutputPath, environment_var = "...")
	{
		aapt package -f -I @apiPlatform -M @appOutputPath/AndroidManifest.xml -A @appOutputPath/assets -S @appOutputPath/res -m -J @appOutputPath/gen -F @appOutputPath/bin/@appName.ap_
	}
	
	
	/*
	//ERROR: on concat expression javaFileList when string declaration is not initialized
	int counter = 0;
	string javaFileList = "";
	var arrayJavaFile = compiler.find_files(appOutputPath, ".*\.java");
	for(var f in arrayJavaFile)
	{
		string file = f.filename;
		if(counter > 0)
			javaFileList = javaFileList + " " + file;
		else
			javaFileList = file;
	}
	*/
	
	
	//TIPS: i need that the string expression scape some characters like quotes
	//string javacParams = "-encoding ascii -target 1.5 -bootclasspath " + apiPlatform + " -classpath " + apiPlatform + " -sourcepath " + appOutputPath + "/src -sourcepath " + appOutputPath + "/gen";
	//compiler.log(javacParams);
	//compiler.log(appOutputPath);
	
	/*
	var arrayJavaFile = compiler.find_files(appOutputPath, ".*\.java");
	for(var file in arrayJavaFile)
	{
		string filename = file.filename;
		compiler.log("Compiling with javac: " + filename);
		
		shell(working_path = "d:/xkp/xss/production/android/SimpleTest")
		{
			javac -encoding ascii -target 1.5 -classpath @apiPlatform -sourcepath "src;gen" -d bin @filename
		}
	}
	*/

	/*	
	//javaFileList = "src/xkp/android/Act" + appName + ".java";
	//javaFileList = "gen/xkp/android/" + appName + "/R.java";
	//javaFileList = javaFileList + " src/xkp/android/libs/Layout/XKPLayout.java";
	//javaFileList = javaFileList + " src/xkp/android/" + appName + "/Act" + appName + ".java";

	//compiler.log(javaFileList);
	
	shell(working_path = "d:/xkp/xss/production/android/SimpleTest")
	{
		javac -encoding ascii -target 1.5 -d bin -classpath @apiPlatform -sourcepath "src;gen" @javaFileList
	}
	*/
	
	shell(working_path = "d:/xkp/xss/production/android/SimpleTest")
	{
		javac @options @classes
	}
	
	string libsParams = "-rj " + appOutputPath + "/libs";
	libsParams = "";
	
	//compiler.log(appOutputPath);
	
	shell()
	{
		dx@winExt --dex --output=@appOutputPath/bin/classes.dex @appOutputPath/bin~
		apkbuilder@winExt @appOutputPath/bin/@appName.apk -z @appOutputPath/bin/@appName.ap_ -f @appOutputPath/bin/classes.dex -rf @appOutputPath/src @libsParams
	}
	
		//javac -encoding ascii -target 1.5 -classpath @apiPlatform -sourcepath "@appOutputPath/src;@appOutputPath/gen" -d @appOutputPath/bin @javaFileList~
}
