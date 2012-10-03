using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Project;
using Excess.Project.WPFProviders;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System.ComponentModel.Design;
using Microsoft.VisualStudio.Package;
using EnvDTE;
using Excess.Project.Library;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.TextManager.Interop;
using System.Collections.Generic;
using Excess.CompilerTasks;
using Microsoft.VisualStudio.Text.Editor;
using Microsoft.VisualStudio.Editor;
using Excess.EditorExtensions;

namespace Excess.Project
{
	//Set the projectsTemplatesDirectory to a non-existant path to prevent VS from including the working directory as a valid template path
    [ProvideProjectFactory(typeof(ExcessProjectFactory), "Excess", "Excess Project Files (*.xsproj);*.xsproj", "xsproj", "xsproj", ".\\NullPath", LanguageVsTemplate = "Excess")]
	//Register the WPF Python Factory
    [ProvideProjectFactory(typeof(PythonWPFProjectFactory), null, null, null, null, null, LanguageVsTemplate = "Excess", TemplateGroupIDsVsTemplate = "WPF", ShowOnlySpecifiedTemplatesVsTemplate = false)]
	[SingleFileGeneratorSupportRegistrationAttribute(typeof(ExcessProjectFactory))]
    //[WebSiteProject("IronPython", "Iron Python")]
    [ProvideObject(typeof(GeneralPropertyPage))]
	[ProvideObject(typeof(ExcessBuildPropertyPage))]
	[ProvideMenuResource(1000, 1)]
    [ProvideEditorExtensionAttribute(typeof(EditorFactory), ".py", 32)]
    [ProvideEditorLogicalView(typeof(EditorFactory), "{7651a702-06e5-11d1-8ebd-00a0c90f26ea}")]  //LOGVIEWID_Designer
    [ProvideEditorLogicalView(typeof(EditorFactory), "{7651a701-06e5-11d1-8ebd-00a0c90f26ea}")]  //LOGVIEWID_Code
	[PackageRegistration(UseManagedResourcesOnly = true)]
	[Guid(GuidList.guidExcessProjectPkgString)]
	//The following attributes are specific to sunpporting Web Application Projects
    //[WAProvideProjectFactory(typeof(WAExcessProjectFactory), "IronPython Web Application Project Templates", "IronPython", false, "Web", null)]
    //[WAProvideProjectFactoryTemplateMapping("{" + GuidList.guidExcessProjectFactoryString + "}", typeof(WAExcessProjectFactory))]
    //[WAProvideLanguageProperty(typeof(WAExcessProjectFactory), "CodeFileExtension", ".py")]
    //[WAProvideLanguageProperty(typeof(WAExcessProjectFactory), "TemplateFolder", "IronPython")]
	// IronPython does not need a CodeBehindCodeGenerator because all the code should be inline, so we disable
	// it setting a null GUID for the class that is supposed to implement it.
	[WAProvideLanguageProperty(typeof(WAExcessProjectFactory), "CodeBehindCodeGenerator", "{00000000-0000-0000-0000-000000000000}")]
    //[ProvideMSBuildTargets("IronPythonCompilerTasks", @"$PackageFolder$\IronPython.targets")]
    [ProvideBindingPathAttribute]
    //td
    //[RegisterSnippetsAttribute(GuidList.guidExcessLanguageString, false, 131,
    //"Excess", @"CodeSnippets\SnippetsIndex.xml", @"CodeSnippets\Snippets\", @"CodeSnippets\Snippets\")]
    [ProvideLanguageService(GuidList.guidExcessLanguageString, "Excess", 101, RequestStockColors = true)]
    [ProvideLanguageExtension(GuidList.guidExcessLanguageString, ".xs")]
    public class PythonProjectPackage : ProjectPackage, IVsInstalledProduct, IOleComponent, IVsRunningDocTableEvents, IVsTextLinesEvents
	{
        private PythonLibraryManager libraryManager;
        private uint componentID;
        private ProjectItemsEvents projectEvents;
        private uint _rdtEventCookie;
        private Dictionary<uint, TextLineEventListener> documents = new Dictionary<uint, TextLineEventListener>();


