#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "CMU418intrin.h"
#include "logger.h"
using namespace std;


void absSerial(float* values, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	if (x < 0) {
	    output[i] = -x;
	} else {
	    output[i] = x;
	}
    }
}

// implementation of absolute value using 15418 instrinsics
void absVector(float* values, float* output, int N) {
    __cmu418_vec_float x;
    __cmu418_vec_float result;
    __cmu418_vec_float zero = _cmu418_vset_float(0.f);
    __cmu418_mask maskAll, maskIsNegative, maskIsNotNegative;

    //  Note: Take a careful look at this loop indexing.  This example
    //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
    //  Why is that the case?
    for (int i=0; i<N; i+=VECTOR_WIDTH) {

		// All ones
		maskAll = _cmu418_init_ones();

		// All zeros
		maskIsNegative = _cmu418_init_ones(0);

		// Load vector of values from contiguous memory addresses
		_cmu418_vload_float(x, values+i, maskAll);               // x = values[i];

		// Set mask according to predicate
		_cmu418_vlt_float(maskIsNegative, x, zero, maskAll);     // if (x < 0) {

		// Execute instruction using mask ("if" clause)
		_cmu418_vsub_float(result, zero, x, maskIsNegative);      //   output[i] = -x;

		// Inverse maskIsNegative to generate "else" mask
		maskIsNotNegative = _cmu418_mask_not(maskIsNegative);     // } else {

		// Execute instruction ("else" clause)
		_cmu418_vload_float(result, values+i, maskIsNotNegative); //   output[i] = x; }

		// Write results back to memory
		_cmu418_vstore_float(output+i, result, maskAll);
    }
}

// Accepts an array of values and an array of exponents
// For each element, compute values[i]^exponents[i] and clamp value to
// 4.18.  Store result in outputs.
// Uses iterative squaring, so that total iterations is proportional
// to the log_2 of the exponent
void clampedExpSerial(float* values, int* exponents, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	float result = 1.f;
	int y = exponents[i];
	float xpower = x; // ? 
	while (y > 0) {
    	if (y & 0x1) {
		result *= xpower;
		if (result > 4.18f) {
		    result = 4.18f;
		    break;
		}
    }
	    xpower = xpower * xpower;
	    y >>= 1;
	}
	output[i] = result;
    }
}

