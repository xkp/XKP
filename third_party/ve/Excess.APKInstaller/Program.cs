using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;

namespace Excess.APKInstaller
{
    class Emulator
    {
        public string Name;
        public string Target;
        public int apiLevel;
    }

    class Program
    {
        private const int MAX_SLEEP_TIME = 300000; // 5 min

        private static string strAndroidSDKPath;
        private static DirectoryInfo diAndroidSDK;
        private static FileInfo fiCommandADB;
        private static FileInfo fiCommandAndroid;
        private static FileInfo fiCommandEmulator;

        private static string TargetDevice;
        private static Emulator TargetEmulator = null;

        private static int APKTargetApi;
        private static string APKFilename;
        private static string APKPackageName;
        private static string APKActivityName;

        private static List<string> idDevices = new List<string>();
        private static List<Emulator> emulators = new List<Emulator>();

        static void Main(string[] args)
        {
            //td: get all arguments & save inside statics variables
            if (args.Length < 1)
            {
                //td: how to output errors to manipulate by other side??
                Console.WriteLine("ERROR: TODO 1: ");
                return;
            }

            if (args.Length % 2 != 0)
            {
                Console.WriteLine("ERROR: La cantidad de parámetros a manipular no es correcta.");
                return;
            }

            // read arguments from command line
            char lastOpt = '0';
            foreach (string arg in args)
            {
                if (lastOpt != '0')
                {
                    switch (lastOpt)
                    {
                        case 'f':
                            APKFilename = arg;
                            break;

                        case 'a':
                            APKTargetApi = Convert.ToInt32(arg);
                            break;

                        case 'p':
                            APKPackageName = arg;
                            break;

                        case 'i':
                            APKActivityName = arg;
                            break;
                    }

                    lastOpt = '0';
                }
                else
                {
                    if (arg[0] == '-')
                    {
                        lastOpt = arg[1];
                        continue;
                    }
                }
            }

            //td: verify if APKFilename exists
            if (!File.Exists(APKFilename))
            {
                Console.WriteLine("ERROR: El archivo {0} que se pasa por argumento no existe", APKFilename);
                return;
            }

            #region ...validation of path & environment variables of android tools
            // get and check environment variable
            strAndroidSDKPath = Environment.GetEnvironmentVariable("ANDROID_SDK");
            if (string.IsNullOrEmpty(strAndroidSDKPath))
            {
                Console.WriteLine("TODO 2: ");
                return;
            }

            // check android directory
            diAndroidSDK = new DirectoryInfo(strAndroidSDKPath);
            if (!diAndroidSDK.Exists)
            {
                Console.WriteLine("TODO 3: ");
                return;
            }

            // check tools of android
            fiCommandADB = new FileInfo(diAndroidSDK.FullName + @"\platform-tools\adb.exe");
            if (!fiCommandADB.Exists)
            {
                Console.WriteLine("TODO 4: ");
                return;
            }

            fiCommandAndroid = new FileInfo(diAndroidSDK.FullName + @"\tools\android.bat");
            if (!fiCommandAndroid.Exists)
            {
                Console.WriteLine("TODO 5: ");
                return;
            }

            fiCommandEmulator = new FileInfo(diAndroidSDK.FullName + @"\tools\emulator.exe");
            if (!fiCommandEmulator.Exists)
            {
                Console.WriteLine("TODO 6: ");
                return;
            }
            #endregion

            // now begin the big process of execute and parse output result
            // step 1: get devices plugged, include emulators
            Step1();

            //step 2: list created emulators
            Step2();

            bool result;
            if (idDevices.Count == 0 || string.IsNullOrEmpty(TargetDevice))
            {
                if (TargetEmulator == null)
                {
                    Console.WriteLine("ERROR: Los API SDK de los dispositivos conectados y los emuladores" +
                        " creados no son apropiados para ejecutar la aplicación. " + 
                        "Cree un emulador nuevo con el API SDK >= " + APKTargetApi);
                    return;
                }

                //step 3: run emulator and wait for startup
                result = Step3();
                if (!result)
                {
                    Console.WriteLine("ERROR: El emulador " + TargetEmulator.Name + " no se ha podido cargar. Inténtelo otra vez.");
                    return;
                }
            }

            //step 4: install apk inside device
            result = Step4();
            if (!result)
            {
                Console.WriteLine("ERROR: No se ha podido instalar la aplicación en el emulador.");
                return;
            }

            //step 5: launch apk inside device
            result = Step5();
            if (!result)
            {
                Console.WriteLine("ERROR: La aplicación ha fallado durante su ejecución en el emulador.");
                return;
            }

            //step 6: get result of launch apk
            //Step6();

            Console.WriteLine("OK");
        }

