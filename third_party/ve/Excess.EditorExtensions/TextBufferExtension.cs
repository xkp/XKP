﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Text;

namespace Excess.EditorExtensions
{
    /// <summary>
    /// Provides extensions for <see cref="ITextBuffer"/>
    /// </summary>
    internal static class ITextBufferExtension
    {
        /// <summary>
        /// Returns the filename of the text buffer
        /// </summary>
        /// <param name="textBuffer"></param>
        /// <returns></returns>
        internal static string GetFileName(this ITextBuffer textBuffer)
        {
            if (textBuffer.Properties.ContainsProperty(typeof(ITextDocument)))
            {
                return textBuffer.Properties.GetProperty<ITextDocument>(typeof(ITextDocument)).FilePath;
            }

            return string.Empty;
        }
    }
}