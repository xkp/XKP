
on finished()
{
	//rem set JAVA_HOME = c:\Progra~1\Java\jdk1.6.0_22\
	//rem set PATH = %PATH%; ..\android-sdk-windows\tools; ..\android-sdk-windows\platforms-tools;
	//rem set the base directory of android
	
	var appOptions = compiler.options;
	int targetAPILevel = application.targetAPILevel;
	string appName = application.appName;
	string outputClasses = application.output_classes;
	string outputJavacOpts = application.output_javac_options;
	
	if(appOptions.android_project_type != "apk")
		return;

	compiler.log("Creating .apk bundle file...");
	
	string osName = compiler.os();
	string winExt = "";
	if(os_name == "windows")
		winExt = ".bat";
	
	string appOutputPath = compiler.output_path();
	appOutputPath = appOutputPath + appName;
	string apiPlatform = application.android_sdk + "/platforms/android-" + targetAPILevel + "/android.jar";

	string dxLibPath = "";
	if(application.libraries) dxLibPath = appOutputPath + "/lib/*.jar";
	
	shell(working_path = appOutputPath)
	{
		aapt package -f -I @apiPlatform -M @appOutputPath/AndroidManifest.xml -A @appOutputPath/assets -S @appOutputPath/res -m -J @appOutputPath/gen -F @appOutputPath/bin/resources.ap_;
		javac @@@outputJavacOpts @@@outputClasses;
		dx@winExt --dex --output=@appOutputPath/bin/classes.dex @appOutputPath/bin @dxLibPath
	}
	
	string appBinPath = appOutputPath + "/bin";
	shell(working_path = appBinPath)
	{
		aapt add @appOutputPath/bin/resources.ap_ classes.dex
	}
	
	string libsParams = "-rj " + appOutputPath + "/lib";
	libsParams = "";
	string password = "xkp.android.projects.2012";
	string androidIdiomPath = appOutputPath + "/../../../idioms/android";
	shell(working_path = appOutputPath)
	{
		apkbuilder@winExt @appOutputPath/bin/@appName.apk -z @appOutputPath/bin/resources.ap_ -f @appOutputPath/bin/classes.dex -rf @appOutputPath/src @libsParams;
		jarsigner -keystore @androidIdiomPath/java.keystore/xkp.android.projects.jks -storepass @password -keypass @password -signedjar @appOutputPath/bin/@appName.apk @appOutputPath/bin/resources.ap_ xkp.android.projects
	}
}
