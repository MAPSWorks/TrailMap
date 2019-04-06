using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BitMiracle.LibTiff.Classic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Xml;
using System.Drawing.Drawing2D;

namespace TiffSplit
{
    public class TiffSplitter
    {
        private string mFilename;
        private string mOutputFolder;
        private PointF mBaseCoordinate;
        private Point mBaseTileLocation;
        private int mTileSize;
        private int mScale;

        private Int32[] palette;
        private List<ushort[]> channels;

        /*
         * Constructor.
         * 
         * fileName     Input file. Accepts .tif and .tab.
         * outputFolder Output folder, which will be created.
         */
        // TODO: add scale parameter
        public TiffSplitter(string fileName, string outputFolder, int tileSize, int scale)
        {
            mTileSize = tileSize;
            mFilename = fileName;
            mScale = scale;
            mOutputFolder = outputFolder.TrimEnd('\\');

            // Find out TIFF filename if input is a tab file.
            if (System.IO.Path.GetExtension(mFilename) == ".tab")
            {
                TabFile tabFile = new TabFile(mFilename);
                if (tabFile.IsValid())
                {
                    mFilename = System.IO.Path.GetDirectoryName(mFilename) +
                        System.IO.Path.DirectorySeparatorChar +
                        tabFile.Filename();

                    mBaseCoordinate = tabFile.Coordinate();
                }
                else
                {
                    Console.Out.WriteLine("File is invalid.");
                    throw new Exception("Invalid file.");
                }
            }
            else if(System.IO.Path.GetExtension(mFilename) == ".tfw")
            {
                TfwFile tfwFile = new TfwFile(mFilename);
                
                Console.Out.WriteLine("File location: {0}E {1}N", tfwFile.Easting, tfwFile.Northing);
                
                mFilename = System.IO.Path.GetDirectoryName(mFilename) +
                        System.IO.Path.DirectorySeparatorChar + 
                        System.IO.Path.GetFileNameWithoutExtension(mFilename) + ".tif";

                mBaseCoordinate = new PointF((float)tfwFile.Easting, (float)tfwFile.Northing);
            }
        }

        public void split()
        {
            System.IO.Directory.CreateDirectory(mOutputFolder);

            generatePalette();

            Tiff image = Tiff.Open(mFilename, "r");
            if (image == null)
            {
                Console.Out.WriteLine("Could not open file.");
                return;
            }

            Console.Out.WriteLine("IsTiled: {0}", image.IsTiled());

            if (image.IsTiled())
            {
                Console.Out.WriteLine("Tiled");
            }
            else
            {
                int width = image.ScanlineSize();
                Console.Out.WriteLine("ScanlineSize: {0}", width);
            }

            int samplesPerPixel = getTag(image, TiffTag.SAMPLESPERPIXEL);
            int bitsPerSample = getTag(image, TiffTag.BITSPERSAMPLE);
            int photometric = getTag(image, TiffTag.PHOTOMETRIC);
            int imageWidth = getTag(image, TiffTag.IMAGEWIDTH);
            int imageHeight = getTag(image, TiffTag.IMAGELENGTH);

            Console.Out.WriteLine("samplesPerPixel: {0}", samplesPerPixel);
            Console.Out.WriteLine("bitsPerSample: {0}", bitsPerSample);
            Console.Out.WriteLine("photometric: {0}", photometric);
            Console.Out.WriteLine("imageWidth: {0}", imageWidth);
            Console.Out.WriteLine("imageHeight: {0}", imageHeight);

            FieldValue[] values = image.GetField(TiffTag.PLANARCONFIG);
            // TODO: check that type is PlanarConfig.CONTIG.
            foreach (FieldValue v in values)
            {
                Console.Out.WriteLine("FieldValue: {0}", v.Value.GetType().ToString());
            }

            // Find out colormap
            FieldValue[] colormap = image.GetField(TiffTag.COLORMAP);
            channels = new List<ushort[]>();

            foreach (FieldValue v in colormap)
            {
                channels.Add(v.ToUShortArray());
            }

            split(image);
        }

        private void generatePalette()
        {
            palette = new Int32[256];
            for (int index = 0; index < palette.Length; index++)
            {
                UInt32 c = 0xFF000000 | (uint)index;
                palette[index] = (Int32)c;
            }
        }

