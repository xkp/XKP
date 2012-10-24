using System;
using System.IO;
using System.Collections.Generic;
using System.Globalization;
using System.Reflection;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;
using Microsoft.Build.BuildEngine;
using ExcessCompiler;

namespace Excess.CompilerTasks
{
	/////////////////////////////////////////////////////////////////////////////
	// My MSBuild Task
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
            string filePath = Path.Combine(projectPath, MainFile);

            ExcessModelService service = ExcessModelService.getInstance();
            List<ExcessErrorInfo> errors = new List<ExcessErrorInfo>();
            bool success = service.Model.buildProject(filePath, errors);
            if (errors.Count == 0)
            {
                Engine engine = new Engine();
                engine.DefaultToolsVersion = "4.0";

                // Instantiate a new FileLogger to generate build log
                myLogger logger = new myLogger(Log);

                // Set the logfile parameter to indicate the log destination
                logger.Parameters = @"logfile=C:\dev\XKP_BIN\build.log";

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
                    Console.WriteLine(@"Build failed. View C:\temp\build.log for details");
                
                return true;
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