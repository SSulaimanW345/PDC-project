#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <omp.h>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>


#define THREAD 4
using namespace std;
double seqGreyScale(float **pixels, unsigned char *outpixels, int height, int width, int channels){
    float r, g, b;
    r = g = b = 0;
    float avg = 0;
    double starttime = omp_get_wtime();
    for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width * channels; j += 3)
            {
                r = pixels[i][j];
                g = pixels[i][j + 1];
                b = pixels[i][j + 2];

                avg = (r + g + b) / 3.0;

                pixels[i][j] = avg;
                pixels[i][j + 1] = avg;
                pixels[i][j + 2] = avg;
                
            }
        
        }
    double endtime = omp_get_wtime();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * channels; j++)
        {
            outpixels[i * width * channels + j] = (unsigned char)pixels[i][j];
        }
    }
    return endtime - starttime;
}
double parallelGreyScale(float **pixels, unsigned char *outpixels, int height, int width, int channels){
    float r, g, b;
    r = g = b = 0;
    float avg = 0;
    double starttime = omp_get_wtime();
    #pragma omp parallel for num_threads(THREAD) schedule(static) private(r,g,b,avg)
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width * channels; j += 3)
            {
                r = pixels[i][j];
                g = pixels[i][j + 1];
                b = pixels[i][j + 2];

				
                avg = (r + g + b) / 3.0;

                pixels[i][j] = avg;
                pixels[i][j + 1] = avg;
                pixels[i][j + 2] = avg;
                
            }

    double endtime = omp_get_wtime();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * channels; j++)
        {
            outpixels[i * width * channels + j] = (unsigned char)pixels[i][j];
        }
    }
    return endtime - starttime;
}
void localGreyScale(float **pixels, unsigned char *outpixels, int height, int width, int channels){
    float r, g, b;
    r = g = b = 0;
    float avg = 0;
    int rowTask = height/4;
    #pragma omp parallel num_threads(4)
    {
    	
        int index = omp_get_thread_num();
        unsigned char *localArr = new unsigned char[rowTask * width * channels];
        for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j < width * channels; j++)
            {
                localArr[i * width * channels + j] = (unsigned char)(pixels[i + rowTask * index][j]);
            }
        }
        
        for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j < width * channels; j += 3)
            {
                r = pixels[i][j];
                g = pixels[i][j + 1];
                b = pixels[i][j + 2];

                avg = (r + g + b) / 3.0;

                pixels[i][j] = avg;
                pixels[i][j + 1] = avg;
                pixels[i][j + 2] = avg;
                
            }
        
        }
        
        
        
        
        if (index == 0)
        {   
			stbi_write_png("greyscale0.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));          
		}
        if (index == 1)
        {
            stbi_write_png("greyscale1.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 2)
        {
            stbi_write_png("greyscale2.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 3)
        {
            stbi_write_png("greyscale3.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        
    }
}

double seqreflect(float **pixels, unsigned char *outpixels, int height, int width, int channels)
{
    int w = width*channels;
    double starttime = omp_get_wtime();
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<int((w)/2);j++)
        {
            float tempb = pixels[i][j];
            pixels[i][j] = pixels[i][w-j-4];
            pixels[i][w-j-4]=tempb;

            float tempg = pixels[i][j+1];
            pixels[i][j+1] = pixels[i][w-(j+1)-5];
            pixels[i][w-(j+1)-4]=tempg;

            float tempr = pixels[i][j+2];
            pixels[i][j+2] = pixels[i][w-(j+2)-6];
            pixels[i][w-(j+2)-4]=tempr;
        }
    }
    double endtime = omp_get_wtime();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * channels; j++)
        {
            outpixels[i * width * channels + j] = (unsigned char)pixels[i][j];
        }
    }
    return endtime -starttime;
}

double parallelreflect(float **pixels, unsigned char *outpixels, int height, int width, int channels)
{
    int w = width*channels;
    double starttime = omp_get_wtime();
    #pragma omp parallel for num_threads(THREAD)
    for(int i=0;i<height;i++)
        for(int j=0;j<int((w)/2);j++)
        {
            float tempb = pixels[i][j];
            pixels[i][j] = pixels[i][w-j-4];
            pixels[i][w-j-4]=tempb;

            float tempg = pixels[i][j+1];
            pixels[i][j+1] = pixels[i][w-(j+1)-5];
            pixels[i][w-(j+1)-4]=tempg;

            float tempr = pixels[i][j+2];
            pixels[i][j+2] = pixels[i][w-(j+2)-6];
            pixels[i][w-(j+2)-4]=tempr;
        }
    double endtime = omp_get_wtime();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * channels; j++)
        {
            outpixels[i * width * channels + j] = (unsigned char)pixels[i][j];
        }
    }
    return endtime -starttime;
}

