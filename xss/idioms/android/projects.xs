
on finished()
{
	//set JAVA_HOME = c:\Progra~1\Java\jdk1.6.0_22\
    //set ANDROID_SDK = d:\android\sdk\
	//set PATH = %PATH%; ..\android-sdk-windows\tools; ..\android-sdk-windows\platforms-tools;
	
	var appOptions = compiler.options;
	int targetAPILevel = application.targetAPILevel;
	string appName = application.appName;
	string outputClasses = application.output_classes;
	string outputJavacOpts = application.output_javac_options;
	
	if(appOptions.android_project_type != "apk")
		return;

	compiler.log("Creating .apk bundle file...");
	
	string osName = compiler.os();
    string exeExt = "";
	string batExt = "";
	if(os_name == "windows")
    {
        exeExt = ".bat";
		batExt = ".exe";
    }
	
	string appOutputPath    = compiler.output_path();
	appOutputPath           = appOutputPath + appName;
    string androidSDKPath   = application.android_sdk;
	string apiPlatform      = androidSDKPath + "/platforms/android-" + targetAPILevel + "/android.jar";

    string output_classes = appName + "/" + application.output_classes;
    string output_javac_options = appName + "/" + application.output_javac_options;
    
    string xss_classes = "java.lst.xss";
    string xss_javac_options = "javac.options.xss";

    compiler.log(xss_classes);
    compiler.log(output_classes);
    compiler.log(appOutputPath);
    compiler.log(apiPlatform);
    
    compiler.xss(xss_classes, output_file = output_classes, appOutputPath);
    //compiler.xss(xss_javac_options, output_file = output_javac_options, appOutputPath);

	string dxLibPath = "";
	if(application.libraries) dxLibPath = appOutputPath + "/lib/*.jar";
	
	shell(working_path = appOutputPath)
	{
        aapt@exeExt package -f -I @apiPlatform -M @appOutputPath/AndroidManifest.xml -A @appOutputPath/assets -S @appOutputPath/res -m -J @appOutputPath/gen -F @appOutputPath/bin/resources.ap_;
        javac @@@outputJavacOpts @@@outputClasses;
		dx@batExt --dex --output=@appOutputPath/bin/classes.dex @appOutputPath/bin @dxLibPath
	}
        //@androidSDKPath/platform-tools/aapt@exeExt package -f -I @apiPlatform -M @appOutputPath/AndroidManifest.xml -A @appOutputPath/assets -S @appOutputPath/res -m -J @appOutputPath/gen -F @appOutputPath/bin/resources.ap_;
        //javac @@@outputJavacOpts @@@outputClasses;
        //@androidSDKPath/platform-tools/dx@batExt --dex --output=@appOutputPath/bin/classes.dex @appOutputPath/bin @dxLibPath
	
	string appBinPath = appOutputPath + "/bin";
	shell(working_path = appBinPath)
	{
		aapt@exeExt add @appOutputPath/bin/resources.ap_ classes.dex
	}
        //@androidSDKPath/platform-tools/aapt@exeExt add @appOutputPath/bin/resources.ap_ classes.dex
	
	string libsParams = "-rj " + appOutputPath + "/lib";
	libsParams = "";
	string password = "xkp.android.projects.2012";
	string androidIdiomPath = appOutputPath + "/../../../idioms/android";
	shell(working_path = appOutputPath)
	{
		apkbuilder@winExt @appOutputPath/bin/@appName.apk -z @appOutputPath/bin/resources.ap_ -f @appOutputPath/bin/classes.dex -rf @appOutputPath/src @libsParams;
		jarsigner -keystore @androidIdiomPath/java.keystore/xkp.android.projects.jks -storepass @password -keypass @password -signedjar @appOutputPath/bin/@appName.apk @appOutputPath/bin/resources.ap_ xkp.android.projects
	}
        //@androidSDKPath/tools/apkbuilder@winExt @appOutputPath/bin/@appName.apk -z @appOutputPath/bin/resources.ap_ -f @appOutputPath/bin/classes.dex -rf @appOutputPath/src @libsParams;
        //jarsigner -keystore @androidIdiomPath/java.keystore/xkp.android.projects.jks -storepass @password -keypass @password -signedjar @appOutputPath/bin/@appName.apk @appOutputPath/bin/resources.ap_ xkp.android.projects
}
