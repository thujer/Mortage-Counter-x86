
#define kbESC           (keyb_scan == 1)
#define kbENTER         (keyb_scan == 28)
#define kbALT_X         ((keyb_ascii == 0x00) && (keyb_scan == 0x2D))

uchar keyb_ascii;
uchar keyb_scan;

//------------------------------------------------------------------------
// Zjisti stisknutou klavesu, pokud neni klavesa stisknuta, ceka.
//------------------------------------------------------------------------
void keyb_get_key()
{
    uchar S,A;

    asm{
        MOV AH,0x10
        INT 0x16
        MOV A,AL
        MOV S,AH
    }
    keyb_ascii = A;
    keyb_scan = S;
}