		protected override void Initialize()
		{
            DTE dte = (DTE)GetService(typeof(DTE));
            if (dte != null)
            {
                base.Initialize();
                this.RegisterProjectFactory(new ExcessProjectFactory(this));
                this.RegisterProjectFactory(new PythonWPFProjectFactory(this as System.IServiceProvider));
                this.RegisterEditorFactory(new EditorFactory(this));
            }

            //td: this library manager makes some sort of sense, review later
            //IServiceContainer container = this as IServiceContainer;
            //container.AddService(typeof(IPythonLibraryManager), CreateService, true);

            //we'll do the events manually
            IVsRunningDocumentTable rdt = GetService(typeof(SVsRunningDocumentTable)) as IVsRunningDocumentTable;
            if (rdt != null)
            {
                rdt.AdviseRunningDocTableEvents((IVsRunningDocTableEvents)this, out this._rdtEventCookie);
            }

            RegisterForIdleTime();
        }

        private object CreateService(IServiceContainer container, Type serviceType)
        {
            object service = null;
            //if(typeof(IPythonLibraryManager) == serviceType)
            //{
            //    libraryManager = new PythonLibraryManager(this);
            //    service = libraryManager as IPythonLibraryManager;
            //    RegisterForIdleTime();
            //}
            return service;
        }

        private void RegisterForIdleTime()
        {
            IOleComponentManager mgr = GetService(typeof(SOleComponentManager)) as IOleComponentManager;
            if (componentID == 0 && mgr != null)
            {
                OLECRINFO[] crinfo = new OLECRINFO[1];
                crinfo[0].cbSize = (uint)Marshal.SizeOf(typeof(OLECRINFO));
                crinfo[0].grfcrf = (uint)_OLECRF.olecrfNeedIdleTime |
                                              (uint)_OLECRF.olecrfNeedPeriodicIdleTime;
                crinfo[0].grfcadvf = (uint)_OLECADVF.olecadvfModal |
                                              (uint)_OLECADVF.olecadvfRedrawOff |
                                              (uint)_OLECADVF.olecadvfWarningsOff;
                crinfo[0].uIdleTimeInterval = 1000;
                int hr = mgr.FRegisterComponent(this, crinfo, out componentID);
            }
        }

        protected override void Dispose(bool disposing)
        {
            try
            {
                if (componentID != 0)
                {
                    IOleComponentManager mgr = GetService(typeof(SOleComponentManager)) as IOleComponentManager;
                    if (mgr != null)
                    {
                        mgr.FRevokeComponent(componentID);
                    }
                    componentID = 0;
                }
                if (null != libraryManager)
                {
                    libraryManager.Dispose();
                    libraryManager = null;
                }

                foreach (TextLineEventListener textListener in documents.Values)
                {
                    textListener.Dispose();
                }
                documents.Clear();
            }
            finally
            {
                base.Dispose(disposing);
            }
        }


        #region IVsInstalledProduct Members
		/// <summary>
		/// This method is called during Devenv /Setup to get the bitmap to
		/// display on the splash screen for this package.
		/// </summary>
		/// <param name="pIdBmp">The resource id corresponding to the bitmap to display on the splash screen</param>
		/// <returns>HRESULT, indicating success or failure</returns>
		public int IdBmpSplash(out uint pIdBmp)
		{
			pIdBmp = 300;

			return VSConstants.S_OK;
		}

		/// <summary>
		/// This method is called to get the icon that will be displayed in the
		/// Help About dialog when this package is selected.
		/// </summary>
		/// <param name="pIdIco">The resource id corresponding to the icon to display on the Help About dialog</param>
		/// <returns>HRESULT, indicating success or failure</returns>
		public int IdIcoLogoForAboutbox(out uint pIdIco)
		{
			pIdIco = 400;

			return VSConstants.S_OK;
		}

		/// <summary>
		/// This methods provides the product official name, it will be
		/// displayed in the help about dialog.
		/// </summary>
		/// <param name="pbstrName">Out parameter to which to assign the product name</param>
		/// <returns>HRESULT, indicating success or failure</returns>
		public int OfficialName(out string pbstrName)
		{
			pbstrName = GetResourceString("@ProductName");
			return VSConstants.S_OK;
		}