        private static void writeSpecFile(string fileName, Rectangle bounds, int tileSize, int scale)
        {
            XmlTextWriter textWriter = new XmlTextWriter(fileName, null);
            textWriter.Formatting = Formatting.Indented;
            textWriter.WriteStartDocument();
            textWriter.WriteComment("Tile folder info file");
            textWriter.WriteStartElement("TileInfo");
            textWriter.WriteStartElement("AbsolutePath");
            textWriter.WriteString("");     // Empty path for now.
            textWriter.WriteEndElement();
            textWriter.WriteStartElement("Scale");
            textWriter.WriteString(scale.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteStartElement("TileSize");
            textWriter.WriteString(tileSize.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteStartElement("Left");
            textWriter.WriteString(bounds.Left.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteStartElement("Top");
            textWriter.WriteString(bounds.Top.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteStartElement("Right");
            textWriter.WriteString(bounds.Right.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteStartElement("Bottom");
            textWriter.WriteString(bounds.Bottom.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteEndElement();
            textWriter.WriteEndDocument();
            textWriter.Close();
        }

        private void split(Tiff image)
        {
            int imageWidth = getTag(image, TiffTag.IMAGEWIDTH);
            int imageHeight = getTag(image, TiffTag.IMAGELENGTH);

            int hTiles = imageWidth / (mTileSize * mScale);
            int vTiles = imageHeight / (mTileSize * mScale);

            // This file is the origo: UR443_RVK_25.tab (coordinate 452000.000, 7218000.000)
            // Each file is 19200 x 9600 pixels.
            // Each file covers 47997,5 x 23997,5 of coordinate space.
            // TODO: read dimensions from the file.
            PointF p = new PointF(mBaseCoordinate.X - 452000, mBaseCoordinate.Y - 7218000);

            mBaseTileLocation = new Point((int)(p.X / (47997.5 / hTiles)),
                                          (int)(-p.Y / (23997.5 / vTiles)));

            // Write scale and tile folder path etc. to file.
            Rectangle bounds = new Rectangle(mBaseTileLocation.X, mBaseTileLocation.Y, hTiles - 1, vTiles - 1);
            string xmlFilename = mOutputFolder + System.IO.Path.DirectorySeparatorChar +
                System.IO.Path.GetFileNameWithoutExtension(mFilename) + ".tiles";
            writeSpecFile(xmlFilename, bounds, mTileSize, mScale);

            int tilesDone = 0;

            for (int y = 0; y < vTiles; y++)
            {
                for (int x = 0; x < hTiles; x++)
                {
                    Bitmap nonScaledBitmap = readRect(image, x * mTileSize * mScale, y * mTileSize * mScale,
                                                      mTileSize * mScale, mTileSize * mScale);
                    if (nonScaledBitmap != null)
                    {
                        Bitmap scaledBitmap = new Bitmap(mTileSize, mTileSize);
                        Graphics graphics = Graphics.FromImage(scaledBitmap);
                        graphics.InterpolationMode = InterpolationMode.High;
                        graphics.DrawImage(nonScaledBitmap, new Rectangle(0, 0, mTileSize, mTileSize));

                        Console.Out.Write("\rProgress:\t{0}%", 100 * tilesDone++ / (vTiles * hTiles));

                        string folder = string.Format(@"{0}\{1}", mOutputFolder, x + mBaseTileLocation.X);
                        string file = string.Format(@"{0}.png", y + mBaseTileLocation.Y);
                        System.IO.Directory.CreateDirectory(folder);
                        scaledBitmap.Save(folder + System.IO.Path.DirectorySeparatorChar + file, ImageFormat.Png);
                    }
                    else
                    {
                        Console.Out.WriteLine("Error!");
                        return;
                    }
                }
            }
        }
        private static int getTag(Tiff image, TiffTag tag)
        {
            FieldValue[] fieldValues = image.GetField(tag);
            foreach (FieldValue value in fieldValues)
            {
                return value.ToInt();
            }

            throw new Exception();
        }

        private Int32 paletteColor(int index)
        {
            byte r = (byte)channels[0][index];
            byte g = (byte)channels[1][index];
            byte b = (byte)channels[2][index];
            UInt32 c = 0xFF000000 | (uint)r << 16 | (uint)g << 8 | (uint)b;
            return (Int32)c;
        }


        /*
         * Reads a rectangular area of given non-tiled type TIFF file.
         */
        private Bitmap readRect(Tiff image, int hOffset, int vOffset, int width, int height)
        {
            // New bitmap
            Bitmap bitmap = new Bitmap(width, height);

            for (int y = vOffset; y < vOffset + height; y++)
            {
                // Allocate buffer for a scanline.
                byte[] buffer = new byte[image.ScanlineSize()];

                // Read scanline into buffer.
                if (!image.ReadScanline(buffer, y))
                {
                    Console.Out.WriteLine("ReadScanline() failed.");
                    return null;
                }

                // Copy specified part of the scanline to bitmap.
                for (int x = hOffset; x < hOffset + width; x++)
                {
                    //Color color = Color.FromArgb(buffer[x], 0, 0);
                    //int c = palette[buffer[x]];
                    //Color color = Color.FromArgb(c);
                    Color color = Color.FromArgb(paletteColor(buffer[x]));
                    bitmap.SetPixel(x - hOffset, y - vOffset, color);
                }
            }

            return bitmap;
        }
    }
}
