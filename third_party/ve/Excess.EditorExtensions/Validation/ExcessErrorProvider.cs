using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Text;
using Excess.CompilerTasks;
using ExcessCompiler;

namespace Excess.EditorExtensions
{
    internal class ExcessErrorProvider
    {
        internal IList<ValidationError> GetErrors(ITextBuffer textBuffer)
        {
            List<ValidationError> result = new List<ValidationError>();

            ExcessModelService service = ExcessModelService.getInstance();
            List<ExcessErrorInfo> errors = service.Model.getErrors(textBuffer.GetFileName());
            foreach(ExcessErrorInfo eei in errors)
            {
                int spos;
                int epos;
                if (eei.BeginLine == eei.EndLine)
                {
                    spos = textBuffer.CurrentSnapshot.GetLineFromLineNumber(eei.BeginLine - 1).Start.Position + eei.BeginColumn - 1;
                    epos = textBuffer.CurrentSnapshot.GetLineFromLineNumber(eei.BeginLine - 1).Start.Position + eei.EndColumn - 1;
                }
                else
                {
                    spos = textBuffer.CurrentSnapshot.GetLineFromLineNumber(eei.BeginLine - 1).Start.Position + eei.BeginColumn - 1;
                    epos = textBuffer.CurrentSnapshot.GetLineFromLineNumber(eei.BeginLine - 1).End.Position;
                }
                result.Add(new ValidationError(new Span(spos, spos < epos ? epos - spos : 1), eei.desc, eei.BeginLine - 1, eei.BeginColumn - 1));
            }

            return result;
        }
    }
}