        #region static methods that are steps of process
        // step 1: get devices plugged, include emulators
        private static void Step1()
        {
            HandleProcess proc = new HandleProcess(fiCommandADB.FullName, "devices");
            int result = proc.Start();

            string[] lines = null;
            if (proc.StandardOutput != null)
                lines = Regex.Split(proc.StandardOutput, "\r\n");

            idDevices.Clear();
            TargetDevice = "";
            bool nextItem = false;
            foreach (string line in lines)
            {
                if (nextItem)
                {
                    if (!string.IsNullOrEmpty(line) && line.Contains("\tdevice"))
                    {
                        int tab = line.IndexOf("\t");
                        string idDevice = line.Substring(0, tab);
                        idDevices.Add(idDevice);

                        string arguments = "-s " + idDevice + " shell getprop ro.build.version.sdk";
                        HandleProcess procAPI = new HandleProcess(fiCommandADB.FullName, arguments);
                        int resultAPI = procAPI.Start();

                        string[] linesAPI = null;
                        if (procAPI.StandardOutput != null)
                            linesAPI = Regex.Split(procAPI.StandardOutput, "\r\n");

                        // if sdk of current device is appropiate, save it
                        if (linesAPI != null)
                        {
                            int apiDevice = Convert.ToInt32(linesAPI[0]);
                            if (apiDevice >= APKTargetApi)
                                TargetDevice = idDevice;
                        }
                    }

                    //td: if (line.Contains("\toffline")) .... think to wait for theirs!!
                }
                else
                    if (line.Contains("List of devices attached"))
                    {
                        nextItem = true;
                    }
            }
        }

        //step 2: list created emulators
        private static void Step2()
        {
            HandleProcess proc = new HandleProcess(fiCommandAndroid.FullName, "list avd");
            int result = proc.Start();

            string[] lines = Regex.Split(proc.StandardOutput, "\r\n");

            Emulator emulator = null;
            emulators.Clear();
            foreach (string line in lines)
            {
                if (line.Contains("Name: "))
                {
                    emulator = new Emulator();
                    emulator.Name = line.Substring(10);
                }
                else
                    if (line.Contains("Target: ") && emulator != null)
                    {
                        emulator.Target = line.Substring(10);

                        int lastSpace = emulator.Target.LastIndexOf(" ");
                        int closeParenthesis = emulator.Target.LastIndexOf(")");
                        string apiLevel = emulator.Target.Substring(lastSpace + 1, closeParenthesis - lastSpace - 1);
                        emulator.apiLevel = Convert.ToInt32(apiLevel);

                        emulators.Add(emulator);
                        emulator = null;
                    }
            }

            //find emulator with the appropiate api level
            foreach (Emulator emu in emulators)
            {
                if (emu.apiLevel >= APKTargetApi)
                {
                    TargetEmulator = emu;
                    break;
                }
            }
        }

        //step 3: run emulator and wait for startup
        private static bool Step3()
        {
            bool resultState = true;
            string arguments = "-avd " + TargetEmulator.Name;
            HandleProcess proc = new HandleProcess(fiCommandEmulator.FullName, arguments);
            int result = proc.Start();

            //td: get current time and save inside some var
            Thread.Sleep(3000);

            string[] lines = null;
            if (proc.StandardOutput != null)
                lines = Regex.Split(proc.StandardOutput, "\r\n");

            while (string.IsNullOrEmpty(TargetDevice))
            {
                Step1();
                Thread.Sleep(1000);

                //td: get current time and verify y diff is greater of MAX_SLEEP_TIME and then break while
                //    and set 
                //resultState = false;
            }

            return resultState;
        }

        //step 4: install apk inside device
        private static bool Step4()
        {
            string arguments = "-s " + TargetDevice + " install -r " + APKFilename;
            HandleProcess proc = new HandleProcess(fiCommandADB.FullName, arguments, true, -1);

            do
            {
                Thread.Sleep(3000);
                int result = proc.Start();
            } while (proc.StandardOutput != null && proc.StandardOutput.Contains("Error: Could not access the Package Manager"));

            if (proc.StandardOutput != null && proc.StandardOutput.Contains("Success"))
                return true;

            return false;
        }

        //step 5: launch apk inside device
        private static bool Step5()
        {
            string arguments = "-s " + TargetDevice + " shell am start -n " + APKPackageName + "/." + APKActivityName;
            HandleProcess proc = new HandleProcess(fiCommandADB.FullName, arguments);
            int result = proc.Start();

            if (proc.StandardOutput == null || proc.StandardOutput.Contains("Error"))
                return false;

            return true;
        }

        //step 6: get result of launch apk
        private static void Step6()
        {
            HandleProcess proc = new HandleProcess(fiCommandADB.FullName, "logcat");
            int result = proc.Start();

            string[] lines = null;
            if (proc.StandardOutput != null)
                lines = Regex.Split(proc.StandardOutput, "\r\n");

            //td: what can i do with this output??
        }
        #endregion
    }

}
