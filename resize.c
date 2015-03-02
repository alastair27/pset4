/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{

    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile, with 0 <= n <= 100\n");
        return 1;
    }

    if (argv[1] == NULL)
    {
        return 1;
    }
    
    // rememeber file names
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];   
    
    if (n < 1 || n > 100)
    {
        printf("Usage: ./copy n infile outfile, with 0 <= n <= 100\n");
        return 1;
    }
   
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // define padding for inptr 
    int paddingIn =  (4 - ((bi.biWidth) * sizeof(RGBTRIPLE) % 4)) % 4;
    
    //define outptr headers
    BITMAPFILEHEADER bf_outptr = bf;
    BITMAPINFOHEADER bi_outptr = bi;
 
    // redefine biWidth and biHeight for output file
    bi_outptr.biWidth = bi.biWidth * n;
    bi_outptr.biHeight = bi.biHeight * n;
    
    // define padding for outptr
    int paddingOut =    (4 - ((bi_outptr.biWidth * sizeof(RGBTRIPLE)) % 4)) % 4;
    
    // redfine biSize and biSizeImages
    bi_outptr.biSizeImage = ((bi_outptr.biWidth * sizeof(RGBTRIPLE)) + paddingOut) * abs(bi_outptr.biHeight);
    bf_outptr.bfSize = bi_outptr.biSizeImage + sizeof(bi_outptr) + sizeof(bf_outptr);
    
     // write outfile's BITMAPFILEHEADER
    fwrite(&bf_outptr, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_outptr, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        
        fpos_t pos;
        fgetpos(inptr, &pos);
        
        // repreat row operation n times (vertical copy)
        for (int j = 0; j < n ; j++)
        {
            if (j > 0)
            {
            fsetpos(inptr,&pos);
            }             
            // iterate over infile's scanlines          
            for (int k = 0; k < bi.biWidth; k++)
            {
             
                // temporary storage
                RGBTRIPLE triple;
                    
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                // repeat copy operation n times
                for (int l = 0; l < n; l++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
             }               
             
             // add padding to out file
             for (int pad = 0; pad < paddingOut; pad++)
             {
                 fputc(0x00, outptr);
             } 
         }
        // skip any padding
        fseek(inptr, paddingIn, SEEK_CUR); 
    }
    

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
