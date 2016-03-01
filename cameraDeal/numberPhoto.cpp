//
//  numberPhoto.cpp
//  cameraDeal
//
//  Created by numberwolf on 16/2/22.
//  Copyright © 2016年 numberwolf. All rights reserved.
//

#include <math.h>
#include "numberPhoto.hpp"

//#define CUT_NUM 5 // 块大小
#define e 2.71828
#define PI 3.1416

#define Mask8(x) ( (x) & 0xFF )
#define R(x) ( Mask8(x) )
#define G(x) ( Mask8(x >> 8 ) )
#define B(x) ( Mask8(x >> 16) )
#define A(x) ( Mask8(x >> 24) )
#define RGBAMake(r, g, b, a) ( Mask8(r) | Mask8(g) << 8 | Mask8(b) << 16 | Mask8(a) << 24 )

/*****
 UInt32 * currentPixel = pixels;
 for (NSUInteger j = 0; j < height; j++) {
     for (NSUInteger i = 0; i < width; i++) {
         // 3.
         UInt32 color = *currentPixel;
         int temp = (R(color)+G(color)+B(color))/3.0;
         if(temp > 250) {
             temp = 255;
         } else {
             temp = 0;
         }
         printf("%3d ",temp);

         // 4.
         currentPixel++;
     }
     printf("\n");
 }
 *****/

void numberPhoto::blackAndWhite(uint32_t *pixels, unsigned long width, unsigned long height) {
    
    // 进行临时赋值处理
    int **gray_arr = new int*[height];
    int **temp = new int*[height];
    //int CUT_NUM = (int)width/20;
    int CUT_NUM_WIDTH = (int)width/20;
    int CUT_NUM_HEIGH = (int)height/20;
    //printf("width:%d,height:%d\n",(int)width,(int)height); // 480,360
    /*       w
      h  {  [1,2,3]
            [a,b,c] }
     */
    
    uint32_t *currentPixel = pixels;
    for (int j = 0; j < height; j++) {
        gray_arr[j] = new int[width];
        temp[j] = new int[width];
        for (int i = 0; i < width; i++) {
            // 3
            uint32_t color = *currentPixel;
            int averageColor = (R(color)+G(color)+B(color))/3.0;
            
            gray_arr[j][i] = averageColor;
            temp[j][i] = averageColor;
            // 4.
            currentPixel++;
        }
    }
    
    /*
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (gray_arr[j][i] > 250) {
                printf("255");
            } else {
                printf("   ");
            }
            
        }
        printf("\n");
    }
    printf("结束了\n");*/
     
    
    GaussDeal(gray_arr, temp, width, height, 3);
    
    // 进行处理
//    for (int i = 0; i < height; i+=CUT_NUM_HEIGH) {
//        for (int j = 0; j < width; j+=CUT_NUM_WIDTH) {
//            /**
//            double fc = fangcha(gray_arr[i], j, j+CUT_NUM);
//            printf("%3f,%3f\n",fangcha(gray_arr[i], j, j+CUT_NUM),fangcha(gray_arr[i], j, j+CUT_NUM));
//            double average = GetSumOfArray(gray_arr[i], j, CUT_NUM)/CUT_NUM;
//            
//            if ( gray_arr[i][j] <= (average + fc/3) && gray_arr[i][j] >= (average - fc/3) ) {
//                gray_arr[i][j] = 0;
//            } else {
//                gray_arr[i][j] = 255;
//            }
//            **/
//            
//            RectHandle(gray_arr, temp,CUT_NUM_WIDTH, CUT_NUM_HEIGH, j, i);
//        }
//    }

    
    // 最终将处理结果赋值过去
    // Convert the image to black and white
    currentPixel = pixels;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            // 3.
            *currentPixel = RGBAMake(gray_arr[j][i], gray_arr[j][i], gray_arr[j][i], A(*currentPixel));
            // 4.
            currentPixel++;
        }
    }
    
    delete [] gray_arr;
    delete [] temp;
}

