//
//  Common.hpp
//  cameraDeal
//
//  Created by numberwolf on 16/3/2.
//  Copyright © 2016年 numberwolf. All rights reserved.
//

#ifndef Common_hpp
#define Common_hpp

#include <stdio.h>
#include <iostream>
#include <math.h>

#endif /* Common_hpp */

class Common {
    
public:
    Common() {}
    ~Common() {}
    
    template<typename T>
    static T GetAverage(T *array, int offset, int limit) {
        T sum = 0;
        
        for (int i = offset; i < (limit - offset); i++) {
            sum += array[i];
        }
        
        return sum/(limit - offset);
    }
    
    // 方差
    template<typename T>
    static double GetVariance(T *array, int offset, int limit) {
        
        //求数组x（具有n个元素）的方差:S=(<x^2>-<x>)^0.5
        double xaver=0.0, x2aver=0.0;
        
        for(int i = offset;i < (limit - offset); ++i){
            xaver+=array[i]; x2aver+=array[i]*array[i];
        }
        
        xaver/=limit; x2aver/=limit; //求x的平均、x^2的平均
        return sqrt(x2aver-xaver*xaver);
    }
    
    // 标准差
    template<typename T>
    static double GetStandard(T *array, int offset, int limit) {
        
        double sum = 0.0f;
        double average = GetAverage(array, offset, limit);
        
        for (int i = offset; i < (limit - offset); i++) {
            sum += (array[i] - average) * (array[i] - average);
        }
        
        return sqrt(sum/limit);
    }
    
private:
    
protected:
    
};

