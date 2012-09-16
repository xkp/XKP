using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcessCompiler;

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
        }

        public ExcessModel Model
        {
            get { return model_; }
        }

        static private ExcessModelService instance_ = new ExcessModelService();
        private ExcessModel               model_;
    }
}
