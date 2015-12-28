#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "bmp.h"

int main(int argc,char **argv)
{
    // Attributes 
    RGBTRIPLE *colorsArray = NULL;
    int capacity = 0;
    int n = 0;

	if (argc != 2)
	{
		printf("Usage: ./paint infile\n");
		return 1;
	}

	// Create InputFilePointer and OutputFilePointer
	FILE *inptr = fopen(argv[1], "r");
	if (inptr == NULL)
	{
		printf("Could not read the file\n");
		return 2;
	}


	// Read File Header 
	BITMAPFILEHEADER bf; 
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

	// Read File Header 
	BITMAPINFOHEADER bi; 
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

	// ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 3;
    }


// determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1,inptr);

            
            // increase capacity dynamically 
            if (n + 1 > capacity)
            {
                if (capacity == 0)
                {
                    capacity = 10;
                }
                else if (capacity <= UINT_MAX / 2)
                {
                    capacity += 5;
                }
                else 
                {
                    printf("Their is a lot of colors\n");
                    free(colorsArray);
                    break;
                }

                RGBTRIPLE* temp = realloc(colorsArray, capacity * sizeof(RGBTRIPLE));
                if (temp == NULL)
                {
                    free(colorsArray);
                    break;
                }
                colorsArray = temp;
            }

            // Check if the color is in the array and if not add it 
            int flag = 0; // FALSE 

            for (int colorNumber = 0; colorNumber < n; colorNumber++)
            {
                if (colorsArray[colorNumber].rgbtBlue == triple.rgbtBlue && 
                        colorsArray[colorNumber].rgbtGreen == triple.rgbtGreen &&
                            colorsArray[colorNumber].rgbtRed == triple.rgbtRed)
                {
                    // I have Found the color 
                    flag = 1;
                }
            }

            if (flag == 1)
            {
                flag = 0;
            }
            else if (flag == 0)
            {
                colorsArray[n++] = triple;
            }


        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

    }

    // close infile
    fclose(inptr);

    // Print the array of colors 
    for (int i = 0; i < n; i ++)
    {
        printf(" 0x%hhX%hhX%hhX\n", 
            colorsArray[i].rgbtBlue, 
                colorsArray[i].rgbtGreen, 
                    colorsArray[i].rgbtRed);
    }
    printf("\n");
    free(colorsArray);

    // that's all folks
    return 0;

}