void clampedExpVector(float* values, int* exponents, float* output, int N) {
	__cmu418_vec_float x;
	__cmu418_vec_int y;
    __cmu418_vec_int zero = _cmu418_vset_int(0);
	__cmu418_vec_int intone = _cmu418_vset_int(1);
	__cmu418_vec_float four = _cmu418_vset_float(4.18f);
	__cmu418_vec_float xpower;
	__cmu418_vec_float v_output;
	__cmu418_mask maskAll, maskIsNegative,maskIsNegative1,maskIsNegative2,maskIsNegative3,maskIsNegative4,maskIsNegative5;
	maskAll = _cmu418_init_ones();
	int i;
    for(i = 0; i <= N -  VECTOR_WIDTH; i += VECTOR_WIDTH){
		maskIsNegative = _cmu418_init_ones(0);
		_cmu418_vload_float(x, values+i, maskAll); //将values数组矢量化
		_cmu418_vload_int(y,exponents+i,maskAll);  //将expo数组矢量化
	    __cmu418_vec_float result = _cmu418_vset_float(1.f); //将result设置为1.0f
		_cmu418_vmove_float(xpower, x, maskAll); //对应 xpower = x
		_cmu418_vgt_int(maskIsNegative, y, zero, maskAll); //是否y > 0 ? 若大于0 则maskIsNegative 的对应位为1 反之则是0（被屏蔽）
		int part = _cmu418_cntbits(maskIsNegative);// 有多少位1 则代表多少位大于0
			while(part > 0){ //若VECTOR_WIDTH均小于0，则part = 0 ，就都不用进去了，但若有一位等于1 ，就意味着还是要进去
			maskIsNegative1 = _cmu418_init_ones(0);
			maskIsNegative2 = _cmu418_init_ones(0);
			maskIsNegative3 = _cmu418_init_ones(0);
			maskIsNegative4 = _cmu418_init_ones(0);
			maskIsNegative5 = _cmu418_init_ones(0);
			__cmu418_vec_int y1 = _cmu418_vset_int(0);
			_cmu418_vbitand_int(y1, y, intone, maskIsNegative);  //if(y & 0x1), 计算y & 0x1，得出的y1每位要么是1要么是0,1代表奇数，则是我们需要的，此时已将part<=0的位屏蔽
			_cmu418_veq_int(maskIsNegative1, y1, intone, maskIsNegative); //屏蔽y1 != 1 也就是 y1 = 0 的情况，也就是屏蔽偶数的情况
			_cmu418_vmult_float(result, result, xpower, maskIsNegative1); // result *= xpower; 
			_cmu418_vgt_float(maskIsNegative2, result, four,maskIsNegative1); //if (result > 4.18f) 
			_cmu418_vset_float(result, 4.18f, maskIsNegative2); // result = 4.18f; 
			int part1 = _cmu418_cntbits(maskIsNegative2);
			int part2 = _cmu418_cntbits(maskIsNegative);
			if(part1 == part2 && part1 != 0) //若所有位都大于4.18时，即不被屏蔽位，并排除全不进的情况，就break出去
				break;
			maskIsNegative5 = _cmu418_mask_not(maskIsNegative2);  //取反，表示不能0被break的位(但这里包括不能进while(y > 0)的位)
			maskIsNegative3 = _cmu418_mask_and(maskIsNegative5, maskIsNegative); //与运算，代表着能进while(y>0)且不能被break的位
			_cmu418_vmult_float(xpower, xpower, xpower, maskIsNegative3); // xpower = xpower*xpower
			_cmu418_vshiftright_int(y, y, intone, maskIsNegative3); //y 右移1位
			_cmu418_vgt_int(maskIsNegative4, y, zero, maskIsNegative3); //是否y > 0 ? 若大于0 则maskIsNegative4 的对应位为1 反之则是0（被屏蔽）
			part = _cmu418_cntbits(maskIsNegative4); // 有多少位1 则代表多少位大于0且没被break掉  
			maskIsNegative = maskIsNegative4; 
			}
			
		_cmu418_vstore_float(output+i, result, maskAll);
	}
	for(;i<N;i++){
		float x = values[i];
		float result = 1.f;
		int y = exponents[i];
		float xpower = x; // ? 
		while (y > 0) {
    		if (y & 0x1) {
			result *= xpower;
			if (result > 4.18f) {
		  	result = 4.18f;
			break;
			}
    	}
	    xpower = xpower * xpower;
	    y >>= 1;
		}
		output[i] = result;
    
	}
		
	
	
}


float arraySumSerial(float* values, int N) {
    float sum = 0;
    for (int i=0; i<N; i++) {
	sum += values[i];
    }

    return sum;
}

// Assume N % VECTOR_WIDTH == 0
// Assume VECTOR_WIDTH is a power of 2
float arraySumVector(float* values, int N) {
	__cmu418_vec_float x,y;
	__cmu418_mask maskAll;
	maskAll = _cmu418_init_ones();
	_cmu418_vload_float(x, values, maskAll);
	for(int i = VECTOR_WIDTH; i < N; i+= VECTOR_WIDTH){
		_cmu418_vload_float(y, values+i, maskAll);
		_cmu418_vadd_float(x,x,y,maskAll);
	}
	int part = VECTOR_WIDTH;
	while(part > 1){
		_cmu418_hadd_float(x, x);
		_cmu418_interleave_float(x, x);
		part = (part >> 1);
	}
	
	return x.value[0];
}
