using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Text;

namespace Excess.EditorExtensions
{
    public enum ValidationErrorSeverity
    {
        Message,
        Warning,
        Error
    }

    public enum ValidationErrorType
    {
        Syntactic,
        Semantic
    }

    public class ValidationError
    {
        public string Description { get; private set; }
        public Span Span { get; private set; }
        public ValidationErrorSeverity Severity { get; private set; }
        public ValidationErrorType Type { get; private set; }
        public int Line { get; private set; }
        public int Column { get; private set; }

        public ValidationError(Span span, string description, int line, int col)
        {
            Span = span;
            Description = description;
            Severity = ValidationErrorSeverity.Error;
            Type = ValidationErrorType.Syntactic;
            Line = line;
            Column = col;
        }

        public ValidationError(Span span, string description, ValidationErrorSeverity severity, ValidationErrorType type, int line, int col)
            : this(span, description, line, col)
        {
            Severity = severity;
            Type = type;
        }
    }
}