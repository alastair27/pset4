/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cs50.h>

int main(int argc, char* argv[])
{
    // TODO
    
    //open memorycard file
    FILE* file = fopen("card.raw", "r");
    if ("card.raw" == NULL)
    {
        printf("Could not open card.raw. \n");
        return 2;
    }
    
    //define BYTE data structure
    typedef uint8_t  BYTE;
    
    BYTE buffer[512];
    
    //define img   
    FILE* img = NULL;

    //define counter
    int counter = 0;

    // read 512 bytes into buffer from file
    do
    {  
        if(fread(&buffer, sizeof(BYTE), 512, file) == 512)
        {   
            //start of a new jpg?
            if ((buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] == 0xe0) || (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] == 0xe1))
            {
                //close any open files
                if (img != NULL)
                {
                    fclose(img);
                } 
                //write a new jpg with title following counter
                char title[8];
                
                sprintf(title, "%03d.jpg", counter);
                
                //open new jpg
                img = fopen(title, "a");
                
                counter = counter + 1;
            }
               
            if (img != NULL)
            {
                //write to current jpg (ie new jpeg if it is the start of a new jpg, or existing jpg if not)
                fwrite(&buffer, sizeof(BYTE), 512, img);   
            } 
        }
                            
    }  while (!feof(file));
    
    // close any remaining files  
    if (img != NULL)
    {
        fclose(img);
    }
}