void localreflect(float **pixels, unsigned char *outpixels, int height, int width, int channels){
    int w = width*channels;
    int rowTask = height/4;
    #pragma omp parallel num_threads(4)
    {
    	
        int index = omp_get_thread_num();
        unsigned char *localArr = new unsigned char[rowTask * width * channels];
        for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j < width * channels; j++)
            {
                localArr[i * width * channels + j] = (unsigned char)(pixels[i + rowTask * index][j]);
            }
        }
        
        for(int i=0;i<rowTask;i++)
        {
            for(int j=0;j<int((w)/2);j++)
            {
                float tempb = pixels[i][j];
                pixels[i][j] = pixels[i][w-j-4];
                pixels[i][w-j-4]=tempb;

                float tempg = pixels[i][j+1];
                pixels[i][j+1] = pixels[i][w-(j+1)-5];
                pixels[i][w-(j+1)-4]=tempg;

                float tempr = pixels[i][j+2];
                pixels[i][j+2] = pixels[i][w-(j+2)-6];
                pixels[i][w-(j+2)-4]=tempr;
            }
        }
        
        
        
        
        if (index == 0)
        {   
			stbi_write_png("reflect0.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));          
		}
        if (index == 1)
        {
            stbi_write_png("reflect1.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 2)
        {
            stbi_write_png("reflect2.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 3)
        {
            stbi_write_png("reflect3.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        
    }
}

double seqblur(float **pixels, unsigned char *data, int height, int width, int channels)
{
	int rows, columns,k;
    rows = height;
    columns = width * channels;
    k=0;
    int avg=0;
    float sum=0;
    int x,y,i,j;
    int c=4; //blur block size 
    int m_rows = (rows%c) ? rows / c+1 : rows / c; //to remove edges pixels while calculating
    int m_cols = (columns%c) ? columns / c+1 : columns / c; //to remove edges pixels while calculating
    int area = columns * rows;
 
    float sumr,sumg,sumb;
    double starttime = omp_get_wtime();
    for(x=0;x<m_rows;++x)
    {
        for(y=0;y<m_cols;++y)
        {
           int x_rows = (x == m_rows - 1) ? rows - x * c : c; //if our inside matrix is completely blur, blur for pixels which were removed initially
           int y_cols = (y == m_cols - 1) ? columns - y * c : c; //if our inside matrix is completely blur, blur for pixels which were removed initially
           int m_area = x_rows * y_cols;

           sumr = 0; sumb = 0; sumg = 0;
            //calculate sum in block of pixels which will be blurred
           for(i=0;i<x_rows;++i)
           {
            for(j=0;j<y_cols;++j)
            {
                //calculating offset of pixels which shall be used in blurring image
                int offset = (x * c + i) * columns + (y * c + j);
                sumr+=(float)data[offset];
                sumg+=(float)data[offset+1];
                sumb+=(float)data[offset+2];
            }
           }

            //copying back to data changed value
            for(i=0;i<x_rows;++i)
           {
            for(j=0;j<y_cols;++j)
            {
                #pragma omp critical
                {
                //calculating offset of pixels which shall be used in blurring image
                int offset = (x * c + i) * columns + (y * c + j);
                //here we are dividing by area to give blur effect
                data[offset] = (char)(sumr/(float)m_area);
                data[offset+1] = (char)(sumg/(float)m_area);
                data[offset+2] = (char)(sumb/(float)m_area);
                }
            }
           }
        }
        }
    double endtime = omp_get_wtime();
    return endtime - starttime;      
}
double parallelblur(float **pixels, unsigned char *data, int height, int width, int channels)
{
	int rows, columns,k;
    rows = height;
    columns = width * channels;
    k=0;
    int avg=0;
    float sum=0;
    int x,y,i,j;
    int c=4; //blur block size 
    int m_rows = (rows%c) ? rows / c+1 : rows / c; //to remove edges pixels while calculating
    int m_cols = (columns%c) ? columns / c+1 : columns / c; //to remove edges pixels while calculating
    int area = columns * rows;
 
    float sumr,sumg,sumb;
    double starttime = omp_get_wtime();
    #pragma omp prallel for num_threads(THREAD) schedule(static) shared(data) private(sumr,sumg,sumb)
    for(x=0;x<m_rows;++x)
        for(y=0;y<m_cols;++y)
        {
           int x_rows = (x == m_rows - 1) ? rows - x * c : c; //if our inside matrix is completely blur, blur for pixels which were removed initially
           int y_cols = (y == m_cols - 1) ? columns - y * c : c; //if our inside matrix is completely blur, blur for pixels which were removed initially
           int m_area = x_rows * y_cols;

           sumr = 0; sumb = 0; sumg = 0;
            //calculate sum in block of pixels which will be blurred
           for(i=0;i<x_rows;++i)
           {
            for(j=0;j<y_cols;++j)
            {
                //calculating offset of pixels which shall be used in blurring image
                int offset = (x * c + i) * columns + (y * c + j);
                sumr+=(float)data[offset];
                sumg+=(float)data[offset+1];
                sumb+=(float)data[offset+2];
            }
           }

            //copying back to data changed value
            for(i=0;i<x_rows;++i)
           {
            for(j=0;j<y_cols;++j)
            {
                #pragma omp critical
                {
                //calculating offset of pixels which shall be used in blurring image
                int offset = (x * c + i) * columns + (y * c + j);
                //here we are dividing by area to give blur effect
                data[offset] = (char)(sumr/(float)m_area);
                data[offset+1] = (char)(sumg/(float)m_area);
                data[offset+2] = (char)(sumb/(float)m_area);
                }
            }
           }
        }

    double endtime = omp_get_wtime();

    return endtime-starttime;      
}

void localblur(float **pixels, unsigned char *data, int height, int width, int channels){
    float sumr,sumg,sumb;
    int x,y,i,j;
	int w = width*channels;
    int rowTask = height/4;
    int rows, columns,k;
    rows = height;
    columns = width * channels;
    k=0;
    int avg=0;
    float sum=0;

    int c=4; //blur block size 
    int m_rows = (rows%c) ? rows / c+1 : rows / c; //to remove edges pixels while calculating
    int m_cols = (columns%c) ? columns / c+1 : columns / c; //to remove edges pixels while calculating
    int area = columns * rows;
 	int m_rowsTask = m_rows/4;
    
    #pragma omp parallel num_threads(4)
    {
    	
        int index = omp_get_thread_num();
        unsigned char *localArr = new unsigned char[rowTask * width * channels];
        for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j < width * channels; j++)
            {
                localArr[i * width * channels + j] = (unsigned char)(pixels[i + rowTask * index][j]);
            }
        }
        
		for(x=0;x<m_rows;++x)
    	{
        for(y=0;y<m_cols;++y)
        {
           int x_rows = (x == m_rows - 1) ? rows - x * c : c; //if our inside matrix is completely blur, blur for pixels which were removed initially
           int y_cols = (y == m_cols - 1) ? columns - y * c : c; //if our inside matrix is completely blur, blur for pixels which were removed initially
           int m_area = x_rows * y_cols;

           sumr = 0; sumb = 0; sumg = 0;
            //calculate sum in block of pixels which will be blurred
           for(i=0;i<x_rows;++i)
           {
            for(j=0;j<y_cols;++j)
            {
                //calculating offset of pixels which shall be used in blurring image
                int offset = (x * c + i) * columns + (y * c + j);
                sumr+=(float)data[offset];
                sumg+=(float)data[offset+1];
                sumb+=(float)data[offset+2];
            }
           }

            //copying back to data changed value
            for(i=0;i<x_rows;++i)
           {
            for(j=0;j<y_cols;++j)
            {
                #pragma omp critical
                {
                //calculating offset of pixels which shall be used in blurring image
                int offset = (x * c + i) * columns + (y * c + j);
                //here we are dividing by area to give blur effect
                data[offset] = (char)(sumr/(float)m_area);
                data[offset+1] = (char)(sumg/(float)m_area);
                data[offset+2] = (char)(sumb/(float)m_area);
                }
            }
           }
        }
        }
        
        
        
        if (index == 0)
        {   
			stbi_write_png("blur0.png", width, rowTask, channels, data, channels * width * sizeof(unsigned char));          
		}
        if (index == 1)
        {
            stbi_write_png("blur1.png", width, (height-(rowTask*2)), channels, data, channels * width * sizeof(unsigned char));
        }
        if (index == 2)
        {
            stbi_write_png("blur2.png", width, (height-rowTask), channels, data, channels * width * sizeof(unsigned char));
        }
        if (index == 3)
        {
            stbi_write_png("blur3.png", width, height, channels, data, channels * width * sizeof(unsigned char));
        }
        
    }
}
int main()
{
	int width, height, channels;
    	unsigned char *data = stbi_load("download.jpg", &width, &height, &channels, 0);
    	if (data == NULL)
    	{
    	    cout << "Failed to load image " << endl;
    	    exit(1);
    	}
    	int size = width * height * channels;
    	float **pixels = new float *[height];
    	for (int i = 0; i < height; i++)
    	{
    	    pixels[i] = new float[width * channels];
    	}
    	for (int i = 0; i < height; i++)
    	{
    	    for (int j = 0; j < width * channels; j++)
   		    {
    	        pixels[i][j] = float(data[i * width * channels + j]);
    	    }
    	}
    int choice;
    do{
        cout << "Select filter you want to run: " << endl
             << "1. Grey Scale" << endl
             << "2. Reflect" << endl
             << "3. Mosaic Blur" << endl
             << "0. End program" << endl << endl
             << "Enter your choice: ";
             cin >> choice;
        if(choice != 1 && choice!= 2 && choice !=3 && choice !=0){
	    	cout << "Invalid choice entered";
	    	sleep(1);
		}
		system("cls");
	}while(choice != 1 && choice!= 2 && choice !=3 && choice !=0);

    if(choice == 1)
    {
    	
        double serialtime = seqGreyScale(pixels, data, height, width, channels);
		printf("Grey Scale [ Sequential ] =  %lf seconds\n", serialtime);
	    stbi_write_png("greySerial.png", width, height, channels, data, channels * width * sizeof(unsigned char));
		
	    double paralleltime = parallelGreyScale(pixels, data, height, width, channels);
        printf("GreyScale [ parallel ] =  %lf seconds\n", paralleltime);
        stbi_write_png("greyParallel.png", width, height, channels, data, channels * width * sizeof(unsigned char));	
	
        cout<<"Local Results ....."<<endl;
        localGreyScale(pixels, data, height, width, channels);
        stbi_image_free(data);
    }
    if(choice == 2)
    {
        double serialtime = seqreflect(pixels, data, height, width, channels);
		printf("Reflect [ Sequential ] =  %lf seconds\n", serialtime);
	    stbi_write_png("reflectSerial.png", width, height, channels, data, channels * width * sizeof(unsigned char));
		
		data = stbi_load("download.jpg", &width, &height, &channels, 0);
		for (int i = 0; i < height; i++)
    	{
    	    for (int j = 0; j < width * channels; j++)
   		    {
    	        pixels[i][j] = float(data[i * width * channels + j]);
    	    }
    	}
	    double paralleltime = parallelreflect(pixels, data, height, width, channels);
        printf("Reflect [ parallel ] =  %lf seconds\n", paralleltime);
        stbi_write_png("reflectParallel.png", width, height, channels, data, channels * width * sizeof(unsigned char));	
	
        cout<<"Local Results ....."<<endl;
        localreflect(pixels, data, height, width, channels);
        stbi_image_free(data);
    }
    if(choice == 3)
    {
        double serialtime = seqblur(pixels, data, height, width, channels);
		printf("Reflect [ Sequential ] =  %lf seconds\n", serialtime);
	    stbi_write_png("blurSerial.png", width, height, channels, data, channels * width * sizeof(unsigned char));
		
	    double paralleltime = parallelblur(pixels, data, height, width, channels);
        printf("Reflect [ parallel ] =  %lf seconds\n", paralleltime);
        stbi_write_png("blurParallel.png", width, height, channels, data, channels * width * sizeof(unsigned char));	
	
        cout<<"Local Results ....."<<endl;
        localblur(pixels, data, height, width, channels);

    }
    if(choice == 0)
    {
        stbi_image_free(data);
        return 0;
    }
}