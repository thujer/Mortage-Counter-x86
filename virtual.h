
#ifndef __VIRTUAL___
    #define __VIRTUAL___
    #include "virtual.def"

    #define ulong unsigned long

    extern void  virtual_add_product(ulong addend_num, uint multiple);
    extern void  virtual_addend(ulong addend_num);
    extern char *virtual_num_ptr(uchar mode);
    extern void  virtual_init();

    //extern char  virtual_buf[VIRTUAL_BUF_SIZE];

#endif

