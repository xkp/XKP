using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Utilities;
using System.ComponentModel.Composition;
using Microsoft.VisualStudio.Text.Classification;
using System.Windows.Media;

namespace Excess.EditorExtensions
{
    /// <summary>
    /// Classification definitions
    /// </summary>
    internal class ExcessClassificationDefinitions
    {
        [Name(ExcessClassificationTypes.Comment), Export]
        internal ClassificationTypeDefinition CommentClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Comment)]
        [Name("PythonCommentFormatDefinition")]
        [Order]
        internal sealed class CommentClassificationFormat : ClassificationFormatDefinition
        {
            internal CommentClassificationFormat()
            {
                ForegroundColor = Colors.Green;
                this.DisplayName = "Python Comment";
            }
        }

        [Name(ExcessClassificationTypes.Delimiter), Export]
        internal ClassificationTypeDefinition DelimiterClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Delimiter)]
        [Name("PythonDelimiterFormatDefinition")]
        [Order]
        internal sealed class DelimiterClassificationFormat : ClassificationFormatDefinition
        {
            public DelimiterClassificationFormat()
            {
                this.DisplayName = "Python Delimiter";
            }
        }

        [Name(ExcessClassificationTypes.Operator), Export]
        internal ClassificationTypeDefinition OperatorClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Operator)]
        [Name("PythonOperatorFormatDefinition")]
        [Order]
        internal sealed class OperatorClassificationFormat : ClassificationFormatDefinition
        {
            public OperatorClassificationFormat()
            {
                this.DisplayName = "Python Operator";
            }
        }

        [Name(ExcessClassificationTypes.Keyword), Export]
        internal ClassificationTypeDefinition KeywordClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Keyword)]
        [Name("PythonKeywordFormatDefinition")]
        [Order]
        internal sealed class KeywordClassificationFormat : ClassificationFormatDefinition
        {
            internal KeywordClassificationFormat()
            {
                ForegroundColor = Colors.Blue;
                this.DisplayName = "Python Keyword";
            }
        }

        [Name(ExcessClassificationTypes.Identifier), Export]
        internal ClassificationTypeDefinition IdentifierClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Identifier)]
        [Name("PythonIdentifierFormatDefinition")]
        [Order]
        internal sealed class IdentifierClassificationFormat : ClassificationFormatDefinition
        {
            public IdentifierClassificationFormat()
            {
                this.DisplayName = "Python Identifier";
            }
        }

        [Name(ExcessClassificationTypes.String), Export]
        internal ClassificationTypeDefinition StringClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.String)]
        [Name("PythonStringFormatDefinition")]
        [Order]
        internal sealed class StringClassificationFormat : ClassificationFormatDefinition
        {
            internal StringClassificationFormat()
            {
                ForegroundColor = Colors.Brown;
                this.DisplayName = "Python String";
            }
        }

        [Name(ExcessClassificationTypes.Number), Export]
        internal ClassificationTypeDefinition NumberClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Number)]
        [Name("PythonNumberFormatDefinition")]
        [Order]
        internal sealed class NumberClassificationFormat : ClassificationFormatDefinition
        {
            public NumberClassificationFormat()
            {
                this.DisplayName = "Python Number";
            }
        }

        [Name(ExcessClassificationTypes.Unknown), Export]
        internal ClassificationTypeDefinition UnknownClassificationType { get; set; }

        [Export(typeof(EditorFormatDefinition))]
        [UserVisible(true)]
        [ClassificationType(ClassificationTypeNames = ExcessClassificationTypes.Number)]
        [Name("PythonUnknownFormatDefinition")]
        [Order]
        internal sealed class UnknownClassificationFormat : ClassificationFormatDefinition
        {
            public UnknownClassificationFormat()
            {
                this.DisplayName = "Python Unknown";
            }
        }
    }
}