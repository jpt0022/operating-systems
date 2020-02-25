/*
 * Project 5: address.h
 * This is a header file of address.c
 * Xiao Qin.
 */
#ifndef ADDRESS_H
#define ADDRESS_H

/* offset is used to address the location within a page */
#define OFFSET_BITS 8

/* mask is used to extract the offset from a logical address */ 
#define OFFSET_MASK 0xFF 

/* There is no byte data type in C */
typedef unsigned char byte;

/* new data type of pages, frames, and offset */
typedef unsigned int page_t;
typedef unsigned int frame_t;
typedef unsigned int offset_t;
typedef unsigned int laddress_t;
typedef unsigned int paddress_t;

int print_info();
/* 
 * In C language, there is no binary format in printf 
 * You must implement the following functions to print binary format 
 * itob16() and itob8() are modified from itob() by Xiao Qin.
 */
char *itob(int x);
char *itob16(int x);
char *itob8(int x);
#endif
