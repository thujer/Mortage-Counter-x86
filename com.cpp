
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

#define COM_RECBUFSIZE  32           // Velikost RecBuf
#define COM_SNDBUFSIZE  32           // Velikost SndBuf
#define COM_SDELAY       5
#define COM_ADDRESS     0x3F8


typedef struct t_speed_list {
    ulong  value;
    char  *text;
};

t_speed_list speed_list[] = {
    1200,   "1200",                     // 0
    2400,   "2400",                     // 1
    4800,   "4800",                     // 2
    9600,   "9600",                     // 3
    19200,  "19200 (default)",          // 4
    38400,  "38400",                    // 5
    57600,  "57600",                    // 6
    115200, "115200"                    // 7
};


uchar com_ribuf;                     // Priznak znaku v bufferu
uchar com_rw_ix;                     // Index pro zapis do RecBuf
uchar com_rr_ix;                     // Index pro cteni z RecBuf
uchar com_sw_ix;                     // Index pro zapis do SndBuf
uchar com_sr_ix;                     // Index pro cteni z SndBuf
uchar com_rbib;                      // Pocet byte v RecBuf
uchar com_sbib;                      // Pocet byte v SndBuf
uchar com_sdelay;                    // Pocet projeti smycky pred odeslanim znaku z SndBuf
uchar com_rbyte;                     // Znak nacteny z RecBuf

uchar com_rec_buf[COM_RECBUFSIZE];   // RecBuf
uchar com_snd_buf[COM_SNDBUFSIZE];   // SndBuf

//=====================================================================
void  com_init_buf();
uchar com_write_char(uchar ch);
uchar com_test_rec_char(uint port_base);
void  com_send_string(uchar *text);
void  com_proc(uint port_base);
uint  com_get_base(uchar port, uint userportadr);
void  com_set_speed(uint port_base, ulong baud);
void  com_set_properties(uint port_base, uchar portset);
void  com_set_interrupt(uint port_base, uchar portset);
uchar com_get_interrupt(uint port_base);
void  com_set_dtr(uint port_base, uchar status);
uchar com_get_error(uint port_base);
uchar com_send_ready(uint port_base);
uchar com_char_ready(uint port_base);
uchar com_get_char(uint port_base);
uchar com_send_char(uint port_base, uchar byte);
uchar com_get_port_status(uint port_base);
uchar com_overrun_error(uint port_base);
uchar com_parity_error(uint port_base);
uchar com_stopbit_error(uint port_base);
uchar com_always_zero(uint port_base);
void  com_set_fifo(uint port_base, uchar status);
//=====================================================================

//------------------------------------------------------------------------
// Init ukazatelu RecBuf a SndBuf, nulovani obou bufferu
//------------------------------------------------------------------------
void com_init_buf()
{
    uchar i;

    com_ribuf=0;
    com_rw_ix=0;
    com_rr_ix=0;
    com_sw_ix=0;
    com_sr_ix=0;
    com_rbib=0;
    com_sbib=0;
    com_rbyte=0;
    com_sdelay=COM_SDELAY;

    for(i=0; i<sizeof(com_rec_buf); i++)
        com_rec_buf[i]=0;

    for(i=0; i<sizeof(com_snd_buf); i++)
        com_snd_buf[i]=0;
}


//------------------------------------------------------------------------
// Zapis retezce do SndBuf (vysilaci fronty),
// pokud je retezec delsi, odesle se jen tolik co se vejde do bufferu
//------------------------------------------------------------------------
void com_send_string(uchar *text)
{
    uchar i=0;

    while(text[i])
    {
        if(i < (sizeof(com_snd_buf)-com_sbib))
            com_write_char(text[i]);
        else
            break;
        i++;
    }
}


//------------------------------------------------------------------------
// Odeslani retezce (s cekanim)
//------------------------------------------------------------------------
void com_send_string_direct(uint port_base, uchar *text, uchar text_size)
{
    uchar i=0;

    while(text[i] && (i<text_size))
    {
        if(com_send_ready(port_base))  // 0 = Vysilac je prazdny a pripraven vysilat
        {
            com_send_char(port_base, text[i]);
            i++;

            if(!text[i])
                break;
        }
    }
}


