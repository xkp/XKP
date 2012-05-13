
on finished()
{
	//rem set JAVA_HOME = c:\Progra~1\Java\jdk1.6.0_22\
	//rem set PATH = %PATH%; ..\android-sdk-windows\tools; ..\android-sdk-windows\platforms-tools;
	//rem set the base directory of android
	
	var appOptions = compiler.options;
	int targetAPILevel = application.targetAPILevel;
	string appName = application.appName;
	string outputClasses = application.output_classes;
	
	if(appOptions.android_project_type != "apk")
		return;

	compiler.log("Creating bundle apk file...");
	
	//td: 
	string sdkBase = "d:/android/sdk/";
	string winExt = ".bat";
	
	string appOutputPath = compiler.output_path();
	appOutputPath = appOutputPath + "/" + appName;

	string apiPlatform = sdkBase + "platforms/android-" + targetAPILevel + "/android.jar";
	
	//td: make that some parameters of shell work well, like that:
	//    shell(working_path="...", environment_var="...") { ... }
	shell(working_path = appOutputPath, environment_var = "...")
	{
		aapt package -f -I @apiPlatform -M @appOutputPath/AndroidManifest.xml -A @appOutputPath/assets -S @appOutputPath/res -m -J @appOutputPath/gen -F @appOutputPath/bin/@appName.ap_
	}
	
	//shell(working_path = appOutputPath)
	shell(working_path = "d:/xkp/xss/production/android/SimpleTest")
	{
		javac @@options @@@outputClasses
	}
	
	string libsParams = "-rj " + appOutputPath + "/libs";
	libsParams = "";
	
	shell()
	{
		dx@winExt --dex --output=@appOutputPath/bin/classes.dex @appOutputPath/bin~
		apkbuilder@winExt @appOutputPath/bin/@appName.apk -z @appOutputPath/bin/@appName.ap_ -f @appOutputPath/bin/classes.dex -rf @appOutputPath/src @libsParams
	}
}
