#include <stdio.h>
#include <math.h>

float  __attribute__((annotate("target('global') scalar( range(0, 10000) location(4) )"))) global = 33.333;

// with declaration here we're not able to get the location in LLVM, LOCATION NEEDED HERE!
float fun(
        float __attribute__((annotate("target('x') scalar( range(0, 1000) location(8) )"))) x,
        float __attribute__((annotate("target('y') scalar( range(0, 1000) location(9) )"))) y
)
{
    float __attribute__((annotate("target('local') scalar( range(0, 1000) declaration )"))) local;
    local = x * y + global;
    global++;
    printf("%f\n",x);
    return local;
}

/*int funInt(float x __attribute((annotate("scalar()"))),
           float y __attribute((annotate("scalar()")))){
    int local;
    local = x * y + global;
    global*=1.098;
    return local;
}*/

int main() {
    float __attribute__((annotate("target('a') scalar( range(0, 1000) declaration )"))) a=10.2049; //location(24)
    float __attribute__((annotate("target('b') scalar( range(0, 1000) declaration )"))) b=10.1024;
    int c = 2;


    a = fun(fun(a,b),a);
    printf("%f\n",a);

    a = fun(a,b);
    printf("%f\n",a);

    a = fun(b,b);
    printf("%f\n",a);

    a = fun(a,a);
    printf("%f\n",a);



    // ------------------ //

    a = a/4000;

    b = fun(b,b);
    printf("%f\n",b);

    b = fun(a,b);
    printf("%f\n",b);

    b = fun(b,a);
    printf("%f\n",b);

    b = fun(a,a);
    printf("%f\n",b);


    // ----------------- //

    b = a/4096;

    c = fun(b,b);
    printf("%d\n",c);

    c = fun(b,b);
    printf("%d\n",c);

    c = fun(b,a);
    printf("%d\n",c);

    c = fun(a,b);
    printf("%d\n",c);


    // ------------------ //


    printf("*******************\n");


    a = sqrt(b);

    float __attribute__((annotate("target('temp') scalar( range(0, 1000) declaration )"))) temp = a * 9.99;

    b = exp(temp);


    b = a;

    printf("%d\n",c);


    printf("-------------------\n");
    return 0;
}