#include <stdio.h>
#include <tiffio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

int FindMaxCenter(uint32 *data, int rows, int cols, int x, int yl, int yh);

void MakeBorders(uint32 *data, int rows, int cols)
{
    uint8 border = 30;
    int counter = 1;
    int darkness = 1;
    int level = 30; 
    int summa = 0;
    int borders[100] = {};
    int cur_border = 1;
    int step = 3; // 3 => double borders
    int boundary = 900; // граница: при больших y уже молния
    

    // cols = 638, rows = 1188
    for(int x = 0; x < cols; x++)
    {
        cur_border = 1;
        
        for(int y = level; y < rows; y++)
        {
            uint8* pixel = (uint8*) (data + x + y*cols);
            uint8 brightness = pixel[1]; 
	    if( x > 230 && x < 430 && y > 760 && y < 960) step = 0; else step = 3; 
	    if( y > 760)	border = 70; else border = 30;   
            if((brightness - border)*darkness < 0)
            {
                counter++;
                if( counter <= step)
                {
                    darkness *= -1;
                    counter = 0;
                    //printf("Border y = %d\n", y);
                    
                if(y >= step)
		{
                    borders[cur_border] = y-step;
                }
                
                else
                {
                	borders[cur_border]=y;
                }
		  cur_border++;
		}
            }
            else
                counter = 0;
        }
        

        for(int i = 0; i < cur_border - 2; i++)
        {    if(i > 0)
		{
            	 *(data + x + borders[i]*cols) = 0xffffffff; //print borders
	  	  // printf(" %d ", (data + x + (borders[i+1] - borders[i])/2*cols)[1]);
	 	  //float ans = FindMaxCenter(data, rows, cols, x, borders[i], borders[i+1]);
	    	  //uint8* pix = (uint8*) (data + x + (int)ans*cols);
            	  //if( ans > 0)
                  //*(data + x + (int)ans*cols) = ( pix[1] > 100) ? 0xff0000ff : 0xffff0000;
		}
	    else 
		{	//*(data + x + borders[i]*cols) = 0xffffffff; //print borders
			float ans = FindMaxCenter(data, rows, cols, 0, borders[i], borders[i+1]);
	        	//uint8* pix = (uint8*) (data + 0 + (int)ans*cols);
            		//if( ans > 0)
                	//*(data + 0 + (int)ans*cols) = ( pix[1] > border) ? 0xff0000ff : 0xffff0000;
			
				int i = 3;
				float answ = FindMaxCenter(data, rows, cols, x, (ans - i), (ans+i));
				uint8* pixa = (uint8*) (data + x + (int)answ*cols);
            	  		if( answ > 0)
                 		 *(data + x + (int)answ*cols) = ( pixa[1] > border) ? 0xff0000ff : 0xffff0000;
				
			

		}

        }
   }
}
	
int FindMaxCenter(uint32 *data, int rows, int cols, int x, int yl, int yh)
{
float deriv[1000];
float sumy = 0, sumxy=0;
int nlight = 10;
int ndark = 5;
int border = 70;
int N = 0;
uint8* pixcenter = (uint8*) (data + x + (yl + yh)/2*cols); 
N = (pixcenter[1] > border) ? nlight : ndark;
		for(int y = (yl + yh)/2 - N; y < (yh + yl)/2 + N; y++)
		{
		uint8* pixelup = (uint8*) (data + x + (y+1)*cols); // lower pixel
		uint8* pixeldown = (uint8*) (data + x + y*cols); // higher pixel 
		deriv[y - yl/2 - yh/2 + N] = (pixelup[1] - pixeldown[1]); 
		//if (pixelup[1] - pixeldown[1] > 100) printf("%d ", pixelup[1] - pixeldown[1]);
		if (deriv[y - yl/2 - yh/2 + N] == 0) 
		{
			sumy += 1;
			sumxy += y;
		}
		else
		{
		sumy += 1/( (deriv[y - yl/2 - yh/2 + N])*(deriv[y - yl/2 - yh/2 + N]) );
		// if (x == 250) printf("%f ", sumy );
		sumxy += y/( (deriv[y - yl/2 - yh/2 + N])*(deriv[y - yl/2 - yh/2 + N]) );
		// if (x == 250) printf("sum y = %f sum xy = %f \n", sumy, sumxy );	
		}
		}
			   
int answ = (int)sumxy/sumy;
return answ;
}
 
void plotLineLow( int *data, int rows, int cols, int x0, int y0, int  x1, int y1, int color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;
	
	if( dy < 0)
	{
	yi = -1;
	dy = -dy;
	}

	int D = 2*dy - dx;
	int y = y0;

	for(int x = x0; x < x1; x++)
	{
	int* pixel = (int*)(data + x + y*cols);
	//*((int*) pixel) = 0xffffffff;
	*((int*) pixel) = color;
	if( D > 0) 
	{
		y = y + yi;
		D = D - 2*dx;
	}

	D = D + 2*dy;
	}
}

