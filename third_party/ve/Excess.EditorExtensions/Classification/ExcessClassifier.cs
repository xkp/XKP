using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Text;
using System.Diagnostics;
using ExcessCompiler;

namespace Excess.EditorExtensions
{
    /// <summary>
    /// Implements <see cref="IClassifier"/> in order to provide coloring
    /// </summary>
    internal class ExcessClassifier : IClassifier
    {
        public event EventHandler<ClassificationChangedEventArgs> ClassificationChanged;
        private IClassificationTypeRegistryService classificationRegistryService;
        private ITextBuffer textBuffer;
        private ExcessCompiler.Compiler compiler_;

        internal ExcessClassifier(ITextBuffer textBuffer, IClassificationTypeRegistryService classificationRegistryService)
        {
            this.compiler_ = new ExcessCompiler.Compiler();
            this.textBuffer = textBuffer;
            this.classificationRegistryService = classificationRegistryService;
           
            this.textBuffer.ReadOnlyRegionsChanged += new EventHandler<SnapshotSpanEventArgs>(textBuffer_ReadOnlyRegionsChanged);
        }

        void textBuffer_ReadOnlyRegionsChanged(object sender, SnapshotSpanEventArgs e)
        {
            // We need to call this event when read-only regions are added, so they will be grayed out.
            OnClassificationChanged(new SnapshotSpan(textBuffer.CurrentSnapshot, e.Span));
        }

        private void OnClassificationChanged(SnapshotSpan changeSpan)
        {
            if (ClassificationChanged != null)
            {
                ClassificationChanged(this, new ClassificationChangedEventArgs(changeSpan));
            }
        }

        IList<ClassificationSpan> IClassifier.GetClassificationSpans(SnapshotSpan span)
        {
            var classifications = new List<ClassificationSpan>();
            Tokenizer tokenizer = compiler_.CreateTokenizer(span.GetText());

            var token = tokenizer.Next();
            int startIndex, endIndex;

            while (token != null)
            {
                TokenType type = token.classify();
                // Determine the bounds of the classfication span
                startIndex = span.Snapshot.GetLineFromLineNumber(token.StartLine - 1 + span.Start.GetContainingLine().LineNumber).Start.Position + token.StartColumn;
                endIndex = span.Snapshot.GetLineFromLineNumber(token.EndLine - 1 + span.Start.GetContainingLine().LineNumber).Start.Position + token.EndColumn;

                if (endIndex > span.Snapshot.GetText().Length)
                    endIndex = span.Snapshot.GetText().Length;

                if (endIndex > startIndex && !span.Snapshot.TextBuffer.IsReadOnly(new Span(startIndex, endIndex - startIndex)))
                {
                    // Add the classfication span
                    classifications.Add(new ClassificationSpan(new SnapshotSpan(span.Snapshot, startIndex, endIndex - startIndex), GetClassificationType(type)));
                }
                token = tokenizer.Next();
            }
            
            return classifications;
        }

        private IClassificationType GetClassificationType(TokenType token)
        {
            int itype = (int)token;

            string[] classTypes = new string[] 
            { 
		        ExcessClassificationTypes.Keyword,      //Keyword
		        ExcessClassificationTypes.String,       //CharLiteral,
		        ExcessClassificationTypes.Number,       //Number,
		        ExcessClassificationTypes.Identifier,   //Identifier
		        ExcessClassificationTypes.Delimiter,    //Delimiter
		        ExcessClassificationTypes.Comment,      //Comment
		        ExcessClassificationTypes.Operator,     //Operator 
		        ExcessClassificationTypes.Unknown,      //Unknown
            }; 

            return classificationRegistryService.GetClassificationType(classTypes[itype]);
        }
    }
}