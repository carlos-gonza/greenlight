#include "bmp.h"
#include <vector>
#include <stdlib.h>

#include <thread>
#include "frame.h"
#include "action.h"
#include "state.h"

using namespace std;

extern int BmpNum;
int imageWidth, imageHeight;

BYTE savedata[1024*1024*3];

void SaveBitmapToFile(BYTE* pBitmapBits,
                      LONG lWidth,
                      LONG lHeight,
                      WORD wBitsPerPixel,
                      const unsigned long& padding_size,
                      char *lpszFileName )
{
    // Some basic bitmap parameters
    unsigned long headers_size = sizeof( BITMAPFILEHEADER ) +
                                 sizeof( BITMAPINFOHEADER );
    unsigned long pixel_data_size = lHeight * ( ( lWidth * ( wBitsPerPixel / 8 ) ) + padding_size );
    BITMAPINFOHEADER bmpInfoHeader = {0};
    // Set the size
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Bit count
    bmpInfoHeader.biBitCount = wBitsPerPixel;
    // Use all colors
    bmpInfoHeader.biClrImportant = 0;
    // Use as many colors according to bits per pixel
    bmpInfoHeader.biClrUsed = 0;
    // Store as un Compressed
    bmpInfoHeader.biCompression = BI_RGB;
    // Set the height in pixels
    bmpInfoHeader.biHeight = lHeight;
    // Width of the Image in pixels
    bmpInfoHeader.biWidth = lWidth;
    // Default number of planes
    bmpInfoHeader.biPlanes = 1;
    // Calculate the image size in bytes
    bmpInfoHeader.biSizeImage = pixel_data_size;
    BITMAPFILEHEADER bfh = {0};
    // This value should be values of BM letters i.e 0x4D42
    // 0x4D = M 0×42 = B storing in reverse order to match with endian
    bfh.bfType = 0x4D42;
    //bfh.bfType = 'B'+('M' << 8);
    // <<8 used to shift ‘M’ to end  */
    // Offset to the RGBQUAD
    bfh.bfOffBits = headers_size;
    // Total size of image including size of headers
    bfh.bfSize =  headers_size + pixel_data_size;

    // Create the file in disk to write
    FILE *out;
    out = fopen(lpszFileName,"wb");
    if (out == NULL)
        return;

    // Write the File header
    fwrite(&bfh, sizeof(char), sizeof(bfh), out);
    // Write the bitmap info header
    fwrite(&bmpInfoHeader, sizeof(char), sizeof(bmpInfoHeader), out);
    // Write the RGB Data
    fwrite(pBitmapBits, sizeof(char), bmpInfoHeader.biSizeImage, out);

    // Close the file handle
    fclose (out);
}

/**
 *  LoadBMP opens bmp file in filename and returns data array
 */
BYTE *LoadBMP(int *width, int *height, unsigned long *size, char *filename)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;

    FILE *in;
    in=fopen(filename,"rb");

    fread(&bmpheader,sizeof(BITMAPFILEHEADER),1,in);
    fread(&bmpinfo,sizeof(BITMAPINFOHEADER),1,in);

    if (bmpheader.bfType != 'MB') {
        return NULL;
    }

    *width = bmpinfo.biWidth;
    *height = bmpinfo.biHeight;

    if (bmpinfo.biCompression != BI_RGB)
        return NULL;

    if (bmpinfo.biBitCount != 24)
        return NULL;

    *size = bmpheader.bfSize - bmpheader.bfOffBits;

    BYTE *buffer = new BYTE[*size];
    fseek(in, bmpheader.bfOffBits, SEEK_SET);
    fread(buffer, *size, 1, in);

    fclose(in);

    return buffer;
}


#define VAL(POS) pmatrix[(POS)] + pmatrix[(POS)+1] + pmatrix[(POS)+2]
#define SETVAL(POS,R,G,B) pmatrix[(POS)] = (R); pmatrix[(POS+1)] = (G); pmatrix[(POS+2)] = (B)
#define POSFIDX(IDX) (IDX)*3


/**
 *  findlargestval
 *  return largestval and largestvalidx for a row at y from a/3 to b/3 columns
 */
void findlargestval(BYTE* pmatrix, int apos, int bpos, int rowpos, int &largestval, int &largestvalidx)
{
    int val;
    if (apos == 0) { // left side
        for (int xpos = apos; xpos < bpos; xpos += 3) {
            val = VAL(rowpos + xpos);

            if (val > largestval) {
                largestval = val;
                largestvalidx = xpos/3;
            }
        } // for
    }
}

//std::vector<RI> pri_2_right; // 2nd pass right
//pri_1_left.push_back(aRI);

bool GetFrameStats(BYTE* pmatrix, int width, int height, FrameStats &fs)
{
    static bool bFirstCall = true;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
        }
    }
    return true;

}

bool SetActionFStatsAndState(Action &a, FrameStats &fs, State &s, int width)
{
    a.aa |= START;
    a.bplaysound = true;
    return true;
}
