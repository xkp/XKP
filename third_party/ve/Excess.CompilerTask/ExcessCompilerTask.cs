using System;
using System.IO;
using System.Collections.Generic;
using System.Globalization;
using System.Reflection;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;

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
			Log.LogMessage(MessageImportance.Normal, "Excess Compilation Task");

			return false;
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
}