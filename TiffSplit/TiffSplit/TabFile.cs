using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;
using System.Globalization;

namespace TiffSplit
{
    public class TabFile
    {
        private bool mIsValidSignature;
        private string mVersion;
        private string mCharSet;
        private string mFilename;
        private string mType;
        private PointF mCoordinate;

        public TabFile(string fileName)
        {
            FileStream fileStream = new FileStream(fileName, FileMode.Open);
            byte[] buffer = new byte[fileStream.Length];
            fileStream.Read(buffer, 0, buffer.Length);
            fileStream.Close();

            string input = Encoding.Default.GetString(buffer);
            Parse(input);
        }

        public bool IsValid()
        {
            return mIsValidSignature && !string.IsNullOrEmpty(mVersion) &&
                !string.IsNullOrEmpty(mFilename) && !string.IsNullOrEmpty(mType);
        }

        public string Filename()
        {
            return mFilename;
        }

        public PointF Coordinate()
        {
            return mCoordinate;
        }

        private void Parse(string buffer)
        {
            string[] lines = buffer.Split(new string[] { "\r\n", "\n" }, StringSplitOptions.None);

            foreach (string line in lines)
            {
                ParseLine(line);
            }
        }

        private void ParseLine(string line)
        {
            line = line.Trim();

            if (line == "!table")
            {
                mIsValidSignature = true;
            }
            else if (FirstWord(line) == "!version")
            {
                mVersion = Params(line);
            }
            else if (FirstWord(line) == "!charset")
            {
                mCharSet = Params(line);
            }
            else if (FirstWord(line) == "File")
            {
                mFilename = Params(line).Trim(new char[] { '"' });
            }
            else if (FirstWord(line) == "Type")
            {
                mType = Params(line).Trim(new char[] {'"'});
            }
            else if (mType != null && mType.ToLower() == "raster" && mCoordinate.IsEmpty)
            {
                mCoordinate = ParseRect(line);
            }
        }

        private static PointF ParseRect(string line)
        {
            string[] fields = line.Split(' ');
            foreach (string field in fields)
            {
                string trimmed = field.Trim(new char[] {'(', ')'} );
                string[] coords = trimmed.Split(',');
                try
                {
                    NumberFormatInfo provider = new NumberFormatInfo();
                    float easting = (float)Convert.ToDouble(coords[0], provider);
                    float northing = (float)Convert.ToDouble(coords[1], provider);

                    return new PointF(easting, northing);
                }
                catch (FormatException)
                {
                    // Ignore
                }
                catch (OverflowException)
                {
                    // Ignore
                }
            }

            return new PointF();
        }

        private static string FirstWord(string line)
        {
            string[] split = line.Split(new char[] {' '});
            return split.First();
        }

        private static string Params(string line)
        {
            string word = FirstWord(line);
            string lineParams = line.Substring(word.Length);
            return lineParams.Trim();
        }
    }
}
