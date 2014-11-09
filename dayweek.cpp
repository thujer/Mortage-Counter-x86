
    /////////////////////////////////////////////////////////////////////////////
    // +---------------------------------------------------------------------+ //
    // |                                Dayweek                              | //
    // |                     ------------------------------                  | //
    // |                               Verze 1.01                            | //
    // |                                                                     | //
    // |                         Copyright Thomas Hoodger                    | //
    // |                             (c) 2005-2008                           | //
    // |                      thomas.hoodger(at)gmail.com                    | //
    // |                                                                     | //
    // +---------------------------------------------------------------------+ //
    // | Vypocet dne v tydnu ze znameho data pomoci algoritmu simulace       | //
    // | prubehu casu. Je mozne nastavit promennou, ktera je inkrementovana  | //
    // | ve smycce kazdy den (napr. pro vypocet uroku, dluhu)                | //
    // |                                                                     | //
    // |                                                                     | //
    // +---------------------------------------------------------------------+ //
    //                                                                         //
    //  ---------------------------------------------------------------------  //
    //    Version history:                                                     //
    //                                                                         //
    //       1.01         Zakladni verze                                       //
    //                                                                         //
    //                                                                         //
    //  ---------------------------------------------------------------------  //
    //                                                                         //
    // ----------------------------------------------------------------------- //
    /////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "event.cpp"
#include "virtual.cpp"

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

ulong   date_day_mortage_addend;
ulong   date_days_accumulator;

// ------------------------------------------------------------------------------------------------
// Struktura tydne, definice nazvu jednotlivych dni v tydnu
typedef struct t_week_days {
    const char  *title_czech;
    const char  *title_english;
};


t_week_days   date_week_days[] = {
    "Pondeli",  "Monday",
    "Utery",    "Tuesday",
    "Streda",   "Wednesday",
    "Ctvrtek",  "Thurday",
    "Patek",    "Friday",
    "Sobota",   "Saturday",
    "Nedele",   "Sunday",
};


// Definice ID dnu v tydnu
typedef enum {
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY,
};


// Definice ID mesicu v roce
typedef enum {
    JANUARY,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER,
};


// ------------------------------------------------------------------------------------------------
// Struktura mesice, definice nazvu a poctu dni jednotlivych mesicu
typedef struct t_month_days {
    const char  *title_czech;
    const char  *title_english;
    uchar        day_count;
};


t_month_days month_days[] = {
    "Leden",    "January",    31,
    "Unor",     "February",   28,
    "Brezen",   "March",      31,
    "Duben",    "April",      30,
    "Kveten",   "May",        31,
    "Cerven",   "June",       30,
    "Cervenec", "July",       31,
    "Srpen",    "August",     31,
    "Zari",     "September",  30,
    "Rijen",    "October",    31,
    "Listopad", "November",   30,
    "Prosinec", "December",   31,
};

// ------------------------------------------------------------------------------------------------
// Tabulka prestupnych roku [2002,2003,...]
uchar date_leap_year_tab[]={
    0, 0, 1, 0,          // 2002 2003 2004 2005
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
};

// ------------------------------------------------------------------------------------------------
#define DATE_MONTHS_IN_YEAR   12
#define DATE_DAYS_IN_WEEK      7

// ------------------------------------------------------------------------------------------------
// Pocatecni datum - nutno doplnit spravny den v tydnu
#define DATE_DAYW_START      TUESDAY
#define DATE_DAY_START       1
#define DATE_MONTH_START     1
#define DATE_YEAR_START      1980

// ------------------------------------------------------------------------------------------------
// Cilove udaje
uint  date_year_target;                  // cilovy rok
uchar date_day_target;                   // cilovy den
uchar date_month_target;                 // cilovy mesic

// ------------------------------------------------------------------------------------------------
// Pocatecni udaje
uint  date_year_start;                   // pocatecni rok

// ------------------------------------------------------------------------------------------------
// Citace
uint  date_year_counter;                 // citac roku
uchar date_day_in_week_counter;                 // citac dni v tydnu
uchar date_day_in_month_counter;                  // citac dni v mesici
uchar date_month_counter;                // citac mesicu v roce
uchar date_month_day_count;              // pocet dni v mesici

uchar date_proc_state;


// ------------------------------------------------------------------------------------------------


// +---------------------------------------------------------------------+
// | Vrati aktualni rok                                                  |
// +---------------------------------------------------------------------+
uint date_get_current_year()
{
    asm{
        MOV  AH,0x2A
        INT  0x21
    }
    return(_CX);
}