void plotLineHigh(int *data, int rows, int cols, int x0, int y0, int  x1, int y1, int color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xi = 1;
	
	if( dx < 0)
	{
	xi = -1;
	dx = -dx;
	}

	int D = 2*dx - dy;
	int x = x0;

	for(int y = y0; y < y1; y++)
	{
	int* pixel = (int*)(data + x + y*cols);
	// *((int*) pixel) = 0xffffffff; white
	*((int*) pixel) = color;
	if( D > 0) 
	{
		x = x + xi;
		D = D - 2*dy;
	}

	D = D + 2*dx;
	}
}

void PlotLine(int *data, int rows, int cols, int x0, int y0, int  x1, int y1, int color)
{
	if( abs(y1 - y0) < abs(x1 - x0))
	{
		if( x0 > x1)
		{
		plotLineLow(data, rows, cols, x1, y1, x0, y0, color);
		}

		else
		{
		plotLineLow(data, rows, cols, x0, y0, x1, y1, color);
		}	
	}
	else 
	{
		if( y0 > y1)
		{
		plotLineHigh(data, rows, cols, x1, y1, x0, y0, color);
		}

		else
		{
		plotLineHigh(data, rows, cols, x0, y0, x1, y1, color);
		}
	}
	
}
// добавить чтобы пацанов другого цвета перекрашивали
void drawing(uint32 *data, int rows, int cols)
{
	int n = 15;
	int level = 40;
	for(int x = 0; x < cols; x++)
    {
        for(int y = level; y < rows; y++)
        {
            uint8* pixel = (uint8*) (data + x + y*cols);
	    if (pixel[2] > 100) //  blue to blue
		{
			for(int i = 0; i<=n; i++) 
				{
				uint8* pixelup = (uint8*) (data + x + 1 + (y+i)*cols);
				uint8* pixeldown = (uint8*) (data + x + 1 + (y-i)*cols);
				if (pixelup[2] > 100) 
				{
				PlotLine(data, rows, cols, x, y, x + 1, y + i, 0xffff0000);
				break;
				}
				else if (pixeldown[2] > 100)
					{
					 PlotLine(data, rows, cols, x, y, x + 1, y - i, 0xffff0000);
					 break;
					}
				}
		}

	    if (pixel[0] > 100) //  red to red
		{
			for(int i = 0; i<=n; i++) 
				{
				uint8* pixelup = (uint8*) (data + x + 1 + (y+i)*cols);
				uint8* pixeldown = (uint8*) (data + x + 1 + (y-i)*cols);
				if (pixelup[0] > 100) 
				{
				PlotLine(data, rows, cols, x, y, x + 1, y + i, 0xff0000ff);
				break;
				}
				else if (pixeldown[0] > 100)
					{
					 PlotLine(data, rows, cols, x, y, x + 1, y - i, 0xff0000ff);
					 break;
					}
				}
		}
	}
   }
}

int main()
{
	TIFF* tif = TIFFOpen("image.tif","r");
	uint32 imagelength = 0;
		tsize_t scanline = 0;
		tdata_t buf = NULL;
		uint32 row = 0;
		uint32 col = 0;
		tsample_t sample = 0;
		uint32* data = NULL;
        
	if(tif)
	{
		

		if( TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &imagelength) == 0)
			printf("Tag not found\n");
        
        uint16 tag = 0;
        
		if( TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, &tag) == 0)
			printf("Tag not found\n");
        else
           // printf("%d samples in pixel\n", tag);
		if( TIFFGetField( tif, TIFFTAG_BITSPERSAMPLE, &tag) == 0)
			printf("Tag not found\n");
        else
           // printf("%d bits in sample\n", tag);

		scanline = TIFFScanlineSize( tif);
		buf = _TIFFmalloc( scanline*imagelength);
		
		for (row = 0; row < imagelength; row++)
		{
			if( TIFFReadScanline( tif, buf + scanline*row, row, sample = 0) < 0)
                printf("Error has occured while scanning line\n");
		}
		
        data = (uint32*) buf;
        MakeBorders( data, imagelength, scanline/4);
         //drawing( data, imagelength, scanline/4);

        TIFFClose(tif);
	}
	else
	{
		printf("File not open\n");
	}
	
	TIFF* out = TIFFOpen("cm_dasha.tif","w");
    
    TIFFSetField (out, TIFFTAG_IMAGEWIDTH, scanline/4);  // set the width of the image
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, imagelength);    // set the height of the image
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);   // set number of channels per pixel
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    //   Some other essential fields to set that you do not have to understand for now.
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    
    
    
    
    for (row = 0; row < imagelength; row++)
    {
        if( TIFFWriteScanline( out, buf + scanline*row, row, sample = 0) < 0)
        {
            printf("Error has occured while writling line\n");
            return -1;
        }
    }
    
    TIFFClose(out);
    _TIFFfree(buf);

	return 0;
}
