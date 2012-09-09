﻿using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Project;
using IOleServiceProvider = Microsoft.VisualStudio.OLE.Interop.IServiceProvider;

namespace Excess.Project
{
	/// <summary>
	/// Creates Python Projects
	/// </summary>
	[Guid(GuidList.guidExcessProjectFactoryString)]
	public class ExcessProjectFactory : ProjectFactory
	{
		#region ctor
		/// <summary>
		/// Constructor for PythonProjectFactory
		/// </summary>
		/// <param name="package">the package who created this object</param>
		public ExcessProjectFactory(PythonProjectPackage package)
			: base(package)
		{
		}
		#endregion

		#region overridden methods
		/// <summary>
		/// Creates the Python Project node
		/// </summary>
		/// <returns>the new instance of the Python Project node</returns>
		protected override ProjectNode CreateProject()
		{
			PythonProjectNode project = new PythonProjectNode(this.Package as PythonProjectPackage);
			project.SetSite((IOleServiceProvider)((IServiceProvider)this.Package).GetService(typeof(IOleServiceProvider)));
			return project;
		}
		#endregion
	}

    //td: ?
	/// <summary>
	/// This class is a 'fake' project factory that is used by WAP to register WAP specific information about
	/// IronPython projects.
	/// </summary>
	[GuidAttribute("0C1E5196-4828-499e-9F72-98268B955B28")]
	public class WAExcessProjectFactory { }


}
