
#include "virtual.def"
#include "stdio.h"
#include "dos.h"

#define uchar  unsigned char
#define uint   unsigned int
#define ulong  unsigned long


char  virtual_buf[VIRTUAL_BUF_SIZE];




// +------------------------------------+
// | Pripocti cislo k virtualnimu cislu |
// +------------------------------------+
void virtual_addend(ulong addend_num)
{
    ulong virt;
    ulong divident;
    uchar index;
    uchar char_cache;

    divident = 1;
    index = sizeof(virtual_buf);

    //printf("\nadd: %u\n", addend_num);
    //delay(10);

    while(index)                                            // dokud neni index na konci
    {
        index--;                                            // posun index

        virt = (uchar) ((addend_num / divident) % 10);      // nacti 1 cislici z cisla

        char_cache = virtual_buf[index];                    // nacti znak do cache

        if(char_cache >= '0')                               // konverze znaku na decimalni cislo
            char_cache -= '0';

        if(char_cache == ' ')                               // konverze mezer na nuly
            char_cache = 0;

        char_cache += (uchar) virt;                         // pripocti cislici

        if(char_cache > 9)                                  // pokud prekrocena hodnota 9
            if(index)                                       // pokud neni index na konci bufferu
                virtual_buf[index-1]++;                     // prenos do vyssiho radu

        char_cache %= 10;                                   // rotuj znak

        virtual_buf[index] = char_cache + '0';              // zapis znak z cache zpet do bufferu

        divident *= 10;                                     // posun delitete
    }

    /*
    for(index=0; index<13; index++)
    {
        printf("%c", virtual_buf[index]);
    }
    printf("\n");
    delay(100);
    */
}


// +----------------------------------------------+
// | Vrat pointer na virtualni cislo              |
// +----------------------------------------------+
//  mode == VIRTUAL_MODE_TRIM_ZEROS ... vynecha nuly na zacatku cisla
//
char *virtual_num_ptr(uchar mode)
{
    char *ptr;

    ptr = virtual_buf;

    switch(mode)
    {
        case SKIP_ZEROS:
            while(*ptr=='0')
                ptr++;                                    // inkrementuj pointer dokud je na nem znak nula
            break;

        case ZEROS_TO_SPACES:
            while(*ptr == '0')                            // dokud je na pointeru nula
            {
                //*ptr = ((*ptr == '0')?' ':(*ptr));        // konvertuj nuly na mezery
                //printf("\n%p: %c\n", ptr, *ptr);

                if(*ptr == '0')
                    (*ptr) = ' ';

                //printf("\n%p: %c\n", ptr, *ptr);

                ptr++;                                    // inkrementuj pointer
            }
            return(virtual_buf);                          // vrat pointer na zacatek buferu

        default:
            return(ptr);                                  // vrat pointer

    }

    return(ptr);                                  // vrat pointer
}


// +----------------------------------------------+
// | Pripocti opakovane cislo k virtualnimu cislu |
// +----------------------------------------------+
void virtual_add_product(ulong addend_num, uint multiple)
{
    for(; multiple; multiple--)
        virtual_addend(addend_num);
}




// +------------------------------------+
// | Inicializace                       |
// +------------------------------------+
void virtual_init()
{
    uchar      i;

    for(i=0; i<sizeof(virtual_buf); i++)
        virtual_buf[i]= '0';
}


