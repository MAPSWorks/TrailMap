using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Globalization;

namespace TiffSplit
{
    public class TfwFile
    {
        // Coordinates are in 'ETRS-TM35FIN-tasokoordinaatit' format.
        public double Easting { get; private set; }
        public double Northing { get; private set; }

        public TfwFile(string fileName)
        {
            FileStream fileStream = new FileStream(fileName, FileMode.Open);
            byte[] buffer = new byte[fileStream.Length];
            fileStream.Read(buffer, 0, buffer.Length);
            fileStream.Close();

            string input = Encoding.Default.GetString(buffer);
            Parse(input);
        }

        private void Parse(string buffer)
        {
            string[] lines = buffer.Split(new string[] { "\r\n", "\n" }, StringSplitOptions.None);

            string easting = lines[4];
            string northing = lines[5];

            NumberFormatInfo provider = new NumberFormatInfo();

            Easting = Convert.ToDouble(easting, provider);
            Northing = Convert.ToDouble(northing, provider);
        }
    }
}
