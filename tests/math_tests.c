#include <assert.h>
#include <math.h>
#include "mathTool.h"
int main(void) {
    const float values[]={1.0e-30f,0.00001f,0.25f,1.0f,2.0f,100.0f,1.0e30f};
    for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);i++) {
        float expected=1.0f/sqrtf(values[i]);
        assert(fabsf(my_sqrt_reciprocal(values[i])-expected)<=fabsf(expected)*1.0e-6f);
    }
    assert(my_sqrt_reciprocal(0)==0);assert(my_sqrt_reciprocal(-1)==0);
    assert(my_sqrt_reciprocal(NAN)==0);assert(my_sqrt_reciprocal(INFINITY)==0);
    return 0;
}
