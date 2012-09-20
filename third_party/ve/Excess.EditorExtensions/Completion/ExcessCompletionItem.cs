using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Language.Intellisense;
using Microsoft.VisualStudio.Utilities;
using System.Windows.Controls;
using System.Windows.Media;
using Microsoft.VisualStudio.TextManager.Interop;
using ExcessCompiler;

namespace Excess.EditorExtensions
{
    /// <summary>
    /// Provides a completion for IronPython
    /// </summary>
    internal class CompletionItem : Completion, IComparable
    {
        /// <summary>
        /// Constructor used by IPy declarations retrieved from the IPy engine
        /// </summary>
        /// 

        //td:
        //internal PyCompletion(Declaration declaration, IGlyphService glyphService)
        internal CompletionItem(ExcessCompletionItem itm, IGlyphService glyphService)
            //: base(declaration.Title)
            : base("")
        {
            this.InsertionText = itm.Item;
            this.DisplayText = itm.Item;
            this.Description = itm.Desc;
            this.IconSource = glyphService.GetGlyph(GetGroupFromDeclaration(itm.Id), GetScopeFromDeclaration(itm.Id));
        }

        /// <summary>
        /// Constructor used by IPy snippets retrieved from the expansion manager
        /// </summary>
        internal CompletionItem(VsExpansion vsExpansion, IGlyphService glyphService)            
            : base(vsExpansion.title)
        {            
            this.InsertionText = vsExpansion.title;
            this.Description = vsExpansion.description;
            this.IconSource = glyphService.GetGlyph(StandardGlyphGroup.GlyphCSharpExpansion, StandardGlyphItem.GlyphItemPublic);

            this.VsExpansion = vsExpansion;
        }

        private StandardGlyphItem GetScopeFromDeclaration(CompletionType id)
        {
            return StandardGlyphItem.GlyphItemPublic;
        }

        private StandardGlyphGroup GetGroupFromDeclaration(CompletionType id)
        {
            switch (id)
            {
                case CompletionType.Instance:
                    return StandardGlyphGroup.GlyphGroupConstant;
                case CompletionType.Property:
                    return StandardGlyphGroup.GlyphGroupProperty;
                case CompletionType.Method:
                    return StandardGlyphGroup.GlyphGroupMethod;
                case CompletionType.Event:
                    return StandardGlyphGroup.GlyphGroupEvent;
                case CompletionType.Type:
                    return StandardGlyphGroup.GlyphGroupClass;
                case CompletionType.Variable:
                    return StandardGlyphGroup.GlyphGroupVariable;
                default:
                    return StandardGlyphGroup.GlyphGroupClass;
            }
        }

        internal VsExpansion? VsExpansion { get; private set; }

        public int CompareTo(object other)
        {
            var otherCompletion = other as CompletionItem;
            return this.DisplayText.CompareTo(otherCompletion.DisplayText); 
        }
    }
}