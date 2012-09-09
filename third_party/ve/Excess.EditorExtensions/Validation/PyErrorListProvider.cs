﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Text;

namespace Excess.EditorExtensions
{
    /// <summary>
    /// Provides errors that should be added to the error list
    /// </summary>
    internal class PyErrorListProvider
    {
        /// <summary>
        /// Gets the errors of the text buffer
        /// </summary>
        /// <param name="textBuffer"></param>
        /// <returns></returns>
        internal IList<ValidationError> GetErrors(ITextBuffer textBuffer)
        {
            //td:
            //var sink = new PyErrorListCompilerSink(textBuffer);
            //var modules = new Microsoft.VisualStudio.IronPythonInference.Modules();
            //modules.AnalyzeModule(sink, textBuffer.GetFileName(), textBuffer.CurrentSnapshot.GetText());

            //return sink.Errors.ToList();
            return null;
        }
    }
}
