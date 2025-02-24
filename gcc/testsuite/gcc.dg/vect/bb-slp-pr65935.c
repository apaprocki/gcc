/* { dg-do run } */
/* { dg-additional-options "-O3" } */
/* { dg-require-effective-target vect_double } */

#include "tree-vect.h"

extern void abort (void);
extern void *malloc (__SIZE_TYPE__);

struct site {
    struct {
	struct {
	    double real;
	    double imag;
	} e[3][3];
    } link[32];
    double phase[32];
} *lattice;
int sites_on_node;

void rephase (void)
{
  int i,j,k,dir;
  struct site *s;
  for(i=0,s=lattice;i<sites_on_node;i++,s++)
    for(dir=0;dir<32;dir++)
      for(j=0;j<3;j++)for(k=0;k<3;k++)
	{
	  s->link[dir].e[j][k].real *= s->phase[dir];
	  s->link[dir].e[j][k].imag *= s->phase[dir];
	}
}

int main()
{
  int i,j,k;
  check_vect ();
  sites_on_node = 1;
  lattice = malloc (sizeof (struct site) * sites_on_node);
  for (i = 0; i < 32; ++i)
    {
      lattice->phase[i] = i;
      for (j = 0; j < 3; ++j)
	for (k = 0; k < 3; ++k)
	  {
	    lattice->link[i].e[j][k].real = 1.0;
	    lattice->link[i].e[j][k].imag = 1.0;
	    __asm__ volatile ("" : : : "memory");
	  }
    }
  rephase ();
  for (i = 0; i < 32; ++i)
    for (j = 0; j < 3; ++j)
      for (k = 0; k < 3; ++k)
	if (lattice->link[i].e[j][k].real != i
	    || lattice->link[i].e[j][k].imag != i)
	  abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-times "basic block vectorized" 1 "slp1" } } */
/* { dg-final { cleanup-tree-dump "slp1" } } */
/* { dg-final { cleanup-tree-dump "slp2" } } */
