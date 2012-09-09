using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Excess.EditorExtensions
{
    internal static class Constants
    {
        /// <summary>
        /// The Guid of the IPy language service
        /// </summary>
        internal static readonly Guid ExcessLanguageServiceGuid = new Guid("{B1E1395C-71CE-489E-A7EE-BC0A25935F6E}");

        /// <summary>
        /// Word separators chars
        /// </summary>
        internal static char[] Separators = new[] { '\n', '\r', '\t', ' ', ':', '(', ')', '[', ']', '{', '}', '?', '/', '+', '-', ';', '=', '*', '!', ',', '<', '>' };

        /// <summary>
        /// Word separators chars including dot
        /// </summary>
        internal static char[] SeparatorsPlusDot = Separators.Union(new[] {'.'}).ToArray();
    }
}
