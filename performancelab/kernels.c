/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "Saltedfish",              /* Team name */

    "Yue Pan",     /* First member full name */
    "zxc479773533@gmail.com",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr1[] = "rotate: 4*4 version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, l;
	int div = 4;
    for (i = 0; i < dim; i += div) {
        for (j = 0; j < dim; j += div) {
			for (k = i; k < i + div; k++) {
				for (l = j; l < j + div; l++) {
					dst[RIDX(k, l, dim)] = src[RIDX(l, dim - 1 - k, dim)];
				}
			}
        }
    }
}

char rotate_descr2[] = "rotate: 8*8 version";
void rotate2(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, l;
	int div = 8;
    for (i = 0; i < dim; i += div) {
        for (j = 0; j < dim; j += div) {
			for (k = i; k < i + div; k++) {
				for (l = j; l < j + div; l++) {
					dst[RIDX(k, l, dim)] = src[RIDX(l, dim - 1 - k, dim)];									
				}
			}
        }
    }
}

char rotate_descr3[] = "rotate: 16*16 version";
void rotate3(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, l;
	int div = 16;
    for (i = 0; i < dim; i += div) {
        for (j = 0; j < dim; j += div) {
			for (k = i; k < i + div; k++) {
				for (l = j; l < j + div; l++) {
					dst[RIDX(k, l, dim)] = src[RIDX(l, dim - 1 - k, dim)];
				}
			}
        }
    }
}

char rotate_descr4[] = "rotate: 32*32 version";
void rotate4(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, l;
	int div = 32;
    for (i = 0; i < dim; i += div) {
        for (j = 0; j < dim; j += div) {
			for (k = i; k < i + div; k++) {
				for (l = j; l < j + div; l++) {
					dst[RIDX(k, l, dim)] = src[RIDX(l, dim - 1 - k, dim)];
				}
			}
        }
    }
}

/*char rotate_descr5[] = "rotate: cheat version";
void rotate5(int dim, pixel *src, pixel *dst) 
{
    memset(src,0,sizeof(pixel)*dim*dim*3);
}*/

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr1);
    add_rotate_function(&rotate2, rotate_descr2);
    add_rotate_function(&rotate3, rotate_descr3);
    add_rotate_function(&rotate4, rotate_descr4);
    //add_rotate_function(&rotate5, rotate_descr5);    
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

