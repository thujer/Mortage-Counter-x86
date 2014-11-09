// +----------------------------------------------------------------------+
// |                  Mortage counter / DOS display driver                |
// |                  Thomas Hoodger (c) 2008                             |
// |                  thomas.hoodger(at)gmail.com                         |
// +----------------------------------------------------------------------+
    #define          APP_NAME           "Mortage counter"
    #define          APP_VERSION        "1.07"
    #define          APP_AUTHOR         "thomas.hoodger(at)gmail.com"
    #define          APP_BUILD          __DATE__
    #define          APP_COPYRIGHT      "Copyright (c) 2008"
// +----------------------------------------------------------------------+
// | History                                                              |
// |                                                                      |
// |   1.07          opravena chyba pretekani pri vypoctu poctu sekund    |
// |                 prislusneho dne                                      |
// |                                                                      |
// +----------------------------------------------------------------------+

#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <dos.h>
#include <io.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#include "dayweek.cpp"
#include "com.cpp"
#include "keyb.cpp"
#include "timeout.cpp"


// ------------------------------------------------------------------------
#define uchar unsigned char   // Zkraceni definice
#define uint  unsigned int    // Zkraceni definice
#define ulong unsigned long   // Zkraceni definice

// ------------------------------------------------------------------------

uchar  main_speed_selected;         // ID zvolene rychlosti
ulong  main_seconds_after_midnight; // pocet sekund od noveho dne

//=====================================================================


/*
// vytvoreni datumu v linux formatu z datumu 25.12.2002 18:15:00
// poèet milisekund od 1.1. 1970
toDate = new Date(2002, 12, 25, 18, 15, 0);

// definice funkce
function calcDiff()
{
    // nacteni aktualniho datumu
    _curr = new Date();

    // vypocet casoveho rozmezi mezi casem z datumu toDate a aktualnim casem
    // zjisti casovy rozdil v sekundach
    diff = (_curr.getTime() - toDate.getTime()) / 1000;

    // vypocet poctu mesicu od datumu toDate ( asi zbytecne, nikde se nevyuzije )
    months = (_curr.getYear() - 2002)*12 + _curr.getMonth() - toDate.getMonth()

    // Vypocet cisla
    // (cas v sekundach od 25.12.2002 18:15:00) * 3800 + 318000000000;
    num = diff * 3800 + 318000000000;

    // Vypis cisla zaokrouhleneho na nejblizsi cele cislo
    document.counter.num2.value = Math.round(num);

    // Nastaveni casoveho limitu v ms, po kterem je tato funkce zavolana znovu
    settimeout("calcDiff()",100);
}
*/



void main_app_footer()
{
    printf("\n\n----------------------------------\n");
    printf("--------  APP TERMINATED  --------\n");
    printf("----------------------------------\n");
    printf("       %s\n", APP_NAME);
    printf("       ver %s.%s\n", APP_VERSION, APP_BUILD);
    printf(" %s\n", APP_AUTHOR);
    printf("----------------------------------\n");
}


void main_app_header()
{
    printf("\n-----------------------------------\n");
    printf(" %s %s %s\n", APP_NAME, APP_VERSION, APP_BUILD);
    printf(" %s\n", APP_AUTHOR);
    printf("-----------------------------------\n");
    printf("ALT-X for Exit\n");
    //printf("Port speed %s\n\n\r", speed_list[main_speed_selected].text);
}


// +------------------------------------------------------------------------+
// | Inicializace COM portu                                                 |
// +------------------------------------------------------------------------+
void main_com_init(ulong speed)
{
    com_init_buf();
    com_set_speed(COM_ADDRESS, speed);
    com_set_properties(COM_ADDRESS, 3);                               // Nastaveni parametru prenosu (8 bit, 1 stopbit, parita none)
    com_set_interrupt(COM_ADDRESS, 0);                                // Nastaveni preruseni (vse vypnuto)
}


// +------------------------------------------------------------------------+
// | Pokousi se odeslat znak, pri preteceni timeout vrati 0xFF              |
// +------------------------------------------------------------------------+
uchar main_send_char(char ch)
{
    uint timeout;

    timeout = 100;  // cca 100ms
    while(!kbALT_X)
    {
        if(com_send_ready(COM_ADDRESS))
        {
            outport(COM_ADDRESS, ch);
            return(0);
        }
        else
            delay(1);

        timeout--;

        if(timeout==0)
            return(0xFF);

        if(kbhit())
        {
            keyb_get_key();

            if(kbESC || kbALT_X || kbENTER)
                return(1);
        }
    }
    return(1);
}



