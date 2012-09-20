using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using Microsoft.VisualStudio.Language.Intellisense;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.TextManager.Interop;
using ExcessCompiler;
using Excess.CompilerTasks;

namespace Excess.EditorExtensions
{
    /// <summary>
    /// Implementation of <see cref="ICompletionSource"/>. Provides the completion sets for the editor. 
    /// </summary>
    internal class CompletionSource : ICompletionSource
    {
        internal static string CompletionSetName = "Excess Completion";

        private ITextBuffer textBuffer;
        private IGlyphService glyphService;
        private IServiceProvider serviceProvider;

        internal CompletionSource(ITextBuffer textBuffer, IGlyphService glyphService, IServiceProvider serviceProvider)
        {
            this.textBuffer = textBuffer;
            this.glyphService = glyphService;
            this.serviceProvider = serviceProvider;
        }

        #region ICompletionSource Members

        void ICompletionSource.AugmentCompletionSession(ICompletionSession session, IList<CompletionSet> completionSets)
        {
            int position = session.GetTriggerPoint(session.TextView.TextBuffer).GetPosition(textBuffer.CurrentSnapshot);
            int line = textBuffer.CurrentSnapshot.GetLineNumberFromPosition(position);
            ITextSnapshotLine line_pos = textBuffer.CurrentSnapshot.GetLineFromPosition(position);
            
            int column = position - textBuffer.CurrentSnapshot.GetLineFromPosition(position).Start.Position;
            
            string LineContents = line_pos.GetText().Substring(0, column);
            ExcessModelService service = ExcessModelService.getInstance();
            List<ExcessCompletionItem> items = service.Model.getCompletion(textBuffer.GetFileName(), LineContents, line, column);

            completionSets.Add(GetCompletions(items, session));
        }

        //private IList<Declaration> GetEngineAttributes(string lineText, int column)
        //{
        //    var declarations = new List<Declaration>();

        //    int start = lineText.Substring(0, column).LastIndexOfAny(Constants.Separators) + 1;

        //    // Now build the string to pass to the engine.
        //    string engineCommand = string.Format(
        //        System.Globalization.CultureInfo.InvariantCulture,
        //        "dir({0})",
        //        lineText.Substring(start, column - start));

        //    try
        //    {
        //        var members = Engine.Evaluate(engineCommand) as IEnumerable;
        //        if (null != members)
        //        {
        //            foreach (string member in members)
        //            {
        //                declarations.Add(new Declaration(member));
        //            }
        //        }
        //    }
        //    catch
        //    {
        //        // Do nothing => Return the empty declarations list
        //    }

        //    return declarations;
        //}

        //private IEngine engine;
        //private IEngine Engine
        //{
        //    get
        //    {
        //        if (null == engine)
        //        {
        //            var provider = (IPythonEngineProvider)serviceProvider.GetService(typeof(IPythonEngineProvider));
        //            engine = provider.GetSharedEngine();
        //        }

        //        return engine;
        //    }
        //}


        /// <summary>
        /// Get a piece of text of a given line in a text buffer
        /// </summary>
        public string TextOfLine(ITextBuffer textBuffer, int lineNumber, int endColumn, out int start, bool skipReadOnly)
        {
            start = 0;
            var line = textBuffer.CurrentSnapshot.GetLineFromLineNumber(lineNumber);

            if (textBuffer.IsReadOnly(line.Extent.Span))
            {
                start = GetReadOnlyLength(textBuffer.CurrentSnapshot) - line.Start;
            }

            return line.GetText().Substring(start, endColumn - start);
        }

        private int GetReadOnlyLength(ITextSnapshot textSnapshot)
        {
            return textSnapshot.TextBuffer.GetReadOnlyExtents(new Span(0, textSnapshot.Length)).Max(region => region.End);
        }

        private CompletionSet GetCompletions(List<ExcessCompletionItem> items, ICompletionSession session)
        {
            // Add IPy completion
            var completions = new List<Completion>();
            completions.AddRange(items.Select(item => new CompletionItem(item, glyphService)));

            if (completions.Count > 0)
            {
                //td:
                //// Add Snippets entries
                //var expansionManager = (IVsTextManager2)this.serviceProvider.GetService(typeof(SVsTextManager));
                //var snippetsEnumerator = new SnippetsEnumerator(expansionManager, Constants.IronPythonLanguageServiceGuid);
                //completions.AddRange(snippetsEnumerator.Select(expansion => new PyCompletion(expansion, glyphService)));
            }

            return
                new CompletionSet("xsCompletion",
                    "Excess Completion",
                    CreateTrackingSpan(session.GetTriggerPoint(session.TextView.TextBuffer).GetPosition(textBuffer.CurrentSnapshot)),
                    completions,
                    null)
            ;
        }

        private ITrackingSpan CreateTrackingSpan(int position)
        {
            char[] separators = new[] { '\n', '\r', '\t', ' ', '.', ':', '(', ')', '[', ']', '{', '}', '?', '/', '+', '-', ';', '=', '*', '!', ',', '<', '>' };

            string text = textBuffer.CurrentSnapshot.GetText();
            int last = text.Substring(position).IndexOfAny(separators);
            int first = text.Substring(0, position).LastIndexOfAny(separators) + 1;

            if (last == -1)
                last = text.Length - position;

            return textBuffer.CurrentSnapshot.CreateTrackingSpan(new Span(first, (last + position) - first), SpanTrackingMode.EdgeInclusive);
        }

        #endregion

        public void Dispose()
        { }
    }
}