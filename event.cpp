
    /////////////////////////////////////////////////////////////////////////////
    // +---------------------------------------------------------------------+ //
    // |                              Event driver                           | //
    // |                     ------------------------------                  | //
    // |                               Verze 1.04
    // |                                                                     | //
    // |                         Copyright Thomas Hoodger                    | //
    // |                             (c) 2005-2008                           | //
    // |                      thomas.hoodger(at)gmail.com                    | //
    // |                                                                     | //
    // +---------------------------------------------------------------------+ //
    // |                                                                     | //
    // |  Uklada udalosti do bufferu udalosti, pokud je v bufferu udalost,   | //
    // |  nastavi priznak, priznak je zachycen v hlavni smycce a udalost     | //
    // |  nasledne vyzvednuta                                                | //
    // |                                                                     | //
    // +---------------------------------------------------------------------+ //
    //                                                                         //
    //  ---------------------------------------------------------------------  //
    //    Version history:                                                     //
    //                                                                         //
    //       1.01         Zakladni verze s jednorozmernym bufferem udalosti    //
    //                                                                         //
    //       1.02         Upraven buffer pro 2 hodnoty (udalost, hodnota)      //
    //                                                                         //
    //       1.03         Upraveno nacitani udalosti z bufferu, pridan flag    //
    //                    pripravene udalosti v promennych                     //
    //                                                                         //
    //       1.04         Drobne upravy                                        //
    //                                                                         //
    //  ---------------------------------------------------------------------  //
    //                                                                         //
    // ----------------------------------------------------------------------- //
    /////////////////////////////////////////////////////////////////////////////
#pragma LIST
#pragma PAGELENGTH(30000);
#pragma LINES

#define MAX_EVENTS_COUNT    5

#define uchar unsigned char
#define uint unsigned int

#include "event.def"
//#include "stdout.h"
//#include "stdout.def"

uint  event[MAX_EVENTS_COUNT];                    // buffer udalosti

uchar event_in_buffer;                            // pocet udalosti v bufferu
uchar event_index_write;                          // index pro zapis udalosti
uchar event_index_read;                           // index pro cteni udalosti

uchar event_id;                                   // globalni promenna pro predani udalosti
uchar event_value;                                // globalni promenna pro predani hodnoty udalosti

uchar event_overflow;                             // flag prepisu udalosti
uchar event_ready;                                // flag nactene udalosti z bufferu

// --------------------------------------------------------------------------------------

// +-------------------------------------+
// |   Odeslani udalosti do bufferu      |
// +-------------------------------------+
// Ulozi udalost do bufferu udalosti a upravi ukazatele
void event_send(uchar event_id, uchar event_value)
{
    if(event_in_buffer < MAX_EVENTS_COUNT)                          // Pokud neni buffer udalosti plny
    {
        event_index_write %= MAX_EVENTS_COUNT;                      // Pretoc index, je-li na max.
        event[event_index_write] = ((event_id << 8) | event_value); // zapis udalost a hodnotu udalosti do bufferu
        event_index_write++;                                        // Posun index
        event_in_buffer++;                                          // Zvys pocet udalosti v bufferu
    }
    else
    {
        event_overflow=1;                                           // Nastav flag preteceni bufferu
        //stdout_printf(EVENT_LOG, "!!! EVENT %i MISSED !!!\n", (int) event_id);
    }

}


// +--------------------------------------------------------+
// |   Proces nacteni udalosti do globalnich promennych     |
// +--------------------------------------------------------+
// Nacte nejstarsi udalost do globalnich promennych,
// upravi ukazatele v bufferu
void event_proc()
{
    uint evnt;

    if(event_in_buffer)                         // pokud je nejaka udalost v bufferu
    {
        event_index_read %= MAX_EVENTS_COUNT;   // pretoc index, je-li na max.
        evnt = event[event_index_read];         // nacti udalost z bufferu
        event[event_index_read] = 0;            // vymaz udalost v bufferu
        event_index_read++;                     // posun index
        event_in_buffer--;                      // dekrementuj pocet udalosti v bufferu

        event_ready = 1;                        // nastav flag cekajici udalosti, nactene z bufferu
        event_id = (evnt >> 8);                 // nacti udalost
        event_value = (evnt & 0xFF);            // nacti hodnotu udalosti
    }
    else
    {
        if(event_ready)
        {
            event_ready = 0;
            //event_send(EVENT_EVENT_BUFFER_FREE, 0);
        }
    }
}



// +-------------------------------------+
// |   Init promennych                   |
// +-------------------------------------+
void event_init()
{
    event_in_buffer = 0;
    event_index_write = 0;
    event_index_read = 0;
    event_overflow = 0;
    event_ready = 0;
    event_id = 0;                                 // globalni promenna pro predani udalosti
    event_value = 0;                              // globalni promenna pro predani hodnoty udalosti
}



