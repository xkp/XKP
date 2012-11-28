using System;
using System.Diagnostics;
using System.IO;
using System.Collections.Generic;
using System.Globalization;
using System.Reflection;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;
using Microsoft.Build.BuildEngine;
using ExcessCompiler;
using System.Xml.Linq;
using Newtonsoft.Json;
using Microsoft.Win32;
using System.Windows.Forms;

namespace Excess.CompilerTasks
{   
	/////////////////////////////////////////////////////////////////////////////
	// My MSBuild Task  
    public class Log
    {       
        public string text { get; set; }
        public string type { get; set; }
    }
	public class ExcessCompilerTask : Task
	{
		#region Constructors
		/// <summary>
		/// Constructor. This is the constructor that will be used
		/// when the task run.
		/// </summary>
		public ExcessCompilerTask()
		{
		}
		#endregion

		#region Public Properties and related Fields
		private string[] sourceFiles;
		/// <summary>
		/// List of Python source files that should be compiled into the assembly
		/// </summary>
		[Required()]
		public string[] SourceFiles
		{
			get { return sourceFiles; }
			set { sourceFiles = value; }
		}

		private string outputAssembly;
		/// <summary>
		/// Output Assembly (including extension)
		/// </summary>
		[Required()]
		public string OutputAssembly
		{
			get { return outputAssembly; }
			set { outputAssembly = value; }
		}

		private ITaskItem[] referencedAssemblies = new ITaskItem[0];
		/// <summary>
		/// List of dependent assemblies
		/// </summary>
		public ITaskItem[] ReferencedAssemblies
		{
			get { return referencedAssemblies; }
			set
			{
				if (value != null)
				{
					referencedAssemblies = value;
				}
				else
				{
					referencedAssemblies = new ITaskItem[0];
				}

			}
		}

		private ITaskItem[] resourceFiles = new ITaskItem[0];
		/// <summary>
		/// List of resource files
		/// </summary>
		public ITaskItem[] ResourceFiles
		{
			get { return resourceFiles; }
			set
			{
				if (value != null)
				{
					resourceFiles = value;
				}
				else
				{
					resourceFiles = new ITaskItem[0];
				}

			}
		}

		private string mainFile;
		/// <summary>
		/// For applications, which file is the entry point
		/// </summary>
		[Required()]
		public string MainFile
		{
			get { return mainFile; }
			set { mainFile = value; }
		}

		private string targetKind;
		/// <summary>
		/// Target type (exe, winexe, library)
		/// These will be mapped to System.Reflection.Emit.PEFileKinds
		/// </summary>
		public string TargetKind
		{
			get { return targetKind; }
			set { targetKind = value.ToLower(CultureInfo.InvariantCulture); }
		}
		private bool debugSymbols = true;
		/// <summary>
		/// Generate debug information
		/// </summary>
		public bool DebugSymbols
		{
			get { return debugSymbols; }
			set { debugSymbols = value; }
		}
		private string projectPath = null;
		/// <summary>
		/// This should be set to $(MSBuildProjectDirectory)
		/// </summary>
		public string ProjectPath
		{
			get { return projectPath; }
			set { projectPath = value; }
		}

		private bool useExperimentalCompiler;
		/// <summary>
		/// This property is only needed because Iron Python does not officially support building real .Net assemblies.
		/// For WAP scenarios, we need to support real assemblies and as such we use an alternate approach to build those assemblies.
		/// </summary>
		public bool UseExperimentalCompiler
		{
			get { return useExperimentalCompiler; }
			set { useExperimentalCompiler = value; }
		}
	
		#endregion

