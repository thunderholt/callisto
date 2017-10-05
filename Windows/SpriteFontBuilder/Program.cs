using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.IO;
using Newtonsoft.Json;

namespace SpriteFontBuilder
{
    class FontColor
    {
        public int red = 0;
        public int green = 0;
        public int blue = 0;
    }

    class SpriteFontBuildSpec
    {
        public string ssRelativeOutputFilePath = "";
        public string textureRelativeOutputFilePath = "";
        public string fontName = "";
        public float fontEmSize = 0;
        public FontColor fontColour = new FontColor();
        public int textureWidth = 0;
        public int textureHeight = 0;
    }

    /*struct FileChunk
    {
        public int offset;
        public int size;
    }

    unsafe struct SpriteSheetFileHeader
    {
        public uint magicNumber;
        public int version;
        public FileChunk detailsChunk;
        public FileChunk spritesChunk;
    }

    unsafe struct SpriteSheetDetails
    {
        public fixed byte textureFilePath[64];
    }*/

    struct CharacterSprite
    {
        public char c;
        public float x;
        public float y;
        public float width;
        public float height;
    }

    class Program
    {
        static void Main(string[] args)
        {
            var program = new Program();
            //program.Do(args[0], args[1]);
            program.Do("C:\\Users\\andym\\Documents\\GitHub\\Callisto\\Assets\\Compiled", "C:\\Users\\andym\\Documents\\GitHub\\Callisto\\Assets\\Src\\SpriteFonts");
        }

        public void Do(string assetsFolderPath, string buildSpecsFolderPath)
        {
            string[] buildSpecFilePaths = Directory.GetFiles(buildSpecsFolderPath);
            var buildSpecs = new List<SpriteFontBuildSpec>();

            foreach (string buildSpecFilePath in buildSpecFilePaths)
            {
                string json = File.ReadAllText(buildSpecFilePath);

                var buildSpec = JsonConvert.DeserializeObject<SpriteFontBuildSpec>(json);
                buildSpecs.Add(buildSpec);
            }

            foreach (var buildSpec in buildSpecs)
            {
                BuildSpriteFont(assetsFolderPath, buildSpec);
            }
        }

        private void BuildSpriteFont(string assetsFolderPath, SpriteFontBuildSpec buildSpec)
        { 
            var bmp = new Bitmap(buildSpec.textureWidth, buildSpec.textureHeight);
            var gfx = Graphics.FromImage(bmp);
            var font = new Font(buildSpec.fontName, buildSpec.fontEmSize);
            var brush = new SolidBrush(Color.FromArgb(buildSpec.fontColour.red, buildSpec.fontColour.green, buildSpec.fontColour.blue));

            gfx.Clear(Color.Transparent);
            gfx.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;

            var chars = new List<char>();
            for (int i = 32; i < 127; i++)
            {
                chars.Add((char)i);
            }

            var charSprites = new List<CharacterSprite>();

            var charPoint = new Point(0, 0);
            foreach (var c in chars)
            {
                // The dimensions come out wierd for spaces, so use 'i' instead.
                string measurementString = c != ' ' ? c.ToString() : 'i'.ToString();
                string renderString = c.ToString();

                var charSize = gfx.MeasureString(measurementString, font, new Point(0, 0), StringFormat.GenericTypographic);
                
                if (charPoint.X + charSize.Width > bmp.Width)
                {
                    charPoint.X = 0;
                    charPoint.Y += (int)charSize.Height;
                }

                gfx.DrawString(renderString, font, brush, charPoint, StringFormat.GenericTypographic);

                var charSprite = new CharacterSprite();
                charSprite.c = c;
                charSprite.x = charPoint.X;
                charSprite.y = charPoint.Y;
                charSprite.width = (int)charSize.Width;
                charSprite.height = (int)charSize.Height;
                charSprites.Add(charSprite);

                charPoint.X += (int)charSize.Width;
            }

            OutputFiles(assetsFolderPath, buildSpec, bmp, charSprites);
        }

