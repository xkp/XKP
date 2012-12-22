using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace Excess.APKInstaller
{
    class HandleProcess
    {
        #region fields
        private string fileName;
        private string arguments;
        private int lastResult;
        private bool success;
        private bool waitForExit;
        private int exitTimeLimit;
        private string standardOutput;
        private string standardError;
        #endregion

        #region properties
        public string FileName
        {
            get
            {
                return fileName;
            }
            set
            {
                fileName = value;
            }
        }

        public string Arguments
        {
            get
            {
                return arguments;
            }
            set
            {
                arguments = value;
            }
        }

        public bool WaitForExit
        {
            get
            {
                return waitForExit;
            }
            set
            {
                waitForExit = value;
            }
        }

        public int ExitTimeLimit
        {
            get
            {
                return exitTimeLimit;
            }
            set
            {
                exitTimeLimit = value;
            }
        }

        public int LastResult
        {
            get { return lastResult; }
        }

        public bool Success
        {
            get { return success; }
        }

        public string StandardOutput
        {
            get { return standardOutput; }
        }

        public string StandardError
        {
            get { return standardError; }
        }
        #endregion

        #region ctors
        public HandleProcess(string fileName, string arguments, bool waitForExit = true, int exitTimeLimit = 10000)
        {
            FileName = fileName;
            Arguments = arguments;
            WaitForExit = waitForExit;
            ExitTimeLimit = exitTimeLimit;
        }
        #endregion

        #region public methods
        public int Start()
        {
            Process proc = new Process();
            proc.StartInfo = new ProcessStartInfo(FileName);

            if (!string.IsNullOrEmpty(Arguments))
                proc.StartInfo.Arguments = Arguments;

            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardError = true;

            var stdOutput = new StringBuilder();
            var stdError = new StringBuilder();

            standardOutput = "";
            standardError = "";

            proc.OutputDataReceived += new DataReceivedEventHandler(StandardOutputHandler);
            proc.ErrorDataReceived += new DataReceivedEventHandler(StandardErrorHandler);

            try
            {
                success = proc.Start();

                proc.BeginOutputReadLine();
                proc.BeginErrorReadLine();

                if (ExitTimeLimit == -1)
                    proc.WaitForExit();
                else
                    success = proc.WaitForExit(ExitTimeLimit);

                if (success)
                    lastResult = proc.ExitCode;
                else
                    lastResult = -1;
            }
            catch (Exception e)
            {
                Console.WriteLine("TODO 7: " + e.Message);
                return -1;
            }

            return lastResult;
        }
        #endregion

        #region private methods
        private void StandardOutputHandler(object sendingProcess,
            DataReceivedEventArgs outLine)
        {
            standardOutput += outLine.Data + "\r\n";
        }

        private void StandardErrorHandler(object sendingProcess,
            DataReceivedEventArgs outLine)
        {
            standardError += outLine.Data + "\r\n";
        }
        #endregion
    }
}