		/// <summary>
		/// Main entry point for the task
		/// </summary>
		/// <returns></returns>
		public override bool Execute()
		{
            bool success = false;
            try
            {
                string filePath = Path.Combine(projectPath, MainFile);
                XAttribute version = XElement.Load(filePath).Attribute("version");
                bool old_version = version != null && version.Value == "0.9.4";
                string installPath = (string)Registry.GetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Excess", @"InstallPath", @"");
                if (installPath == null || installPath == String.Empty)
                    installPath = (string)Registry.GetValue(@"HKEY_CURRENT_USER\SOFTWARE\Excess", @"InstallPath", @"");    

                if (old_version)
                {
                    string execPath = installPath + @"\" + version.Value + @"\";

                    if (!Directory.Exists(installPath))
                    {
                        MessageBox.Show("Excess IDE Installed Path", "The path specified inside InstallPath registry key is not exists.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return success;
                    }

                    if (!File.Exists(execPath + "xss.exe"))
                    {
                        MessageBox.Show("Excess IDE Compiler", "The xss.exe don't exists inside installed path.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return success;
                    }
                    
                    //call xss.exe
                    Process proc = new Process();
                    proc.StartInfo = new ProcessStartInfo(execPath + "xss.exe");
                    proc.StartInfo.Arguments = "\"" + filePath + "\" json";
                    proc.StartInfo.RedirectStandardOutput = true;
                    proc.StartInfo.UseShellExecute = false;

                    bool success_ = proc.Start();

                    string output_string = proc.StandardOutput.ReadToEnd();

                    List<Log> logs = JsonConvert.DeserializeObject<List<Log>>(output_string);

                    foreach (Log l in logs)
                    {
                        if (l.type == "error")
                        {
                            Log.LogError(l.text);
                            success_ = false;
                        }
                        if (l.type == "msg")
                            Log.LogMessage(l.text);
                        if (l.type == "log")
                            Log.LogMessage(l.text);
                    }

                    return success_;
                }

                ExcessModelService service = ExcessModelService.getInstance();
                List<ExcessErrorInfo> errors = new List<ExcessErrorInfo>();
                success = service.Model.buildProject(filePath, errors);

                if (errors.Count == 0)
                {
                    Engine engine = new Engine();
                    engine.DefaultToolsVersion = "4.0";

                    // Instantiate a new FileLogger to generate build log
                    myLogger logger = new myLogger(Log);

                    // td: change location of logs to Personal Folders location

                    // Set the logfile parameter to indicate the log destination
                    logger.Parameters = @"logfile=" + installPath + @"logs\build.log";

                    // Register the logger with the engine
                    engine.RegisterLogger(logger);

                    // Build a project file
                    string appName = service.Model.getAppName(filePath);
                    string slnPath = Path.Combine(projectPath, @"bin\debug\" + appName + ".sln");
                    try
                    {
                        success = engine.BuildProjectFile(slnPath);
                    }
                    catch (Exception e)
                    {
                        success = false;
                    }

                    //Unregister all loggers to close the log file
                    engine.UnregisterAllLoggers();

                    if (success)
                        Console.WriteLine("Build succeeded.");
                    else
                        Console.WriteLine(@"Build failed. View " + installPath + @"logs\build.log for details");

                    return true;
                }
            }
            catch (Exception e)
            {
                success = false;
                Log.LogError("Build crash \n" + e.ToString());
            }


            //foreach (ExcessErrorInfo error in errors)
            //{
            //    Log.LogError("", "", "", error.File, error.BeginLine, error.BeginColumn, error.BeginLine, error.BeginColumn + 1, error.desc);
            //}

            return success;
		}

		/// <summary>
		/// Initialize compiler options based on task parameters
		/// </summary>
		/// <returns>false if failed</returns>
		private bool InitializeCompiler()
		{
			return true;
		}
	}

    internal class myLogger : Logger
    {
        public myLogger(TaskLoggingHelper log)
        {
            log_ = log;
        }

        public override void Initialize(Microsoft.Build.Framework.IEventSource eventSource)
        {
            ////Register for the ProjectStarted, TargetStarted, and ProjectFinished events
            //eventSource. ProjectStarted += new ProjectStartedEventHandler(eventSource_ProjectStarted);
            //eventSource.TargetStarted += new TargetStartedEventHandler(eventSource_TargetStarted);
            eventSource.ErrorRaised += new BuildErrorEventHandler(ErrorHandler);
        }

        private void ErrorHandler(object sender, BuildErrorEventArgs e)
        { 
            //td: !! send the errors back to xs
            log_.LogError(e.Subcategory, e.Code, e.HelpKeyword, e.File, e.LineNumber, e.ColumnNumber, e.EndLineNumber, e.EndColumnNumber, e.Message);
        }

        private TaskLoggingHelper log_;
    }
}