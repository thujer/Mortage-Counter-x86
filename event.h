

#ifndef EVENT__H
    #define EVENT__H

    #define uchar unsigned char
    #define uint unsigned int

    extern void         event_send(uchar event_id, uchar event_value);
    extern void         event_init();
    extern void         event_proc();

    extern uchar idata  event_in_buffer;               // pocet udalosti v bufferu
    extern bit          event_overflow;                // priznak prepisu udalosti
    extern bit          event_ready;                   // flag pripravene udalosti

    extern uchar idata  event_id;                      // globalni promenna pro predani udalosti
    extern uchar idata  event_value;                   // globalni promenna pro predani hodnoty udalosti

#endif
