using System;
using System.ComponentModel.Design;

namespace Excess.Project
{
    /// <summary>
    /// CommandIDs matching the commands defined symbols in PkgCmd.vsct
    /// </summary>
    public sealed class ExcessMenus
    {
        internal static readonly Guid guidExcessProjectCmdSet = new Guid(GuidList.guidExcessProjectCmdSetString);
        internal static readonly CommandID SetAsMain = new CommandID(guidExcessProjectCmdSet, 0x3001);
    }
}

