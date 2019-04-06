using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BitMiracle.LibTiff.Classic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;


/*
 * This tool splits a large TIFF file into many small tiles.
 * 
 * Currently the only supported filetype is 'Maastokarttarasteri 1:50 000' here:
 * https://tiedostopalvelu.maanmittauslaitos.fi/tp/kartta
 **/
namespace TiffSplit
{
    class Program
    {
        [STAThreadAttribute]    // Required by OpenFileDialog.
        static void Main(string[] args)
        {
            Console.Out.WriteLine("TiffSplit");

            string inputFilePath = "";
            string outputFolder = "";

            if (args.Length == 0)
            {
                OpenFileDialog fileDialog = new OpenFileDialog();
                fileDialog.Filter = "MapInfo Tab Files (*.tab)|*.tab|All files (*.*)|*.*";
                if (fileDialog.ShowDialog() != DialogResult.OK)
                {
                    return;
                }

                inputFilePath = fileDialog.FileName;

                FolderBrowserDialog folderDialog = new FolderBrowserDialog();
                folderDialog.Description = "Select folder for output tiles:";
                if (folderDialog.ShowDialog() != DialogResult.OK)
                {
                    return;
                }

                outputFolder = folderDialog.SelectedPath;
            }
            else if (args.Length == 2)
            {
                inputFilePath = args[0];
                outputFolder = args[1];
            }
            else
            {
                Console.Out.WriteLine("Invalid arguments.");
                Console.Out.WriteLine("Usage: TiffSplit <filename.tab> <outputfolder>");
                Console.Out.WriteLine("<outputfolder> will be created and it will contain the resulting tiles.");
                return;
            }

            Console.Out.WriteLine("Input file: " + inputFilePath);
            Console.Out.WriteLine("Output folder: " + outputFolder);

            TiffSplitter splitter = new TiffSplitter(inputFilePath, outputFolder, 480, 2);     // TODO: magic, scale
            splitter.split();
        }
    }
}