		/// <summary>
		/// This methods provides the product description, it will be
		/// displayed in the help about dialog.
		/// </summary>
		/// <param name="pbstrProductDetails">Out parameter to which to assign the description of the package</param>
		/// <returns>HRESULT, indicating success or failure</returns>
		public int ProductDetails(out string pbstrProductDetails)
		{
			pbstrProductDetails = GetResourceString("@ProductDetails");
			return VSConstants.S_OK;
		}

		/// <summary>
		/// This methods provides the product version, it will be
		/// displayed in the help about dialog.
		/// </summary>
		/// <param name="pbstrPID">Out parameter to which to assign the version number</param>
		/// <returns>HRESULT, indicating success or failure</returns>
		public int ProductID(out string pbstrPID)
		{
			pbstrPID = GetResourceString("@ProductID");
			return VSConstants.S_OK;
		}

		#endregion

		/// <summary>
		/// This method loads a localized string based on the specified resource.
		/// </summary>
		/// <param name="resourceName">Resource to load</param>
		/// <returns>String loaded for the specified resource</returns>
		public string GetResourceString(string resourceName)
		{
			string resourceValue;
			IVsResourceManager resourceManager = (IVsResourceManager)GetService(typeof(SVsResourceManager));
			if(resourceManager == null)
			{
				throw new InvalidOperationException("Could not get SVsResourceManager service. Make sure the package is Sited before calling this method");
			}
			Guid packageGuid = this.GetType().GUID;
			int hr = resourceManager.LoadResourceString(ref packageGuid, -1, resourceName, out resourceValue);
			Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(hr);
			return resourceValue;
		}

        public override string ProductUserContext
        {
            get 
            {
                throw new NotImplementedException(); 
            }
        }

        public int FContinueMessageLoop(uint uReason, IntPtr pvLoopData, MSG[] pMsgPeeked)
        {
            return 1;
        }

        public int FDoIdle(uint grfidlef)
        {
            ExcessModelService service = ExcessModelService.getInstance();
            bool changed = service.Model.updateChanges();

            if (changed && currentView_ != null)
            {
                ErrorListPresenter ep = currentView_.Properties.GetOrCreateSingletonProperty<ErrorListPresenter>(() => null);
                if (ep != null)
                    ep.CreateErrors();
            }

            return 0;
        }

        public int FPreTranslateMessage(MSG[] pMsg)
        {
            return 0;
        }

        public int FQueryTerminate(int fPromptUser)
        {
            return 1;
        }

        public int FReserved1(uint dwReserved, uint message, IntPtr wParam, IntPtr lParam)
        {
            return 1;
        }

        public IntPtr HwndGetWindow(uint dwWhich, uint dwReserved)
        {
            return IntPtr.Zero;
        }

        public void OnActivationChange(IOleComponent pic, int fSameComponent, OLECRINFO[] pcrinfo, int fHostIsActivating, OLECHOSTINFO[] pchostinfo, uint dwReserved)
        {
        }

        public void OnAppActivate(int fActive, uint dwOtherThreadID)
        {
        }

        public void OnEnterState(uint uStateID, int fEnter)
        {
        }

        public void OnLoseActivation()
        {
        }

        public void Terminate()
        {
        }

        //IVsRunningDocTableEvents
        public int OnAfterAttributeChange(uint docCookie, uint grfAttribs)
        {
            if ((grfAttribs & (uint)(__VSRDTATTRIB.RDTA_MkDocument)) == (uint)__VSRDTATTRIB.RDTA_MkDocument)
            {
                IVsRunningDocumentTable rdt = GetService(typeof(SVsRunningDocumentTable)) as IVsRunningDocumentTable;
                if (rdt != null)
                {
                    uint flags, readLocks, editLocks, itemid;
                    IVsHierarchy hier;
                    IntPtr docData = IntPtr.Zero;
                    string moniker;
                    int hr;
                    try
                    {
                        hr = rdt.GetDocumentInfo(docCookie, out flags, out readLocks, out editLocks, out moniker, out hier, out itemid, out docData);
                        TextLineEventListener listner;
                        if (documents.TryGetValue(docCookie, out listner))
                        {
                            listner.FileName = moniker;
                        }
                    }
                    finally
                    {
                        if (IntPtr.Zero != docData)
                        {
                            Marshal.Release(docData);
                        }
                    }
                }
            }
            return VSConstants.S_OK;
        }