//------------------------------------------------------------------------
// Zapis znak do SndBuf (vysilaci fronty)
//------------------------------------------------------------------------
uchar com_write_char(uchar ch)
{
    if(com_sw_ix >= COM_SNDBUFSIZE)
        com_sw_ix=0;   // Pripadne vrat index

    if(com_sbib < (COM_SNDBUFSIZE-1))
    {
        com_snd_buf[com_sw_ix] = ch;              // Zapis znak do sndbuf
        com_sw_ix++;                              // Posun index
        com_sbib++;                               // Zvys pocet znaku v sndbuf
        return(1);
    }
    else
        return(0);
}


//------------------------------------------------------------------------
// Test na prichozi znak, pokud je pripraven, zapis ho do RecBuf
//------------------------------------------------------------------------
uchar com_test_rec_char(uint port_base)
{
    if(com_char_ready(port_base))
    {
        if(com_rbib < COM_RECBUFSIZE)                    // Pokud neni rbuf plny
        {
            if(com_rw_ix >= COM_RECBUFSIZE)
                com_rw_ix = 0;                           // Pripadne vrat index

            com_rec_buf[com_rw_ix] = com_get_char(port_base);
            com_rw_ix++;                                // Posun index
            com_rbib++;                                 // Zvys pocet znaku v rbuf
        }
        return(1);                                      // Prisel znak? vrat 1
    }
    return(0);
}


//------------------------------------------------------------------------
// Vyzvednuti znaku z RecBuf, odeslani znaku z SndBuf, test prijmu
//------------------------------------------------------------------------
void com_proc(uint port_base)
{
    com_test_rec_char(port_base);                  // Test prijmu

    if(com_rbib > 0)                            // Pokud je v rbuf znak
    {
        com_ribuf=1;                              // Nastav priznak

        if(com_rr_ix >= COM_RECBUFSIZE)
            com_rr_ix=0; // Pripadne vrat index

        com_rbyte = com_rec_buf[com_rr_ix];         // Nacti znak z rbuf
        com_rr_ix++;                              // Posun index
        com_rbib--;                               // Sniz pocet znaku v rbuf
    }
    else
        com_ribuf=0;             // Jinak nuluj priznak znaku v rbuf

    com_test_rec_char(port_base);                  // Test prijmu

    if(com_sbib > 0)                            // Pokud je v sndbuf znak
    {
        com_sdelay--;
        if(!com_sdelay)
        {
            if(com_sr_ix >= COM_SNDBUFSIZE)
                com_sr_ix=0;  // Pripadne vrat index

            com_send_char(port_base, com_snd_buf[com_sr_ix]);
            com_sdelay = COM_SDELAY;
            com_sr_ix++;                               // Posun index
            com_sbib--;                                // Sniz pocet znaku v sndbuf
        }
    }
}



//------------------------------------------------------------------------
// Zjisti zakladni adresu COM portu
//------------------------------------------------------------------------
uint com_get_base(uchar port, uint userportadr)
{
    switch(port)
    {
        case 0: return(peek(0,0x0400));   // default 0x3F8;
        case 1: return(peek(0,0x0402));   // default 0x2F8;
        case 2: return(peek(0,0x0404));   // default 0x3E8;
        case 3: return(peek(0,0x0406));   // default 0x2E8;
        case 4: return(userportadr);
        default: return(0);
    }
}


//------------------------------------------------------------------------
// Nastavi prenosovou rychlost aktualniho portu
//------------------------------------------------------------------------
void com_set_speed(uint port_base, ulong baud)
{
    uchar D0, D1;

    D0 = (uchar) ((115200  / baud) & 0xFF);        // Dolni byte
    D1 = (uchar) (((115200 / baud) >> 8) & 0xFF);  // Horni byte

    outport(port_base + 3, 128);    // Nastavi Zapis do reg. Adr+1
    outport(port_base,     D0);    // Zapis dolniho byte delitele ryhlosti
    outport(port_base + 1, D1);    // Zapis horniho byte delitele ryhlosti
    outport(port_base + 3, 3);
};