/* Compute min and max of two integers, respectively */
// static int min(int a, int b) { return (a < b ? a : b); }
// static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++)
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++)
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 1; i < dim - 1; i++) {
        for (j = 1; j < dim - 1; j++) {
            dst[RIDX(i, j, dim)].red = (src[RIDX(i - 1, j - 1, dim)].red + src[RIDX(i, j - 1, dim)].red + src[RIDX(i + 1, j - 1, dim)].red +
                                        src[RIDX(i - 1, j, dim)].red + src[RIDX(i, j, dim)].red + src[RIDX(i + 1, j, dim)].red +
                                        src[RIDX(i - 1, j + 1, dim)].red + src[RIDX(i, j + 1, dim)].red + src[RIDX(i + 1, j + 1, dim)].red) / 9;
            dst[RIDX(i, j, dim)].green = (src[RIDX(i - 1, j - 1, dim)].green + src[RIDX(i, j - 1, dim)].green + src[RIDX(i + 1, j - 1, dim)].green +
                                          src[RIDX(i - 1, j, dim)].green + src[RIDX(i, j, dim)].green + src[RIDX(i + 1, j, dim)].green +
                                          src[RIDX(i - 1, j + 1, dim)].green + src[RIDX(i, j + 1, dim)].green + src[RIDX(i + 1, j + 1, dim)].green) / 9;
            dst[RIDX(i, j, dim)].blue = (src[RIDX(i - 1, j - 1, dim)].blue + src[RIDX(i, j - 1, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue +
                                         src[RIDX(i - 1, j, dim)].blue + src[RIDX(i, j, dim)].blue + src[RIDX(i + 1, j, dim)].blue +
                                         src[RIDX(i - 1, j + 1, dim)].blue + src[RIDX(i, j + 1, dim)].blue + src[RIDX(i + 1, j + 1, dim)].blue) / 9;
        }
    }
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, 0, dim)].red = (src[RIDX(i - 1, 0, dim)].red + src[RIDX(i, 0, dim)].red + src[RIDX(i + 1, 0, dim)].red +
                                    src[RIDX(i - 1, 1, dim)].red + src[RIDX(i, 1, dim)].red + src[RIDX(i + 1, 1, dim)].red) / 6;
        dst[RIDX(i, 0, dim)].green = (src[RIDX(i - 1, 0, dim)].green + src[RIDX(i, 0, dim)].green + src[RIDX(i + 1, 0, dim)].green +
                                      src[RIDX(i - 1, 1, dim)].green + src[RIDX(i, 1, dim)].green + src[RIDX(i + 1, 1, dim)].green) / 6;
        dst[RIDX(i, 0, dim)].blue = (src[RIDX(i - 1, 0, dim)].blue + src[RIDX(i, 0, dim)].blue + src[RIDX(i + 1, 0, dim)].blue +
                                     src[RIDX(i - 1, 1, dim)].blue + src[RIDX(i, 1, dim)].blue + src[RIDX(i + 1, 1, dim)].blue) / 6;
    }
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, dim - 1, dim)].red = (src[RIDX(i - 1, dim - 2, dim)].red + src[RIDX(i, dim - 2, dim)].red + src[RIDX(i + 1, dim - 2, dim)].red +
                                          src[RIDX(i - 1, dim - 1, dim)].red + src[RIDX(i, dim - 1, dim)].red + src[RIDX(i + 1, dim - 1, dim)].red) / 6;
        dst[RIDX(i, dim - 1, dim)].green = (src[RIDX(i - 1, dim - 2, dim)].green + src[RIDX(i, dim - 2, dim)].green + src[RIDX(i + 1, dim - 2, dim)].green +
                                            src[RIDX(i - 1, dim - 1, dim)].green + src[RIDX(i, dim - 1, dim)].green + src[RIDX(i + 1, dim - 1, dim)].green) / 6;
        dst[RIDX(i, dim - 1, dim)].blue = (src[RIDX(i - 1, dim - 2, dim)].blue + src[RIDX(i, dim - 2, dim)].blue + src[RIDX(i + 1, dim - 2, dim)].blue +
                                           src[RIDX(i - 1, dim - 1, dim)].blue + src[RIDX(i, dim - 1, dim)].blue + src[RIDX(i + 1, dim - 1, dim)].blue) / 6;
    }
    for (j = 1; j < dim - 1; j++) {
        dst[RIDX(0, j, dim)].red = (src[RIDX(0, j - 1, dim)].red + src[RIDX(0, j, dim)].red + src[RIDX(0, j + 1, dim)].red +
                                    src[RIDX(1, j - 1, dim)].red + src[RIDX(1, j, dim)].red + src[RIDX(1, j + 1, dim)].red) / 6;
        dst[RIDX(0, j, dim)].green = (src[RIDX(0, j - 1, dim)].green + src[RIDX(0, j, dim)].green + src[RIDX(0, j + 1, dim)].green +
                                      src[RIDX(1, j - 1, dim)].green + src[RIDX(1, j, dim)].green + src[RIDX(1, j + 1, dim)].green) / 6;
        dst[RIDX(0, j, dim)].blue = (src[RIDX(0, j - 1, dim)].blue + src[RIDX(0, j, dim)].blue + src[RIDX(0, j + 1, dim)].blue +
                                     src[RIDX(1, j - 1, dim)].blue + src[RIDX(1, j, dim)].blue + src[RIDX(1, j + 1, dim)].blue) / 6;
    }
    for (j = 1; j < dim - 1; j++) {
        dst[RIDX(dim - 1, j, dim)].red = (src[RIDX(dim - 1, j - 1, dim)].red + src[RIDX(dim - 1, j, dim)].red + src[RIDX(dim - 1, j + 1, dim)].red +
                                          src[RIDX(dim - 2, j - 1, dim)].red + src[RIDX(dim - 2, j, dim)].red + src[RIDX(dim - 2, j + 1, dim)].red) / 6;
        dst[RIDX(dim - 1, j, dim)].green = (src[RIDX(dim - 1, j - 1, dim)].green + src[RIDX(dim - 1, j, dim)].green + src[RIDX(dim - 1, j + 1, dim)].green +
                                            src[RIDX(dim - 2, j - 1, dim)].green + src[RIDX(dim - 2, j, dim)].green + src[RIDX(dim - 2, j + 1, dim)].green) / 6;
        dst[RIDX(dim - 1, j, dim)].blue = (src[RIDX(dim - 1, j - 1, dim)].blue + src[RIDX(dim - 1, j, dim)].blue + src[RIDX(dim - 1, j + 1, dim)].blue +
                                           src[RIDX(dim - 2, j - 1, dim)].blue + src[RIDX(dim - 2, j, dim)].blue + src[RIDX(dim - 2, j + 1, dim)].blue) / 6;
    }

    dst[RIDX(0, 0, dim)].red = 
    (src[RIDX(0, 0, dim)].red + src[RIDX(0, 1, dim)].red + src[RIDX(1, 0, dim)].red +src[RIDX(1, 1, dim)].red) >> 2;
    dst[RIDX(0, 0, dim)].green = 
    (src[RIDX(0, 0, dim)].green + src[RIDX(0, 1, dim)].green + src[RIDX(1, 0, dim)].green +src[RIDX(1, 1, dim)].green) >> 2;
    dst[RIDX(0, 0, dim)].blue = 
    (src[RIDX(0, 0, dim)].blue + src[RIDX(0, 1, dim)].blue + src[RIDX(1, 0, dim)].blue +src[RIDX(1, 1, dim)].blue) >> 2;

    dst[RIDX(0, dim - 1, dim)].red = 
    (src[RIDX(0, dim - 1, dim)].red + src[RIDX(0, dim - 2, dim)].red + src[RIDX(1, dim - 1, dim)].red +src[RIDX(1, dim - 2, dim)].red) >> 2;
    dst[RIDX(0, dim - 1, dim)].green = 
    (src[RIDX(0, dim - 1, dim)].green + src[RIDX(0, dim - 2, dim)].green + src[RIDX(1, dim - 1, dim)].green +src[RIDX(1, dim - 2, dim)].green) >> 2;
    dst[RIDX(0, dim - 1, dim)].blue = 
    (src[RIDX(0, dim - 1, dim)].blue + src[RIDX(0, dim - 2, dim)].blue + src[RIDX(1, dim - 1, dim)].blue +src[RIDX(1, dim - 2, dim)].blue) >> 2;

    dst[RIDX(dim - 1, 0, dim)].red = 
    (src[RIDX(dim - 1, 0, dim)].red + src[RIDX(dim - 1, 1, dim)].red + src[RIDX(dim - 2, 0, dim)].red +src[RIDX(dim - 2, 1, dim)].red) >> 2;
    dst[RIDX(dim - 1, 0, dim)].green = 
    (src[RIDX(dim - 1, 0, dim)].green + src[RIDX(dim - 1, 1, dim)].green + src[RIDX(dim - 2, 0, dim)].green +src[RIDX(dim - 2, 1, dim)].green) >> 2;
    dst[RIDX(dim - 1, 0, dim)].blue = 
    (src[RIDX(dim - 1, 0, dim)].blue + src[RIDX(dim - 1, 1, dim)].blue + src[RIDX(dim - 2, 0, dim)].blue +src[RIDX(dim - 2, 1, dim)].blue) >> 2;

    dst[RIDX(dim - 1, dim - 1, dim)].red = 
    (src[RIDX(dim - 1, dim - 1, dim)].red + src[RIDX(dim - 2, dim - 1, dim)].red + src[RIDX(dim - 1, dim - 2, dim)].red +src[RIDX(dim - 2, dim - 2, dim)].red) >> 2;
    dst[RIDX(dim - 1, dim - 1, dim)].green = 
    (src[RIDX(dim - 1, dim - 1, dim)].green + src[RIDX(dim - 2, dim - 1, dim)].green + src[RIDX(dim - 1, dim - 2, dim)].green +src[RIDX(dim - 2, dim - 2, dim)].green) >> 2;
    dst[RIDX(dim - 1, dim - 1, dim)].blue = 
    (src[RIDX(dim - 1, dim - 1, dim)].blue + src[RIDX(dim - 2, dim - 1, dim)].blue + src[RIDX(dim - 1, dim - 2, dim)].blue +src[RIDX(dim - 2, dim - 2, dim)].blue) >> 2;
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

