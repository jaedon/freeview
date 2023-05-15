/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
      File multiply_low.c.    Version 10 December 2000
*/
#define DRM_BUILDING_MULTLOW_C
#include "bignum.h"
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#if defined (DRM_MSC_VER) && DRM_SUPPORT_ASSEMBLY
    #pragma warning(disable : 4100)      /* Unreferenced formal parameter */
#endif

/****************************************************************************/
#if defined(_M_IX86) && DRM_SUPPORT_ASSEMBLY
__declspec(naked) DRM_VOID __stdcall multiply_low(const digit_t  a[],
                                    const digit_t  b[],
                                    digit_t   c[],
                                    const DRM_DWORD lng )
#else
DRM_API DRM_NO_INLINE  DRM_VOID DRM_CALL multiply_low(
                                    const digit_t  a[],
                                    const digit_t  b[],
                                    digit_t   c[],
                                    const DRM_DWORD lng )
#endif
/*
        This routine multiplies two integers a and b of length lng,
        returning the low half of the product in c.  That is,
        c == a*b (mod RADIX^lng).  The top of c is unchanged.

        If lng = 2n, then this code uses n^2 + 2n multiplications,
        whereas a full Karatsuba multiplication uses about n^1.58.
        This code uses fewer multiplications for lng <= 20.
        This routine has less overhead than the standard Karatsuba
        multiplication (which gets the full product)

                2n      n^2 + 2n         Karatsuba

                 2          3                3
                 4          8                9
                 6         15               18
                 8         24               27
                10         35
                12         48               54
                14         63
                16         80               81
                24        168              162
                32        288              243

        We rewrite (assuming lng = 5)

                (a0 + a1*R + a2*R^2 + a3*R^3 + a4*R^4)
              * (b0 + b1*R + b2*R^2 + b3*R^3 + b4*R^4)
        as
                  (a0*b0 + a1*b1*R + a2*b2*R^2 + a3*b3*R^3 + a4*b4*R^4)
                * (1 + R + R^2 + R^3 + R^4)
              - R*(a1 - a0)*(b1 - b0)
              - R^2*(a2 - a0)*(b2 - b0)
              - R^3*[(a3 - a0)*(b3 - b0) + (a2 - a1)*(b2 - b1)]
              - R^4*[(a4 - a0)*(b4 - b0) + (a3 - a1)*(b3 - b1)]
              - (multiple of R^5)

        The products (ai - aj)*(bi - bj) can be positive or negative.
        We accumulate positive and negative terms in separate arrays.
        Specifically, the even subscripts in the ctemp array
        (i.e., ctemp[0], ctemp[2], ctemp[4], ...)
        contain the positive terms, while
        its odd subscripts (ctemp[1], ctemp[3], ctemp[5])
        contain the negated terms.  At the end,
        we subtract the negated terms from the positive terms
        and store the difference in the c array.

        To obtain

                  (a0*b0 + a1*b1*R + a2*b2*R^2 + a3*b3*R^3 + a4*b4*R^4)
                * (1 + R + R^2 + R^3 + R^4)

        modulo R^5, we note the second term is 1/(1-R) (mod R^5).
        Define absum[k] by

                  a0*b0 + ... + a[k]*b[k]*R^k
                = (c0 + c1*R + ... + c[k]*R^k)*(1 - R)
                  + absum[k]*R^(k+1).

        The recurrence for absum[k] is

                  absum[-1] = c[-1] = 0

                  absum[k]*R^(k+1) + c[k]*R^k*(1 - R)
                = absum[k-1]*R^k + a[k]*b[k]*R^k
        or
                  R*(absum[k] - c[k]) + c[k] = a[k]*b[k] + absum[k-1]

        Given single-words  absum[k-1] - c[k-1]  and  c[k-1],
        we can form the double-word

                a[k]*b[k]  +  (absum[k-1] - c[k-1])  +  c[k-1]

        From this, we extract  absum[k] - c[k]  and c[k].
*/
{
#if defined(_M_IX86) && DRM_SUPPORT_ASSEMBLY

    typedef struct {
             digit_t   *addrc;                   /* &c[0] */
             DRM_DWORD     lng;                      /* Saved value of lng */
             __int32   saved_regs[4];            /* ebp/ebx,edi/esi */
             digit_t   ctemp[2*MP_LONGEST+2];
             digit_t   acopy[MP_LONGEST | 1];    /* Copy of a array */
             digit_t   bcopy[MP_LONGEST | 1];    /* Copy of b array */
                   } mullow_locals_t;

                       /* Note -- copying the arrays to acopy
                          and bcopy simplifies indexing in inner
                          loops, since there are insufficient
                          registers to save &a[0] and &b[0].
                          The acopy array has an odd length,
                          to ensure bcopy - acopy is not
                          a multiple of 32 bytes.
                       */
  _asm {

                /*
                     Load formal parameters.
                     Modify register ebp last.
                */

    mov   ecx,[esp+4+8]             ; c
    sub   esp,TYPE mullow_locals_t  ; Allocate space for local struct
                                    ; AGI delay (esp)
    mov   [esp].saved_regs,ebp
    mov   [esp+4].saved_regs,ebx
    mov   [esp+8].saved_regs,edi
    mov   [esp+12].saved_regs,esi

    mov   ebp,[esp+TYPE mullow_locals_t+4+12] ; lng
    mov   esi,[esp+TYPE mullow_locals_t+4+0]  ; a
    mov   edi,[esp+TYPE mullow_locals_t+4+4]  ; b
    cmp   ebp,0
    je    mullow_exit               ; Exit if lng = 0

    mov   eax,[esi]                 ; eax = a[0]
    mov   ebx,[edi]                 ; ebx = b[0]
    mov   [esp].acopy,eax           ; Store acopy[0]
    mul   ebx                       ; edx:eax = dsum = a[0]*b[0]

    cmp   ebp,2                     ; Compare lng to 2

    jle    mullow_lng_le_2          ; Branch if lng <= 2
/*
                                Case lng > 2
*/
    mov  [esp].lng,ebp             ; Save ebp = lng
    mov  [esp].addrc,ecx           ; Save ecx = &c[0]
    mov  ebp,1                     ; ebp = i = 1
    mov  [esp].bcopy,ebx           ; Store bcopy[0] = ebx = b[0]
    mov  ebx,eax
    mov  ecx,edx                   ; ecx:ebx = dsum

mullow_loop1:
    mov  [esp+8*ebp-8].ctemp,ebx   ; Store ctemp[2*i - 2] = ebx = LOW(dsum)
    add  ebx,ecx                   ;
    mov  eax,[esi+4*ebp]           ; eax = a[i]
    mov  ecx,0

    mov  edx,[edi+4*ebp]           ; edx = b[i]
                                   ; Possible data cache conflict
    mov  [esp+8*ebp-4].ctemp,ecx   ; Store ctemp[2*i - 1] = 0

    adc  ecx,ecx                   ; ecx:ebx = HIGH(dsum) + LOW(dsum)
    mov  [esp+4*ebp].acopy,eax     ; Store acopy[i] = eax = a[i]
    mov  [esp+4*ebp].bcopy,edx     ; Store bcopy[i] = ebx = b[i]
    inc  ebp                       ; Advance i
    mul  edx                       ; edx:eax = a[i]*b[i]
    add  ebx,eax
    mov  eax,[esp].lng             ; eax = lng = loop bound
    adc  ecx,edx                   ; ecx:ebx = dsum
                                   ;         =   a[i]*b[i]
                                   ;           + HIGH(old dsum)
                                   ;           + LOW(old dsum)
    cmp  ebp,eax                   ; Compare (new i) to lng
    jne  mullow_loop1              ; Continue until i = lng
                                /*
                                        At this point

                                        eax = ebp = lng
                                        ecx:ebx = final dsum
                                */
    xor  esi,esi                   ; esi = 2*i = 0
    mov  [esp+8*ebp-8].ctemp,ebx   ; Store ctemp[2*lng-2] = LOW(dsum)

    mov  ebx,[esp].acopy           ; ebx = acopy[0]
    mov  ecx,[esp].bcopy           ; ecx = bcopy[0]
    lea  edi,[2*ebp-2]             ; edi = 2*((first j) + 1) = 2*(lng - 1)

    mov  [esp+8*ebp-4].ctemp,esi   ; Store ctemp[2*lng-1] = 0
    mov  [esp+8*ebp].ctemp,esi     ;       ctemp[2*lng  ] = 0
    mov  [esp+8*ebp+4].ctemp,esi   ;       ctemp[2*lng+1] = 0

mullow_loopi:
                                /*
                                        Before branching here, set

                                            ebp = lng
                                            ebx = acopy[i]
                                            ecx = bcopy[i]
                                            esi = 2*i
                                            edi = 2*(j+1) = 2*(lng - i - 1)
                                */
    mov  eax,[esp+2*edi].acopy         ; eax = acopy[j+1]
    mov  edx,[esp+2*edi].bcopy         ; edx = bcopy[j+1]
    sub  eax,ebx                       ; eax = adif = acopy[j+1] - acopy[i]
    sub  edx,ecx                       ; edx = bdif = bcopy[j+1] - bcopy[i]

    imul eax,edx                       ; eax = LOW(adif*bdif)

    mov  edx,[esp+8*ebp-4].ctemp       ; edx = ctemp[2*lng-1]
    add  esi,2                         ; Advance 2*i by 2

    add  edx,eax                       ; edx = LOW(adif*bdif + ctemp[2*lng-1])
                                       ; TBD -- poor Pentium P5 pairing here
    mov  [esp+8*ebp-4].ctemp,edx       ; Store new ctemp[2*lng-1]

mullow_loopj:
/*
                        Before branching here, set

                                          ebx = acopy[i]
                                          ecx = bcopy[i]
                                          edi = 2*j + 2
                                          esi = 2*i + 2
*/
    mov  eax,[esp+2*edi-4].acopy        ; eax = acopy[j]
    mov  edx,[esp+2*edi-4].bcopy        ; edx = bcopy[j]
    lea  ebp,[edi+esi-3]                ; ebp = 2*i + 2*j + 1
    sub  eax,ebx                        ; eax = adif = acopy[j] - acopy[i]
    sbb  ebx,ebx                        ; ebx = asgn
    sub  edx,ecx                        ; edx = bdif = bcopy[j] - bcopy[i]
    sbb  ecx,ecx                        ; ecx = bsgn

    add  eax,ebx                        ; eax = adif + asgn
    xor  eax,ebx                        ; eax = | adif |
    xor  ebx,ecx                        ; ebx = absgn = asgn^bsgn

    add  edx,ecx                        ; edx = bdif + bsgn
    add  ebp,ebx                        ; ebp = csub = absgn + 2*i + 2*j + 1
    xor  edx,ecx                        ; edx = | bdif |

    mul  edx                            ; edx:eax = |adif|*|bdif|

    mov  ecx,[esp+4*ebp].ctemp          ; ecx = ctemp[csub]
    mov  ebx,[esp+4*ebp+8].ctemp        ; ebx = ctemp[csub+2]
    add  ecx,eax                        ; ctemp[csub] += eax

    lea  edi,[edi-2]                    ; Decrement 2*j + 2 by 2
    mov  [esp+4*ebp].ctemp,ecx          ; Update ctemp[csub]

mullow_carryprop:
    lea  ebp,[ebp+2]                    ; Advance csub by 2
                                        ; (Note -- this pairs with mov
                                        ;  on first iteration of loop.
                                        ;  Loop executes about 10/9 times)

    adc  ebx,edx                        ; ctemp[(old csub) + 2] += edx + CF
    mov  edx,0                          ; Clear edx for future iterations
    mov  [esp+4*ebp].ctemp,ebx          ; Update ctemp[(old csub) + 2]
    mov  ebx,[esp+4*ebp+8].ctemp        ; ebx = ctemp[(new csub) + 2]
    jc   mullow_carryprop               ; If CF <> 0, continue

    cmp  esi,edi                        ; Compare 2*i + 2 to 2*(new j) + 2
    mov  ebx,[esp+2*esi-4].acopy        ; ebx = acopy[i]
    mov  ecx,[esp+2*esi-4].bcopy        ; ecx = bcopy[i]
    jne  mullow_loopj
/*
                                At this point

                                        esi = edi = 2*i + 2 = 2*(new i)
*/
    mov  ebp,[esp].lng               ; ebp = lng
    xor  edi,-2                      ; edi = -2 - (2*i + 2) = -4 - 2*i

                                     ; (both operands are even)

    mov  ebx,[esp+2*esi].acopy       ; ebx = acopy[new i]
    mov  ecx,[esp+2*esi].bcopy       ; ecx = bcopy[new i]

    lea  edx,[esi+2]                 ; edx = 2*(new i) + 2
    lea  edi,[edi+2*ebp]             ; edi = 2*lng - 2*i - 4
                                     ;     = 2*(lng - 1 - (new i))
    cmp  edx,edi                     ; Compare 2*(new i) + 2
                                     ;      to 2*(lng - 1 - (new i))
    jl   mullow_loopi              ; Continue if (new i) + 1 < lng - 1 - (new i)

    shr  ebp,1                       ; ebp = FLOOR(lng/2); set CF
    lea  esi,[esp].ctemp             ; esi = &ctemp[0]
    mov  edi,[esp].addrc             ; edi = &c[0]
    jnc  mullow_final_even           ; Branch if lng is even



    mov  edx,[esi]                  ; edx = ctemp[0]
    mov  ecx,[esi+4]                ; ecx = ctemp[1]
    sub  edx,ecx                    ; edx = ctemp[0] - ctemp[1]

mullow_final_odd_sub:
    mov  ebx,[esi+8]                ; ebx = ctemp[2*i]
    mov  eax,[esi+12]               ; eax = ctemp[2*i+1]
    mov  [edi],edx                  ; Store c[i-1]
    lea  edi,[edi+8]                ; Advance &c[i-1] by 2 dwords
    mov  edx,[esi+16]               ; edx = ctemp[2*i+2]
    mov  ecx,[esi+20]               ; ecx = ctemp[2*i+3]
    sbb  ebx,eax                    ; ebx = ctemp[2*i] - ctemp[2*i+1] - CF
    lea  esi,[esi+16]               ; Advance &ctemp[2*i-2] by 4 dwords
    sbb  edx,ecx                    ; edx = ctemp[2*i+2] - ctemp[2*i+3] - CF
    dec  ebp                        ; Decrement loop counter
    mov  [edi-4],ebx                ; Store c[i]
    jnz  mullow_final_odd_sub

    mov  [edi],edx                  ; Store c[lng-1]
    jz   mullow_exit                ; Exit


mullow_lng_le_2:                    ; Come here is lng <= 2
    mov   [ecx],eax                 ; Store c[0] = LOW(dsum)
    jne   mullow_exit               ; Exit if lng = 1

                                /*
                                        Case lng = 2
                                */

    mov   eax,[esi+4]               ; eax = a[1]
    mov   esi,[esi]                 ; esi = a[0]

    imul  eax,ebx                   ; eax = LOW(a[1]*b[0])
    mov   ebx,[edi+4]               ; ebx = b[1]
    add   edx,eax                   ; edx = HIGH(dsum) + a[1]*b[0] mod RADIX

    imul  esi,ebx                   ; esi = LOW(a[0]*b[1])
    add   edx,esi
    mov   [ecx+4],edx               ; Store c[1]
    jmp   mullow_exit



mullow_final_even:
                                /*
                                      At this point:

                                          ebp = lng/2
                                          edi = @c[0]
                                          esi = @ctemp[0]
                                          ebx = acopy[lng/2 - 1]
                                          ecx = bcopy[lng/2 - 1]
                                */
    mov  eax,[esp+4*ebp].acopy       ; eax = acopy[lng/2]
    mov  edx,[esp+4*ebp].bcopy       ; edx = bcopy[lng/2]
    sub  eax,ebx                     ; eax = adif
    sub  edx,ecx                     ; edx = bdif
    mov  ebx,[esi]                   ; ebx = ctemp[0]
    mov  ecx,[esi+4]                 ; ecx = ctemp[1]

    imul eax,edx                     ; eax = LOW(adif*bdif)
    mov  edx,[esi+8]                 ; edx = ctemp[2]
    sub  ebx,ecx                     ; ebx = ctemp[0] - ctemp[1]
    mov  ecx,[esi+12]                ; ecx = ctemp[3]
    dec  ebp                         ; ebp = lng/2 - 1 = loop count
    sbb  edx,ecx                     ; edx = ctemp[3] - ctemp[2] - CF
    mov  [edi],ebx                   ; Store c[0] = ebx

mullow_final_even_sub:
    mov  ebx,[esi+16]                ; ebx = ctemp[2*i + 2]
    mov  ecx,[esi+20]                ; ecx = ctemp[2*i + 3]
    mov  [edi+4],edx                 ; Store c[i-1] = edx
    lea  edi,[edi+8]                 ; Advance &c[i-2] by 2 dwords
    sbb  ebx,ecx                     ; ebx = ctemp[2*i + 2] - ctemp[2*i + 3] - CF
    mov  edx,[esi+24]                ; ebx = ctemp[2*i + 4]
    mov  ecx,[esi+28]                ; ecx = ctemp[2*i + 5]
    lea  esi,[esi+16]                ; Advance &ctemp[2*i - 2] by 4 dwords
    sbb  edx,ecx                     ; edx = ctemp[2*i + 4] - ctemp[2*i + 5] - CF
    dec  ebp                         ; Decrement loop counter
    mov  [edi],ebx                   ; Store  c[i] = ebx
    jnz  mullow_final_even_sub

    sub  edx,eax                     ; Subtract LOW(adif*bdif)
    mov  [edi+4],edx                 ; Store c[lng-1]


mullow_exit:
    mov  ebp,[esp].saved_regs
    mov  ebx,[esp+4].saved_regs
    mov  edi,[esp+8].saved_regs
    mov  esi,[esp+12].saved_regs
    add  esp,TYPE mullow_locals_t    ; Restore esp
    ret 16                           ; pop off all 4 arguments from the stack
    } /* asm IX86 */
#else

    DRM_UINT64 dsum;

    if (lng == 0) return;

    dsum = DPRODUU(a[0], b[0]);
    if ((lng > 2) && (lng <= MP_LONGEST)) {
        digit_t borrow;
        DRM_DWORD i, j;
        digit_t ctemp[2*MP_LONGEST + 2];

        for (i = 1; i != lng; i++) {
            ctemp[2*i-2] = DRM_UI64Low32(dsum);
            ctemp[2*i-1] = 0;
            dsum = MULTIPLY_ADD2(a[i], b[i], DRM_UI64High32(dsum), DRM_UI64Low32(dsum));
        }
        ctemp[2*lng-2] = DRM_UI64Low32(dsum);
        ctemp[2*lng-1] = 0;
        ctemp[2*lng  ] = 0;
        ctemp[2*lng+1] = 0;  /* So inner loop will terminate */
                             /* (i.e., carries will not propagate beyond here) */

        for (i = 0; 2*i < lng-2; i++) {
            const digit_t ai = a[i];
            const digit_t bi = b[i];

            ctemp[2*lng-1] += (a[lng-1-i] - ai)*(b[lng-1-i] - bi);

            for (j = lng-2-i; j != i; j--) {
                digit_t adif = a[j] - ai;
                digit_t bdif = b[j] - bi;

                const digit_t asgn = (const digit_t)( -(a[j] < ai) );
                const digit_t bsgn = (const digit_t)( -(b[j] < bi) );
                digit_t carry;
                DRM_DWORD csub;

                adif = (adif + asgn)^asgn;
                bdif = (bdif + bsgn)^bsgn;
                csub = 2*(i + j) + (DRM_DWORD)((asgn^bsgn) + 1);
                         /* even subscript if adding, odd if subtracting */
                dsum = MULTIPLY_ADD1(adif, bdif, ctemp[csub]);
                ctemp[csub] = DRM_UI64Low32(dsum);
                dsum = DRM_UI64Add32(ctemp[csub+2], DRM_UI64High32(dsum) );
                ctemp[csub+2] = DRM_UI64Low32(dsum);
                carry = DRM_UI64High32(dsum);
                do {
                    ctemp[csub+4] += carry;
                    carry = (digit_t)(ctemp[csub+4] < carry);
                    csub += 2;
                } while ((carry != 0) && (csub < (2*MP_LONGEST - 2)));
            } /* for j */

        } /* for i */

        if (IS_EVEN(lng)) {
            const DRM_DWORD lnghalf = lng >> 1;
            ctemp[2*lng-1] +=   (a[lnghalf] - a[lnghalf-1])
                              * (b[lnghalf] - b[lnghalf-1]);
        }

        borrow = 0;
        for (i = 0; i < lng; i++) {
            const digit_t diff = ctemp[2*i] - ctemp[2*i+1];
            const digit_t new_borrow = (const digit_t)(ctemp[2*i] < ctemp[2*i+1]);

            c[i] = diff - borrow;
            if (diff != 0) borrow = new_borrow;
        }
    } else if (lng == 2) {
        c[0] = DRM_UI64Low32(dsum);
        c[1] = DRM_UI64High32(dsum) + a[0]*b[1] + a[1]*b[0];  /* mod RADIX */
    } else if (lng == 1) {
        c[0] = DRM_UI64Low32(dsum);
    } else {
        TRACE( ( "Unexpected length %ld in multiply_low.\n", ( DRM_LONG )lng ) );
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA, "multiply_low");
    }
#endif  /* assembly */
} /* multiply low */

EXIT_PK_NAMESPACE_CODE;
