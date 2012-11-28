using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcessCompiler;
using Microsoft.Win32;

namespace Excess.CompilerTasks
{
    public class ExcessModelService
    {
        static public ExcessModelService getInstance()
        {
            return instance_;
        }

        private ExcessModelService()
        {
            model_ = new ExcessModel();

            string xssIdiomsPath = (string)Registry.GetValue(@"HKEY_LOCAL_MACHINE\SOFTWARE\Excess", @"XSSIdioms", @"");
            if (xssIdiomsPath == null || xssIdiomsPath == String.Empty)
                xssIdiomsPath = (string)Registry.GetValue(@"HKEY_CURRENT_USER\SOFTWARE\Excess", @"XSSIdioms", @"");

            if (xssIdiomsPath != null)
                model_.addSearchPath(xssIdiomsPath);
        }

        public ExcessModel Model
        {
            get { return model_; }
        }

        static private ExcessModelService instance_ = new ExcessModelService();
        private ExcessModel               model_;
    }
}
