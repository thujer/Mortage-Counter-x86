
#define uchar unsigned char
#define uint  unsigned int

uchar  time_hour;                      // aktualni hodina
uchar  time_minute;                    // aktualni minuta
uchar  time_second;                    // aktualni sekunda
uchar  time_msec;                      // aktualni milisekunda

uint   time_msec_temp;                 // komparacni hodnota pro ms
uint   time_msec_current;              // aktualni pocet ms
uint   time_msec_difference;           // rozdil casu v ms

uchar  time_100ms_temp;                // registr pro porovnani noveho stavu




void time_update()
{
  asm{
    mov ah, 0x2C
    int 0x21
    mov time_hour,   ch
    mov time_minute, cl
    mov time_second, dh
    mov time_msec,   dl
  }
}


void timeout_init()
{
    time_hour = 0;
    time_minute = 0;
    time_second = 0;
    time_msec   = 0;
    time_msec_temp = 0;
    time_msec_difference = 0;
}



void timeout_proc()
{
    time_update();

    time_msec_current = time_msec + (time_second*100);                                             // preved aktualni pocet msec a sekund na msec

    //printf("\nmsec_curr:%d\n", time_msec_current);

    if(time_100ms_temp != (time_msec_current & 16))
    {
        //printf("\nmsec_curr:%d\n", time_msec_current);

        if(time_msec_temp < time_msec_current)
            time_msec_difference = time_msec_current - time_msec_temp;
        else
            time_msec_difference = (6000 - time_msec_temp) + time_msec_current;

        //printf("\n%d ms:%d[%d]\n", time_msec_difference, time_msec_current, time_msec_temp);

        time_msec_temp = time_msec_current;
        time_100ms_temp = (time_msec_current & 16);

        event_send(EVENT_TIME_100MS, time_msec_difference);
    }
}

