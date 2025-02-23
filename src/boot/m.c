#include<stdio.h>
typedef unsigned int uint32_tw;
int main(){
    uint32_tw a=0xffffffff;
    printf("zzzz:%u\n",a);
    printf("bbbb:%x\n",a);

    uint32_tw q1 = 0x123456;
    uint32_tw q2 = 0x345678;
    uint32_tw q3 = q2 - q1;
    uint32_tw q5 = q1 + q3;
    printf("q5:%x\n",q5);

    uint32_tw q6 =  q1 - q2;
    printf("q6:%x\n",q6);

    uint32_tw q7 =  q2 + q6;
    printf("q7:%x\n",q7);
    
    return 0;
}