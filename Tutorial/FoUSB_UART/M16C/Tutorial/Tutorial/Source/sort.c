//-----------------------------------------------------------------------------
// C language specification test.
// Author:
//-----------------------------------------------------------------------------
#include "sort.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
init(struct Sample *p_sam)
{
	p_sam->s0 = 0;
	p_sam->s1 = 0;
	p_sam->s2 = 0;
	p_sam->s3 = 0;
	p_sam->s4 = 0;
	p_sam->s5 = 0;
	p_sam->s6 = 0;
	p_sam->s7 = 0;
	p_sam->s8 = 0;
	p_sam->s9 = 0;
}

sort(long *a)
{
 	long t;
	int i, j, k, gap;

	gap = 5;
	while( gap > 0 ){
		for( k=0; k<gap; k++){
			for( i=k+gap; i<10; i=i+gap ){
				for(j=i-gap; j>=k; j=j-gap){
					if(a[j]>a[j+gap]){
						t = a[j];
						a[j] = a[j+gap];
						a[j+gap] = t;
					}
					else
						break;
				}
			}
		}
		gap = gap/2;
	}
}

change(long *a)
{
	long tmp[10];
	int i;

	for(i=0; i<10; i++){
		tmp[i] = a[i];
	}
	for(i=0; i<10; i++){
		a[i] = tmp[9 - i];
	}
}