//------------------------------------------------------------------------
// Nastaveni parametru prenosu (default: 00000011B - 8 bitu,1 stopbit,parita None)
// ----------------------------
// bit 0..1   Delka slova 00=5, 01=6, 10=7, 11=8 bitu
// bit 2      Stop bity 0=1, 1=2
// bit 3..4   Parita x0=None, 01=Licha, 11=Suda
// bit 5      Parita znaku (BIOS nepouziva)
// bit 6      mo�n� ��zen� BREAK; 1=za��tek vys�l�n� 0s
// bit 7      DLAB (p��stupov� bit k registru d�li�ky)
//            - ur�uje m�d port� 03F8H a 03F9H pro nast.
//            (pokud je DLAB=1, je p��stup k d�li�ce)
//
//------------------------------------------------------------------------
void com_set_properties(uint port_base, uchar portset)
{
    outport(port_base+3, portset);
}

// Nastaveni preruseni portu
// ----------------------------
//03F9H  - z�pis: registr povolen� p�eru�en� (DLAB=0)
//                +---------------+
//                |7 6 5 4 3 2 1 0|
//                +---------------+ bit
//                 | | | | | | | +-> 0: 1=mo�n� p�eru�en� p�i p��jmu dat
//                 | | | | | | +---> 1: 1=mo�n� p�er. p�i pr�zdn�m vys. buff.
//                 | | | | | +-----> 2: 1=mo�n� p�er. stavem p�ij�mac� linky
//                 | | | | +-------> 3: 1=mo�n� p�er. stavem modemu
//                 | | | +---------> 4: v�dy = 0
//                 | | +-----------> 5: v�dy = 0
//                 | +-------------> 6: v�dy = 0
//                 +---------------> 7: v�dy = 0
//       - z�pis: registr   d�li�ky  -  vy���  bajt  (pokud  DLAB=1,  tj.  po
//                instrukci  OUT  03FBH,80H).  Po operaci OUT 03FBH,80H tento
//                port  obsahuje  vy��� bajt d�li�ky taktu. Spole�n� s ni���m
//                bajtem   (port   03F8H)  tvo��  16-bitovou  hodnotu,  kter�
//                nastavuje rychlost p�enosu dat.
void com_set_interrupt(uint port_base, uchar portset)
{
    uchar backup;

    backup = inp(port_base+3);                           // Ulozeni nastaveni portu

    outport(port_base+3, inp(port_base+3) & (0xFF-0x80)); // Nastaveni DLAB=0;
    outport(port_base+1, portset);                       // Nastaveni preruseni portu
    outport(port_base+3, backup);                        // Obnoveni nastaveni portu
}



//03FAH  - �ten�: identifika�n�  registr  p�eru�en�. Pokud nastane p�eru�en�,
//                �t�te  tento  registr,  pokud  chcete zjistit, co zp�sobilo
//                p�eru�en�.
//                +---------------+
//                |7 6 5 4 3 2 1 0|
//                +---------------+  bit
//                 | | | | | | | +->  0:  1=nenastalo ��dn� p�eru�en�
//                 | | | | | +-+--->  1-2: 00=p�eru�en� stavem p�ij�mac� linky
//                 | | | | |                 vznik�  p�i p�ete�en�, parit�,
//                 | | | | |                 chyb� r�mu nebo p�i p�eru�en�.
//                 | | | | |                 Resetov�n� �ten�m stavu linky
//                 | | | | |                 (port 03FDH)
//                 | | | | |              01=p�ijat� data platn� (resetov�n�
//                 | | | | |                 �ten�m p�ij�m. bufferu 03F8H)
//                 | | | | |              10=vys�lac� buffer pr�zdn� (reset
//                 | | | | |                 z�pisem do vys�l. bufferu 03F8H)
//                 | | | | |              11=stav modemu. Vznik� p�i zm�n�
//                 | | | | |                 sign�l� CTS, DSR, RI nebo RLSD.
//                 | | | | |                 Reset �ten�m stavu modemu 03FEH.
//                 | | | | +------->  3:  v�dy = 0
//                 | | | +--------->  4:  v�dy = 0
//                 | | +----------->  5:  v�dy = 0
//                 | +------------->  6:  v�dy = 0
//                 +--------------->  7:  v�dy = 0
//
uchar com_get_interrupt(uint port_base)
{
    return(inport(port_base+2));
}


//------------------------------------------------------------------------
// Aktivace (Status=1) / Deaktivace (Status=0) signalu DTR
//------------------------------------------------------------------------
void com_set_dtr(uint port_base, uchar status)
{
    if(status)
        outport(port_base+4,inp(port_base+4) | 1);
    else
        outport(port_base+4,inp(port_base+4) & (0xFF-1));   // Aktivace DTR
}


