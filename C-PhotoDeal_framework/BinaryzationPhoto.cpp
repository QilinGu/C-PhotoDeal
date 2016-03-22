//
//  BinaryzationPhoto.cpp
//  cameraDeal
//
//  Created by numberwolf on 16/3/1.
//  Copyright © 2016年 numberwolf. All rights reserved.
//
/**************************************************************************
 * Email：porschegt23@foxmail.com || numberwolf11@gmail.com
 * Github:https://github.com/numberwolf
 * APACHE 2.0 LICENSE
 * Copyright [2016] [Chang Yanlong]
 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 **************************************************************************/


#include "BinaryzationPhoto.hpp"
#include "Common.hpp"
#include <math.h>

#define CANNY_PIXELS_VAL 200  // 临时存放边界key
#define CANNY_PIXELS_ALPHA 255

#pragma mark
void BinaryzationPhoto::binaryCanny(int wRadius, int hRadius, int width, int height) {
    
    for (int j = 0; j < height; j+=wRadius) {
        for (int i = 0; i < width; i+=hRadius) {
            // 区域内处理
            int *localArr = new int[wRadius*hRadius];
            int *pointer = localArr;
            
            // 将范围内的添加进数组
            for (int y = j; y < (j+hRadius); y++) {
                for (int x = i; x < (i+wRadius); x++) {
                    
                    if (x >= width || y >= height) {
                        *pointer = 0;
                    } else {
                        *pointer = this->BinaryPixels->getGray(x, y); // 因为是已经二值化的图 所以只需要取其中一个通道就可以
                    }
                    
                    pointer++;
                }
            } // 添加结束
            
            // 范围内第一步处理
            for (int y = j; y < (j+hRadius); y++) {
                for (int x = i; x < (i+wRadius); x++) {
                    
                    // (x,y)左边的点
                    int x_right = x + 1;
                    int y_right = y;
                    
                    // (x,y)下边的点
                    int x_under = x;
                    int y_under = y + 1;
                    
                    if (x_right >= width || y_right >= height || x_under >= width || y_under >= height) {
                        // 越界和不需要比对边缘的
                        continue;
                    } else {
                        if (y < (height - 1)) {
                            if (x == (width - 1)) { // 当遇到处理x最边像素的时候
                                if (this->BinaryPixels->getGray(x, y) > this->BinaryPixels->getGray(x_under, y_under) || this->BinaryPixels->getGray(x, y) < this->BinaryPixels->getGray(x_under, y_under)) {
                                    
                                    this->BinaryPixels->rgbMake(x, y, CANNY_PIXELS_VAL, CANNY_PIXELS_VAL, CANNY_PIXELS_VAL, CANNY_PIXELS_ALPHA);
                                }
                                
                            } else {
                                if (this->BinaryPixels->getGray(x, y) > this->BinaryPixels->getGray(x_right, y_right) || this->BinaryPixels->getGray(x, y) > this->BinaryPixels->getGray(x_under, y_under) || this->BinaryPixels->getGray(x, y) < this->BinaryPixels->getGray(x_right, y_right) || this->BinaryPixels->getGray(x, y) < this->BinaryPixels->getGray(x_under, y_under)) {
                                    
                                    this->BinaryPixels->rgbMake(x, y, CANNY_PIXELS_VAL, CANNY_PIXELS_VAL, CANNY_PIXELS_VAL, CANNY_PIXELS_ALPHA);
                                }
                            }
                        }
                    }
                    
                }
            } // 第一步处理结束
        
            pointer = NULL;
            delete [] localArr;
            
        } // end for-i-width
    } // end for-j-height
    
    // 范围内第二步处理
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (this->BinaryPixels->getGray(i, j) == CANNY_PIXELS_VAL) {
                //printf("白线");
                this->BinaryPixels->rgbMake(i, j, 255, 255, 255, 255);
            } else {
                this->BinaryPixels->rgbMake(i, j, 0, 0, 0, 255);
            }
        }
    } // 第二步处理结束
}

// 区域二值化
void BinaryzationPhoto::binaryzation(int wRadius, int hRadius, int width, int height) {
    
    for (int j = 0; j < height; j+=1) {
        for (int i = 0; i < width; i+=1) {
            
            // (y,x)->(h,w)
            int *localArr = new int[wRadius*hRadius];
            int *pointer = localArr;
            
            // 将范围内的添加进数组
            for (int y = j; y < (j+hRadius); y++) {
                for (int x = i; x < (i+wRadius); x++) {
                    
                    if (x >= width || y >= height) {
                        *pointer = 0;
                    } else {
                        *pointer = this->BinaryPixels->getGray(x, y);
                    }
                    
                    pointer++;
                }
            } // 添加结束
            
            // 标准差
            int standard = Common::GetStandard(localArr, 0, wRadius*hRadius);
            
            // 平均数
            int average = Common::GetAverage(localArr, 0, wRadius*hRadius);
            
            // 范围内处理
            for (int y = j; y < (j+hRadius); y++) {
                for (int x = i; x < (i+wRadius); x++) {
                    
                    if (x >= width || y >= height) {
                        continue;
                    } else {
                        if (this->BinaryPixels->getGray(x, y) > average) {
                            this->BinaryPixels->rgbMake(x, y, 255, 255, 255, 255);
                        } else {
                            this->BinaryPixels->rgbMake(x, y, 0, 0, 0, 255);
                        }
                        //printf("-->%3d ",this->BinaryPixels->getGray(x, y));
                    }
                }
            } // 处理结束
            
            pointer = NULL;
            delete [] localArr;
            
        }
    }
}