// +------------------------------------------------------------------------+
// | Show speed select menu                                                 |
// +------------------------------------------------------------------------+
void main_show_speed_select()
{
    uchar i;

    printf("-------------------------\n");
    printf("Port speed select [kbps]:\n");
    printf("Key speed                \n");

    for(i=1; i<(sizeof(speed_list) / sizeof(speed_list[0])); i++)
        printf("%2X: %s\n", i, speed_list[i].text);

    printf("-------------------------\n");
    printf("\r\n\n");
}


// +------------------------------------------------------------------------+
// | Key service                                                            |
// +------------------------------------------------------------------------+
void main_key_task()
{
    keyb_get_key();

    main_send_char(keyb_ascii);

    if((keyb_ascii >= '0') &&
       (keyb_ascii <= '7'))
    {
    	main_speed_selected = keyb_ascii - '0';
    	main_com_init(speed_list[main_speed_selected].value);
    	printf("\n\n\rPort speed changed to %s\n\r", speed_list[main_speed_selected].text);
    }

    if(kbESC)
        printf("\n\nALT+X for EXIT\n\n");
}


// +------------------------------------------------------------------------+
// | Event service                                                          |
// +------------------------------------------------------------------------+
void main_event_task(uchar event_id, uchar event_value)
{
    switch(event_id)
    {
        case EVENT_DATE_WORKING_FINISHED:
            main_seconds_after_midnight  = time_hour*60;      // preved hodiny na minuty
            main_seconds_after_midnight += time_minute;       // pripocti minuty
            main_seconds_after_midnight *= 60;                // preved na sekundy
            main_seconds_after_midnight += time_second;       // pripocti sekundy

            //printf("\nSeconds after midnight: %u\n", main_seconds_after_midnight);

            virtual_add_product(main_seconds_after_midnight, 3800);                                   // pridej k cislu 3800 * pocet sekund od pulnoci

            printf("\r Today is %dst %s %i, %s, ", date_day_target+1, month_days[date_month_target].title_english, date_year_target, date_week_days[date_day_in_week_counter].title_english);
            printf("current mortage: %13s\n\n", virtual_num_ptr(ZEROS_TO_SPACES));
            break;

        case EVENT_TIME_100MS:
            //printf("\n%i\n", time_msec_difference);
            //virtual_addend(time_hundreth);

            //printf("\ndays: %u\n", date_days_accumulator);            // vypis poctu dni od pocatecniho data

            //virtual_addend(event_value * 38);
            if(event_value);                                            // dead code

            virtual_addend(time_msec_difference * 38);

        	printf("\r %d.%d.%d %.2d:%.2d:%.2d.%.2d, mortage: %13s, refresh: %u ms  \r", date_day_target+1, date_month_target+1, date_year_target, time_hour, time_minute, time_second, time_msec, virtual_num_ptr(ZEROS_TO_SPACES), (uint) time_msec_difference*10);
        	com_send_char(COM_ADDRESS, 2);
        	com_send_string_direct(COM_ADDRESS, virtual_num_ptr(ZEROS_TO_SPACES), sizeof(virtual_buf));
        	com_send_char(COM_ADDRESS, 3);
            break;
    }
}


    // (cas v sekundach od 25.12.2002 18:15:00) * 3800 + 318000000000;
    // (86400 sekund za den) * 3800
    // prirustek dluhu: 328320000 denne
    /*
        8.  4.2008
     - 25. 12.2002
    ---------------
                 5
      leden  31
      unor   28
      brezen 31
              8

      cca 1868 dni od 25.12.2002
      dluh cca  613301760000 od 25.12.2002
      poc.dluh  318000000000
                ------------
      souc.dluh 931301760000

    */


// +------------------------------------------------------------------------+
// | Init promennych hlavniho programu                                      |
// +------------------------------------------------------------------------+
void main_init()
{
    main_seconds_after_midnight = 0;
    main_speed_selected = 4;
}


// +------------------------------------------------------------------------+
// | Main program                                                           |
// +------------------------------------------------------------------------+
void main()
{
    main_app_header();

    date_init();
    event_init();
    timeout_init();

    main_init();
    main_com_init(speed_list[main_speed_selected].value);

    virtual_init();
    virtual_add_product(318000000, 1000);                                  // pripocti k virtualnimu cislo cislo opakovane - pocatecni dluh

    printf("Mortage on 25.12.2002: %.13s\n\n", virtual_buf);

    main_show_speed_select();

    date_set_start_date(24, DECEMBER, 2002, WEDNESDAY);
    date_set_target_date(date_get_current_day(), date_get_current_month(), date_get_current_year());

    date_day_mortage_addend = 328320000;                                                              // nastav denni prirustek dluhu

    time_update();


    while(!kbALT_X)
    {
    	if(kbhit())
    	    main_key_task();

        if(event_ready)
            main_event_task(event_id, event_value);

        date_proc();
    	timeout_proc();
  	    event_proc();
    	com_proc(COM_ADDRESS);
    }

    main_app_footer();
}

