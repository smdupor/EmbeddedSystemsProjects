/***********************************************************************/
/*                                                                     */
/*  FILE        :Tutorial.c                                          */
/*  DESCRIPTION :Main Program                                          */
/*                                                                     */
/*                                                                     */
/***********************************************************************/

#include "sort.h"
#include <stdlib.h>

void tutorial(void);

void main(void)
{
	while (1){
		tutorial();
	}
}

void tutorial(void)
{
	long a[10];
	long j;
	int i;
	struct Sample far *p_sam;

	p_sam= malloc(sizeof(struct Sample));
	init(p_sam);

	for( i=0; i<10; i++ ){
		j = rand(); 
		if(j < 0){
			j = -j;
		}
		a[i] = j;
	}
	sort(a);
	change(a);

	p_sam->s0=a[0];
	p_sam->s1=a[1];
	p_sam->s2=a[2];
	p_sam->s3=a[3];
	p_sam->s4=a[4];
	p_sam->s5=a[5];
	p_sam->s6=a[6];
	p_sam->s7=a[7];
	p_sam->s8=a[8];
	p_sam->s9=a[9];
	free(p_sam);
	p_sam = NULL;
}

void abort(void)
{
	
}
