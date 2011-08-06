@echo off
rem set JAVA_HOME = c:\Progra~1\Java\jdk1.6.0_22\
rem set in Path = android-sdk-windows\tools; android-sdk-windows\platforms-tools;
rem set the base directory of android

	aapt p -m -J D:\xkp\xss\production\android\src -M AndroidManifest.xml -S D:\xkp\xss\production\android\res -I d:/android/android-sdk-windows/platforms/android-8/android.jar
	javac -encoding ascii -target 1.5 -d D:\xkp\xss\production\android\bin -bootclasspath d:/android/android-sdk-windows/platforms/android-8/android.jar D:\xkp\xss\production\android\src/com/xkp/android/SimpleTest/*.java
	dx --dex --output=D:\xkp\xss\production\android\bin/classes.dex D:\xkp\xss\production\android\bin
	aapt p -f -M AndroidManifest.xml -S D:\xkp\xss\production\android\res -I d:/android/android-sdk-windows/platforms/android-8/android.jar -F D:\xkp\xss\production\android\bin/output.ap_ 
	apkbuilder D:\xkp\xss\production\android\bin/SimpleTest.apk -z D:\xkp\xss\production\android\bin/output.ap_ -f D:\xkp\xss\production\android\bin/classes.dex -rf D:\xkp\xss\production\android\src -rj D:\xkp\xss\production\android\libs
	adb install -r D:\xkp\xss\production\android\bin/SimpleTest.apk