// +---------------------------------------------------------------------+
// | Vrati aktualni mesic                                                |
// +---------------------------------------------------------------------+
uchar date_get_current_month()
{
    asm{
        MOV  AH,0x2A
        INT  0x21
    }
    return(_DH-1);
}


// +---------------------------------------------------------------------+
// | Vrati aktualni den                                                  |
// +---------------------------------------------------------------------+
uchar date_get_current_day()
{
    asm{
        MOV  AH,0x2A
        INT  0x21
    }
    return(_DL-1);
}



// +---------------------------------------------------------------------+
// | Vrati pocet dni v mesici s ohledem na prestupny rok                 |
// +---------------------------------------------------------------------+
uchar date_get_month_day_count(uchar month, uint year)
{
    if((date_leap_year_tab[year]) &&               // pokud je rok prestupny
       (month == 1))                               // a mesic je unor
    {
        return(29);                                // vrat 29 dni
    }
    else
    {
        return(month_days[month].day_count);       // vrat pocet dni mesice z tabulky
    }
}


// +---------------------------------------------------------------------+
// | Nastaveni ciloveho datumu                                           |
// +---------------------------------------------------------------------+
void date_set_target_date(uchar day, uchar month, uint year)
{
    date_day_target    = day;
    date_month_target  = month;
    date_year_target   = year;

    date_proc_state = 1;                                          // spusteni procesu
}


void date_set_start_date(uchar day, uchar month, uint year, uchar dayw)
{
    date_day_in_week_counter  = dayw;
    date_year_counter  = date_year_start = year;
    date_month_counter = month;
    date_day_in_month_counter   = day;
}



void date_proc()
{
    if(date_proc_state)
    {
        if(date_year_counter <= date_year_target)
        {
            //printf("\n----------------------------------  %i/%i  -----------------------------------", date_year_counter, date_year_target);
            if(date_month_counter < DATE_MONTHS_IN_YEAR)
            {
                //printf("\n%10s ", month_days[date_month_counter].title);

                // vrat pocet dni v mesici s ohledem na prestupny rok
                date_month_day_count = date_get_month_day_count(date_month_counter, (date_year_counter - date_year_start));

                // dokud neni citac dni na konci mesice
                while(date_day_in_month_counter < date_month_day_count)
                {
                    //printf("%.1s ", date_dayw_strings[date_day_in_week_counter]);

                    // Pokud jsou citace rovny cilovemu datumu, ukonci smycku
                    if((date_year_counter           == date_year_target  )     &&
                       (date_month_counter          == date_month_target )     &&
                       (date_day_in_month_counter   == date_day_target   ))
                    {
                        date_proc_state = 0;
                        event_send(EVENT_DATE_WORKING_FINISHED, 0);
                        break;
                    }

                    //printf("\nadd: %u\n", date_day_mortage_addend);
                    //delay(10);

                    virtual_addend(date_day_mortage_addend);

                    date_day_in_month_counter++;                                 // dalsi den v mesici
                    date_day_in_week_counter++;                                  // dalsi den v tydnu
                    date_day_in_week_counter %= DATE_DAYS_IN_WEEK;               // rotuj den v tydnu

                    date_days_accumulator++;
                }

                //if(!date_proc_state)
                //    break;

                date_day_in_month_counter = 0;
                date_month_counter++;

                //delay(100);
            }
            else
            {
                date_month_counter = 0;
                date_year_counter++;
            }
        }
        else
        {
            event_send(EVENT_DATE_WORKING_FINISHED, 1);
            date_proc_state = 0;
        }
    }
}


void date_init()
{
    date_year_target        = DATE_YEAR_START;
    date_day_target         = DATE_DAY_START;
    date_month_target       = DATE_MONTH_START;
    date_year_start         = DATE_YEAR_START;
    date_year_counter       = DATE_YEAR_START;
    date_day_in_week_counter       = DATE_DAYW_START;
    date_day_in_month_counter        = DATE_DAY_START;
    date_month_counter      = DATE_MONTH_START;
    date_month_day_count    = 31;
    date_proc_state         = 0;

    date_day_mortage_addend = 0;
    date_days_accumulator   = 0;
}


// +---------------------------------------------------------------------+
// | Hlavni program                                                      |
// +---------------------------------------------------------------------+
/*
void main()
{
    date_set_start_date(0, JANUARY, 1980, TUESDAY);
    //date_set_start_date(0, JANUARY, 2008, TUESDAY);
    date_set_target_date(date_get_current_day(), date_get_current_month(), date_get_current_year());
    //date_set_target_date(6, 3, 2008);

    while(1)
    {
        if(event_ready)
            main_event_working(event_id, event_value);

        date_proc();
        event_proc();
    }
}

*/