#pragma mark 高斯模糊
void numberPhoto::GaussDeal(int **array, int **temp,int width, int height, int r) {
    
    // **array是二维数组，是rgb8888 最后的每个像素组成的数组
    double xaver = 0.0, x2aver = 0.0;
    
    for (int h = 0; h < height; h++) {
        
        xaver=0.0, x2aver=0.0;
        for (int w = 0 ; w < width; w++) {
            
            int *pixelsArr = new int[(2*r+1)*(2*r+1)]; // 存储半径内像素
            int *pixTemp = pixelsArr;
            
            int pixelSum = 0;
            int pix2Sum = 0;
            
            for (int j = 0; j <= (2*r); j++) {
                int y = h - r + j;
                
                for (int i = 0; i <= (2*r); i++) {
                    int x = w - r + i;
                    
                    // 除去中心点 和 越界点
                    if ( y < 0 || x < 0 || y >= height || x >= width || (y == h && x == w)) {
                        *pixTemp = 0;
                        pixTemp++;
                        continue;
                    }
                    
                    *pixTemp = temp[y][x];
                    
                    pixelSum += *pixTemp; // 总数
                    pix2Sum += (*pixTemp) * (*pixTemp); // 总数^2
                    
                    pixTemp++;
                }
            }
            
            /** change End **/
        
            // 开始计算方差
            xaver = (pixelSum - temp[h][w])/((2*r+1)*(2*r+1)-1);
            x2aver = (pix2Sum - temp[h][w]*temp[h][w])/((2*r+1)*(2*r+1)-1);
            
            double fc = sqrt(x2aver - xaver*xaver); // 得到方差
            
            // 开始计算权重
            double left = 1/(2*PI*fc*fc);
            
            double *weightArr = new double[(2*r+1)*(2*r+1)]; // 得到权重数组
            int weightArrNum = 0;
            double weightSum = 0.0;
            
            // 周围像素权重
            for (int j = 0; j <= (2*r); j++) {
                int y = r - j;
                int y_real = h - r + j; // 真坐标
                
                for (int i = 0; i <= (2*r); i++) {
                    int x = i - r;
                    int x_real = w - r + i; // 真坐标
                    
                    // 除去中心点
                    if ((x == 0 && y == 0) || y_real < 0 || x_real < 0 || y_real > height || x_real > width) {
                        weightArr[weightArrNum] = 0;
                        weightArrNum++;
                        continue;
                    }
                    
                    double right = pow(e, -(x*x + y*y)/(2*fc*fc));
                    double weight = left*right;
                    
                    weightArr[weightArrNum] = weight;

                    weightArrNum++;
                }
            }
            
            for (int n = 0; n < ((2*r+1)*(2*r+1)); n++) {
                weightSum += weightArr[n];
            }
            
            weightArrNum = 0;
            
            int array_h_w_temp = 0;
            
            // 得到周围像素权重
            for (int j = 0; j <= (2*r); j++) {
                for (int i = 0; i <= (2*r); i++) {
                    weightArr[weightArrNum] /= weightSum;
                    *pixelsArr = (*pixelsArr) * weightArr[weightArrNum]; // 周围像素的 权值 * 像素
                    array_h_w_temp += *pixelsArr;
                    
                    weightArrNum++;
                    pixelsArr++;
                }
            }
            
            array[h][w] = array_h_w_temp;
            
            delete [] weightArr;
            //delete [] pixelsArr;

        }
    }
    
    
}


#pragma mark 矩阵内处理
void numberPhoto::RectHandle(int **array, int **temp,int width, int height, int wStart, int hStart) {
    int sum = 0;
    
    for (int i = hStart; i < (hStart + height); i++) {
        for (int j = wStart; j < (wStart + width); j++) {
            sum += array[i][j];
        }
    }
    
    double average = sum/(width*height);
    
    double standard = Standard(array, width, height, wStart, hStart, average, width*height);
    
    for (int i = hStart; i < (hStart + height); i++) {
        for (int j = wStart; j < (wStart + width); j++) {
            
            if ( array[i][j] < (average + standard/3) && array[i][j] > (average - standard/3)) {
                array[i][j] = 0;
            } else {
                array[i][j] = 255;
            }
        }
    }
}

int numberPhoto::GetSumOfArray(int *array, int start, int count) {
    int sum = 0;
    
    for (int j = 0; j < count; j++) {
        int num = start + j;
        sum += array[num];
    }
    
    return sum;
}

#pragma mark 方差
double numberPhoto::fangcha(int x[], int start, int end){
    //求数组x（具有n个元素）的方差:S=(<x^2>-<x>)^0.5
    int i;
    double xaver=0.0, x2aver=0.0;
    
    for(i = start;i <= end; ++i){
        xaver+=x[i]; x2aver+=x[i]*x[i];
    }
    
    int n = end - start + 1;
    xaver/=n; x2aver/=n; //求x的平均、x^2的平均
    return sqrt(x2aver-xaver*xaver);
}

#pragma mark 标准差
double numberPhoto::Standard(int **array, int width, int height, int wStart, int hStart, double average, int count) {
    
    double sum = 0.0f;
    
    for (int i = hStart; i < (hStart + height); i++) {
        for (int j = wStart; j < (wStart + width); j++) {
            sum += (array[i][j] - average) * (array[i][j] - average);
        }
    }
    
    return sqrt(sum/count);
}