//03FDH  - �ten�: stavov� registr linky
//         -----------------
//          7 6 5 4 3 2 1 0
//         ----------------- bit
//          | | | | | | | +-> 0: 1=data p�ipravena (DR) (reset �ten�m dat)
//          | | | | | | +---> 1: 1=chyba p�ete�en� (OE): p�edch. znak ztracen
//          | | | | | +-----> 2: 1=chyba parity (PE): reset �ten�m st.linky
//          | | | | +-------> 3: 1=chyba r�mu (FE): �patn� stop-bit ve znaku
//          | | | +---------> 4: 1=indikace break (BI): p�ijata dlouh� mezera
//          | | +-----------> 5: 1=vys�lac� registr pr�zdn� (OK k vys�l�n�)
//          | +-------------> 6: 1=vys�la� pr�zdn�. Nevys�l� ��dn� data.
//          +---------------> 7: v�dy = 0
//         Pozn.: P�i povolen�m p�eru�en� (03F9H) zp�sob� bity 1-4 p�eru�en�.
//
uchar com_get_error(uint port_base)
{
    return(port_base+5);
}


//------------------------------------------------------------------------
// Vraci 1 pokud je port pripraven vyslat dalsi znak
//------------------------------------------------------------------------
uchar com_send_ready(uint port_base)  // 0 = vysilac je prazdny a pripraven vysilat
{
    if((inp(port_base+5)&(32+64)) == (32+64))
        return(1);
    else
        return(0);
}



//------------------------------------------------------------------------
// Vraci 1 pokud port prijal dalsi znak
//------------------------------------------------------------------------
uchar com_char_ready(uint port_base)     // 1 = Znak byl prijat
{
    if((inp(port_base+5) & 1) == 1)
        return(1);
    else
        return(0);
}


//------------------------------------------------------------------------
// Vraci prijaty znak
//------------------------------------------------------------------------
uchar com_get_char(uint port_base)
{
    return(inp(port_base));
}

//------------------------------------------------------------------------
// Odesle znak
//------------------------------------------------------------------------
uchar com_send_char(uint port_base, uchar byte)
{
    return(outp(port_base, byte));
}

//------------------------------------------------------------------------
//  Nacte status portu
//------------------------------------------------------------------------
uchar com_get_port_status(uint port_base)
{
    return(inp(port_base+5));
}


//------------------------------------------------------------------------
// Pri ztrate predchoziho znaku vraci 2
//------------------------------------------------------------------------
uchar com_overrun_error(uint port_base)
{
    return(com_get_port_status(port_base) & 2);
}


//------------------------------------------------------------------------
// Pri chybne parite vraci 4
//------------------------------------------------------------------------
uchar com_parity_error(uint port_base)
{
    return(com_get_port_status(port_base) & 4);
}


//------------------------------------------------------------------------
// Pri chybnem stopbitu vraci 8
//------------------------------------------------------------------------
uchar com_stopbit_error(uint port_base)
{
    return(com_get_port_status(port_base) & 8);
}


//------------------------------------------------------------------------
// Pokud je prijmana trvala nula, vraci 16
//------------------------------------------------------------------------
uchar com_always_zero(uint port_base)
{
    return(com_get_port_status(port_base) & 16);
}


//3faH  Z�pis:  *** pouze 16550 *** R�d�c� registr fronty FIFO
//      (FIFO = First in, first out, tedy prvn� dovnitr, prvn� ven)
//      +7-6-5-4-3-2-1-0+
//      �   �0 0 0� � � �
//      +-+-------------+ bit
//        �        � � +-? 0: 1=povolen� FIFO prij�mace a vys�lace
//        �        � �        0=v�maz prij�mac� a vys�lac� fronty, znakov� re�im
//        �        � +---? 1: 1=reset prij�mac�ch FIFO registru
//        �        +-----? 2: 1=reset vys�lac�ch FIFO registru
//        +--------------? 7-6: Nastaven� FIFO registru 00 = 1 byte
//                                                      01 = 4 byty
//                                                      10 = 8 bytu
//                                                      11 = 14 bytu
void com_set_fifo(uint port_base, uchar status)
{
    outport(port_base + 2, status);
}
