#include <stdio.h>
#include <math.h>

extern double my_sin(double x);

int main()
{
	double x;
	scanf("%lf", &x);
	printf("%lf\n", my_sin(x));
	//printf("%lf", sin(x));
}