        public int OnAfterFirstDocumentLock(uint docCookie, uint dwRDTLockType, uint dwReadLocksRemaining, uint dwEditLocksRemaining)
        {
            return VSConstants.S_OK;
        }

        public int OnAfterSave(uint docCookie)
        {
            return VSConstants.S_OK;
        }

        private IWpfTextView currentView_;
        public void OnViewChange(IVsWindowFrame pFrame)
        {
            IVsTextView tv = VsShellUtilities.GetTextView(pFrame);
            IWpfTextView view = null;
            IVsUserData userData = tv as IVsUserData;

            if (null != userData)
            {
                IWpfTextViewHost viewHost;
                object holder;
                Guid guidViewHost = DefGuidList.guidIWpfTextViewHost;

                try
                {
                    userData.GetData(ref guidViewHost, out holder);
                    viewHost = (IWpfTextViewHost)holder;
                    view = viewHost.TextView;
                }
                catch (Exception e)
                {
                    view = null;
                }
            }

            currentView_ = view;
        }

        public int OnBeforeDocumentWindowShow(uint docCookie, int fFirstShow, IVsWindowFrame pFrame)
        {
            OnViewChange(pFrame);
            
            if (documents.ContainsKey(docCookie))
            {
                return VSConstants.S_OK;
            }
            // Get the information about this document from the RDT.
            IVsRunningDocumentTable rdt = GetService(typeof(SVsRunningDocumentTable)) as IVsRunningDocumentTable;
            if (null != rdt)
            {
                // Note that here we don't want to throw in case of error.
                uint flags;
                uint readLocks;
                uint writeLoks;
                string documentMoniker;
                IVsHierarchy hierarchy;
                uint itemId;
                IntPtr unkDocData;
                int hr = rdt.GetDocumentInfo(docCookie, out flags, out readLocks, out writeLoks,
                                             out documentMoniker, out hierarchy, out itemId, out unkDocData);
                try
                {
                    if (Microsoft.VisualStudio.ErrorHandler.Failed(hr) || (IntPtr.Zero == unkDocData))
                    {
                        return VSConstants.S_OK;
                    }
                    // Check the extension of the file to see if a listener is required.
                    string extension = System.IO.Path.GetExtension(documentMoniker);
                    if (0 != string.Compare(extension, ExcessConstants.xsFileExtension, StringComparison.OrdinalIgnoreCase))
                    {
                        return VSConstants.S_OK;
                    }

                    // Create the module id for this document.
                    ModuleId docId = new ModuleId(hierarchy, itemId);

                    // Try to get the text buffer.
                    IVsTextLines buffer = Marshal.GetObjectForIUnknown(unkDocData) as IVsTextLines;

                    // Create the listener.
                    TextLineEventListener listener = new TextLineEventListener(buffer, documentMoniker, docId);
                    // Add the listener to the dictionary, so we will not create it anymore.
                    documents.Add(docCookie, listener);
                }
                finally
                {
                    if (IntPtr.Zero != unkDocData)
                    {
                        Marshal.Release(unkDocData);
                    }
                }
            }
            // Always return success.
            return VSConstants.S_OK;
        }

        public int OnAfterDocumentWindowHide(uint docCookie, IVsWindowFrame pFrame)
        {
            return VSConstants.S_OK;
        }

        public int OnBeforeLastDocumentUnlock(uint docCookie, uint dwRDTLockType, uint dwReadLocksRemaining, uint dwEditLocksRemaining)
        {
            return VSConstants.S_OK;
        }

        //
        public void OnChangeLineAttributes(int iFirstLine, int iLastLine)
        {
        }

        public void OnChangeLineText(TextLineChange[] pTextLineChange, int fLast)
        {
            int ii = 0;
            ii++;
        }
    }
}
