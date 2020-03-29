#include <math.h>
#include <stdlib.h>
#define LOG2 0.6931471805
#define LN10 2.3025850929940456840179914546844

static int most_sig_bit(int n)
{
    if (n == 0)
        return 0;
 
    int msb = 0;
    while (n != 0) {
        n = n / 2;
        msb++;
    }
 
    return msb;
}

// This ln was made by yaaaaaaaaqweer big tnx :D
double log(double x)
{
    if(x >= 1 && x <= 2)
        return (-1.941064448+(3.529305040+(-2.461222169+(1.130626210+(-0.2887399591+0.03110401824*x)*x)*x)*x)*x);
    else if(x < 1 && x > 0)
    {
        x = 1 - x;
        return -1 * (x + x*x/2 + x*x*x/3 + x*x*x*x/4 + x*x*x*x*x/5);
    }
    else if(x <= 0)
        abort();
    // 2n≤x≤2n+1, ln(x)=nln(2)+ln(x/2n).
    int _bit;
    _bit = most_sig_bit((int)x);
    return _bit * LOG2 + my_ln(x/POW2(_bit));
 
}

double log10 (double x) {
    return log(x) / LN10;
}