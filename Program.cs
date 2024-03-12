using System;
using System.Diagnostics;

namespace RobloxPlayerBeta
{
    internal class Program
    {
        static string arguments;

        static void Main(string[] args)
        {
            foreach (string arg in args) { arguments += "\"" + arg + "\" "; }
            //Console.WriteLine(arguments);

            // Roblox path
            string robloxAppDataFolderPath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "Roblox",
                "Versions"
            );

            // Bloxstrap path
            string bloxstrapAppDataFolderPath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "Bloxstrap",
                "Versions"
            );

            string robloxAnselExecutable = "eurotrucks2.exe";
            string bloxstrapAnselPath = null;
            string robloxAnselPath = null;

            // bloxstrap install
            var bloxstrapTrue = false;

            // roblox install
            var robloxTrue = false;

            // bloxstrap path
            if ((bloxstrapAnselPath = CheckForAnsel(bloxstrapAppDataFolderPath, robloxAnselExecutable)) != null)
            {
                bloxstrapTrue = true;
            }

            // roblox path
            if (bloxstrapTrue == false)
            {
                if ((robloxAnselPath = CheckForAnsel(robloxAppDataFolderPath, robloxAnselExecutable)) != null)
                {
                    robloxTrue = true;
                }
            }

            if (bloxstrapTrue == true)
            {
                Console.WriteLine(bloxstrapAnselPath);
                Console.WriteLine("Starting Bloxstrap.");
                Process.Start(bloxstrapAnselPath, arguments);
                Thread.Sleep(1000);
            }
            else if (robloxTrue == true)
            {
                Console.WriteLine(robloxAnselPath);
                Console.WriteLine("Starting Roblox.");
                Process.Start(robloxAnselPath, arguments);
                Thread.Sleep(1000);
            }
            else
            {
                DialogResult result = MessageBox.Show("Roblox installation was not found. Do you want to download it?", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
                if (result == DialogResult.OK)
                {
                    ProcessStartInfo psi = new ProcessStartInfo
                    {
                        FileName = "https://www.roblox.com/download/client",
                        UseShellExecute = true
                    };
                    Process.Start(psi);
                }
            }
        }
        static string CheckForAnsel(string folderPath, string executableName)
        {
            string anselPath = null;

            if (Directory.Exists(folderPath))
            {
                DirectoryInfo directoryInfo = new DirectoryInfo(folderPath);
                DirectoryInfo[] versionFolders = directoryInfo.GetDirectories().OrderByDescending(f => f.LastWriteTime).ToArray();

                foreach (DirectoryInfo versionFolder in versionFolders)
                {
                    string executablePath = Path.Combine(versionFolder.FullName, executableName);
                    if (File.Exists(executablePath))
                    {
                        anselPath = executablePath;
                        break;
                    }
                }
            }

            return anselPath;
        }
    }
}