        private void OutputFiles(string assetsFolderPath, SpriteFontBuildSpec buildSpec, Bitmap bmp, List<CharacterSprite> charSprites)
        {
            string textureFilePath = Path.Combine(assetsFolderPath, buildSpec.textureRelativeOutputFilePath);
            EnsureDirectoryExists(textureFilePath);

            bmp.Save(textureFilePath, ImageFormat.Png);

            string ssFilePath = Path.Combine(assetsFolderPath, buildSpec.ssRelativeOutputFilePath);
            EnsureDirectoryExists(ssFilePath);

            using (var sw = new StreamWriter(ssFilePath, false, Encoding.ASCII))
            {
                sw.WriteLine("texture-asset-ref " + buildSpec.textureRelativeOutputFilePath);

                foreach (var charSprite in charSprites)
                {
                    string c = charSprite.c.ToString();
                    if (c == " ")
                    {
                        c = "SPACE";
                    }

                    sw.WriteLine("sprite " + c + " " + charSprite.x + " " + charSprite.y + " " + charSprite.width + " " + charSprite.height);
                }
            }
        }

        /*private void OutputFiles(string assetsFolderPath, SpriteFontBuildSpec buildSpec, Bitmap bmp, List<CharacterSprite> charSprites)
        {
            string textureFilePath = Path.Combine(assetsFolderPath, buildSpec.textureRelativeOutputFilePath);
            EnsureDirectoryExists(textureFilePath);

            bmp.Save(textureFilePath, ImageFormat.Png);

            unsafe
            {
                var fileHeader = new SpriteSheetFileHeader();
                var details = new SpriteSheetDetails();

                int fileHeaderSize = Marshal.SizeOf(fileHeader);
                int detailsSize = Marshal.SizeOf(details);

                fileHeader.magicNumber = 0xCA000001;
                fileHeader.version = 1;
                fileHeader.detailsChunk.offset = fileHeaderSize;
                fileHeader.detailsChunk.size = detailsSize;
                fileHeader.spritesChunk.offset = fileHeaderSize + detailsSize;
                fileHeader.spritesChunk.size = Marshal.SizeOf(charSprites[0]) * charSprites.Count;

                StringCopy(buildSpec.textureRelativeOutputFilePath, details.textureFilePath);

                string ssFilePath = Path.Combine(assetsFolderPath, buildSpec.ssRelativeOutputFilePath);
                EnsureDirectoryExists(ssFilePath);

                using (var fs = new FileStream(ssFilePath, FileMode.OpenOrCreate))
                {
                    byte[] fileHeaderBytes = ConvertStructToBytes(fileHeader);
                    fs.Write(fileHeaderBytes, 0, fileHeaderBytes.Length);

                    byte[] detailsBytes = ConvertStructToBytes(details);
                    fs.Write(detailsBytes, 0, detailsBytes.Length);

                    foreach (var charSprite in charSprites)
                    {
                        byte[] charSpriteBytes = ConvertStructToBytes(charSprite);
                        fs.Write(charSpriteBytes, 0, charSpriteBytes.Length);
                    }
                }
            }
        }*/

        private void EnsureDirectoryExists(string filePath)
        {
            string directoryPath = Path.GetDirectoryName(filePath);
            if (!Directory.Exists(directoryPath))
            {
                Directory.CreateDirectory(directoryPath);
            }
        }

        /*private byte[] ConvertStructToBytes(object obj)
        {
            int size = Marshal.SizeOf(obj);
            byte[] bytes = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, bytes, 0, size);
            Marshal.FreeHGlobal(ptr);
            return bytes;
        }

        private unsafe void StringCopy(string src, byte* dest)
        {
            for (int i = 0; i < src.Length; i++)
            {
                dest[i] = (byte)src[i];
            }
        }*/
    }
}
