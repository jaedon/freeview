/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMINT64_C
#include <drmerr.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#if !DRM_SUPPORT_NATIVE_64BIT_TYPES

static const  DRM_INT64   g_one  = {1, 0};
static const  DRM_UINT64  g_uone = {1, 0};


static DRM_UINT64 _RemoveSign(const DRM_INT64 a, long * sa)
{
    DRM_UINT64 aa;
    unsigned long int *pa, *pb;

    pa  = (unsigned long int *)&(a.val);
    pb  = (unsigned long int *)&(aa.val);
    *pb = *pa;
    *(pb+1) = *(pa+1);
    *sa  = ((*(pa+1)) & 0x80000000);
    if (*sa) {
        *pb     = ~(*pb);
        *(pb+1) = ~(*(pb+1));
        aa = DRM_UI64Add(aa, g_uone);
    }
    return aa;
}

static void _SetBit(unsigned long int * pData, int loc)
{
    long bit = 1;
    bit = bit << loc;
    *pData |= bit;
}

DRM_API DRM_INT64 DRM_CALL DRM_I64Add(const DRM_INT64 a, const DRM_INT64 b)
{
    /* Same as DRM_UI64Add */
    DRM_INT64 c;
    unsigned short int *upa, *upb, *upc;
    unsigned long int temp, carry, i;
    upa = (unsigned short int *)&(a.val);
    upb = (unsigned short int *)&(b.val);
    upc = (unsigned short int *)&(c.val);
    carry = 0;

#if TARGET_LITTLE_ENDIAN
    for (i=0;i<4;i++)
    {
        temp  = (carry + *upa) + *upb;
        carry = temp >> 16;
        *upc  = (unsigned short)temp;
        upa++; upb++; upc++;
    }
#else
    temp  = carry + *(upa+1) + *(upb+1);
    carry = temp >> 16;
    *(upc+1)  = temp;

    temp  = carry + *upa + *upb;
    carry = temp >> 16;
    *upc  = temp;

    temp  = carry + *(upa+3) + *(upb+3);
    carry = temp >> 16;
    *(upc+3)  = temp;

    temp  = carry + *(upa+2) + *(upb+2);
    carry = temp >> 16;
    *(upc+2)  = temp;
#endif

    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_I64Sub(const DRM_INT64 a, const DRM_INT64 b)
{
    DRM_INT64 nb;
    unsigned long int *pa, *pb;
    pa  = (unsigned long int *)&(nb.val);
    pb  = (unsigned long int *)&(b.val);
    *pa = ~(*pb);
    pa++; pb++;
    *pa = ~(*pb);
    return DRM_I64Add(a, DRM_I64Add(nb, g_one)) ;

}

DRM_API DRM_INT64 DRM_CALL DRM_I64Mul(const DRM_INT64 a, const DRM_INT64 b)
{
    DRM_INT64  c;
    DRM_UINT64 aa, bb, cc;
    unsigned long int *pa, *pb;
    long sa, sb;

    aa = _RemoveSign(a,&sa) ;
    bb = _RemoveSign(b,&sb) ;

    cc = DRM_UI64Mul(aa, bb);
    if (sa ^ sb) {
        pa  = (unsigned long int *)&(cc.val);
        *pa     = ~(*pa);
        *(pa+1) = ~(*(pa+1));
        cc = DRM_UI64Add(cc, g_uone);
    }

    pa  = (unsigned long int *)&(cc.val);
    pb  = (unsigned long int *)&(c.val);
    *pb = *pa;
    *(pb+1) = *(pa+1);

    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_I64Div(const DRM_INT64 a, const DRM_INT64 b)
{
    DRM_INT64 c;
    DRM_UINT64 aa, bb, cc;
    unsigned long int *pa, *pb;
    long sa, sb;

    aa = _RemoveSign(a,&sa) ;
    bb = _RemoveSign(b,&sb) ;

    cc = DRM_UI64Div(aa, bb);
    if (sa ^ sb) {
        pa  = (unsigned long int *)&(cc.val);
        *pa     = ~(*pa);
        *(pa+1) = ~(*(pa+1));
        cc = DRM_UI64Add(cc, g_uone);
    }

    pa  = (unsigned long int *)&(cc.val);
    pb  = (unsigned long int *)&(c.val);
    *pb = *pa;
    *(pb+1) = *(pa+1);

    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_I64Mod(const DRM_INT64 a, const DRM_INT64 b)
{
    DRM_INT64 c;
    DRM_UINT64 aa, bb, cc;
    unsigned long int *pa, *pb;
    long sa, sb;

    aa = _RemoveSign(a,&sa) ;
    bb = _RemoveSign(b,&sb) ;

    cc = DRM_UI64Mod(aa, bb);

    if (sa) {
        pa  = (unsigned long int *)&(cc.val);
        *pa     = ~(*pa);
        *(pa+1) = ~(*(pa+1));
        cc = DRM_UI64Add(cc, g_uone);
    }

    pa  = (unsigned long int *)&(cc.val);
    pb  = (unsigned long int *)&(c.val);
    *pb = *pa;
    *(pb+1) = *(pa+1);

    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_I64And(const DRM_INT64 a, const DRM_INT64 b)
{
    DRM_INT64 c;
    long int *pa, *pb, *pc;
    pa = (long int *)&(a.val);
    pb = (long int *)&(b.val);
    pc = (long int *)&(c.val);

    *pc = *pa & *pb;
    pc++; pa++; pb++;
    *pc = *pa & *pb;
    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_I64ShR(const DRM_INT64 a, const unsigned int b)
{
    DRM_INT64 c;
    unsigned long int *upa, *upc, td;
    long int *pa, *pc;
    upa = (unsigned long int *)&(a.val);
    upc = (unsigned long int *)&(c.val);
    pa  = (long int *)&(a.val);
    pc  = (long int *)&(c.val);

    if (b > 63) {
        if ( *(pa+1) < 0 ) {
            *upc++ = 0xFFFFFFFF;
            *upc   = 0xFFFFFFFF;
        } else {
            *upc++ = 0;
            *upc = 0;
        }
    }
    else if (b > 31){
        *pc = *(pa+1) >> (b - 32);
        pc++;
        *pc = *(pa+1)<0 ? 0xFFFFFFFF : 0;
    }
    else if (b > 0) {
        *upc = *upa >> b;
        td  = *(upa+1) << (32 - b);
        *upc |= td;
        pc++; pa++;
        *pc = *pa >> b;
    }
    else {
        *upc = *upa ;
        upc++; upa++;
        *upc = *upa ;
    }
    return c;



}


DRM_API DRM_INT64 DRM_CALL DRM_I64ShL(const DRM_INT64 a, const unsigned int b)
{
    DRM_INT64 c;
    unsigned long int *pa, *pc, td;
    pa = (unsigned long int *)&(a.val);
    pc = (unsigned long int *)&(c.val);

    if (b > 63) {
        *pc = 0;
        pc++;
        *pc = 0;
    }
    else if (b > 31){
        *pc = 0;
        pc++;
        *pc = *pa << (b - 32);
    }
    else if (b >0) {
        *pc = *pa << b;
        td  = *pa >> (32 - b);
        pc++; pa++;
        *pc = *pa << b;
        *pc |= td;
    }
    else {
        *pc = *pa ;
        pc++; pa++;
        *pc = *pa ;
    }
    return c;
}


DRM_API int DRM_CALL DRM_I64Eql(const DRM_INT64 a, const DRM_INT64 b)
{
    long int *pa, *pb;
    pa = (long int *)&(a.val);
    pb = (long int *)&(b.val);

    return ((*pa == *pb)&&(*(pa+1) == *(pb+1)));

}


DRM_API int DRM_CALL DRM_I64Les(const DRM_INT64 a, const DRM_INT64 b)
{
    long int *pa = (long int *) &(a.val);
    long int *pb = (long int *) &(b.val);

    if (*(pa+1) == *(pb+1))
    {
        /* high 32-bits are equal,
        ** so just compare low 32-bits
        */
        return (*(unsigned long int *)pa < *(unsigned long int *)pb);
    }
    else
    {
        if ((*(pa+1)) & 0x80000000)
        {
            if((*(pb+1)) & 0x80000000)
            {
                /* a and b are both negative */
                return (*(unsigned long int *)(pa+1) < *(unsigned long int *)(pb+1));
            }
            else
            {
                /* a is negative, b is positive,
                ** so a is less than b
                */
                return 1;
            }
        }
        else
        {
            if((*(pb+1)) & 0x80000000)
            {
                /* a is positive, b is negative,
                ** so a is not less than b
                */
                return 0;
            }
            else
            {
                /* a and b are both positive and not equal,
                ** so just compare the high bytes
                */
                return (*(pa+1) < *(pb+1));
            }
        }
    }
}


DRM_API DRM_INT64 DRM_CALL DRM_I64(const long int b)
{
    DRM_INT64 c;
    long int *pa;
    pa = (long int *)&(c.val);
    *pa = b;
    pa++;
    if( b & 0x80000000 )
    {
        /* Make sure we sign extend correctly */
        *pa = 0xFFFFFFFF;
    }
    else
    {
        *pa = 0;
    }

    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_I64Asgn(const long int a, const long int b)
{
    DRM_INT64 c;
    long int *pa;
    pa = (long int *)&(c.val);
    *pa = b;
    pa++;
    *pa = a;

    return c;
}


DRM_API DRM_INT64 DRM_CALL DRM_UI2I64(const DRM_UINT64 b)
{
    DRM_INT64 c;
    long int *pa, *pb;
    pa = (long int *)&(c.val);
    pb = (long int *)&(b.val);
    *pa = *pb;
    pa++; pb++;
    *pa = *pb;

    return c;
}


DRM_API unsigned long int DRM_CALL DRM_I64ToUI32(const DRM_INT64 b)
{
    unsigned long int *pa;
    pa = (unsigned long int *)&(b.val);

    return *pa;
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64Add(const DRM_UINT64 a, const DRM_UINT64 b)
{
    DRM_UINT64 c;
    unsigned short int *upa, *upb, *upc;
    unsigned long int temp, carry, i;
    upa = (unsigned short int *)&(a.val);
    upb = (unsigned short int *)&(b.val);
    upc = (unsigned short int *)&(c.val);
    carry = 0;

#if TARGET_LITTLE_ENDIAN
    for (i=0;i<4;i++) {
        temp  = carry + *upa + *upb;
        carry = temp >> 16;
        *upc  = (unsigned short)temp;
        upa++; upb++; upc++;
    }
#else
    temp  = carry + *(upa+1) + *(upb+1);
    carry = temp >> 16;
    *(upc+1)  = temp;

    temp  = carry + *upa + *upb;
    carry = temp >> 16;
    *upc  = temp;

    temp  = carry + *(upa+3) + *(upb+3);
    carry = temp >> 16;
    *(upc+3)  = temp;

    temp  = carry + *(upa+2) + *(upb+2);
    carry = temp >> 16;
    *(upc+2)  = temp;
#endif

    return c;
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64Sub(const DRM_UINT64 a, const DRM_UINT64 b)
{
    DRM_UINT64 nb;
    unsigned long int *pa, *pb;
    pa  = (unsigned long int *)&(nb.val);
    pb  = (unsigned long int *)&(b.val);
    *pa = ~(*pb);
    pa++; pb++;
    *pa = ~(*pb);
    return DRM_UI64Add(a, DRM_UI64Add(nb, g_uone)) ;
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64Mul(const DRM_UINT64 a, const DRM_UINT64 b)
{
    DRM_UINT64  c       = DRM_UI64LITERAL( 0, 0 );
    DRM_BOOL    fZero   = TRUE;
    DRM_LONG    i       = 0;
    DRM_LONG    j       = 0;
    DRM_LONG    carry   = 0;
    DRM_LONG    temp    = 0;
    DRM_BYTE    aa[8], bb[8], cc[8];


    DWORD_TO_LITTLEENDIAN_BYTES( aa, 0, a.val[0] );
    DWORD_TO_LITTLEENDIAN_BYTES( aa, 4, a.val[1] );
    DWORD_TO_LITTLEENDIAN_BYTES( bb, 0, b.val[0] );
    DWORD_TO_LITTLEENDIAN_BYTES( bb, 4, b.val[1] );


    for (i = 0; i < NO_OF(a.val); i++)
    {
        if (a.val [i] != 0
        ||  b.val [i] != 0)
        {
            fZero = FALSE;
            break;
        }
    }

    if (fZero)
    {
        for (i = 0; i < NO_OF(c.val); i++)
        {
            c.val [i] = 0;
        }

        return c;
    }

    carry = 0;

    for (i = 0; i < 8; i++)
    {
        temp = carry;
        for (j=0;j<=i;j++)
        {
            temp += aa[j] * bb[(i-j)];
        }

        carry = temp >> 8;
        cc[i] = (DRM_BYTE)(temp & 0xFF);
    }

    LITTLEENDIAN_BYTES_TO_DWORD( c.val[0], cc, 0 );
    LITTLEENDIAN_BYTES_TO_DWORD( c.val[1], cc, 4 );

    return c;
}

#define TEST_TOP_BIT(x) (x.val[1]&0x80000000)
#define TEST_BIT(x,b) ( x.val[(b)>>5]&(1<<((b)&0x1f)) )


DRM_API DRM_UINT64 DRM_CALL DRM_UI64Div(const DRM_UINT64 a, const DRM_UINT64 b)
{
    DRM_UINT64 c, aa, bb;
    unsigned long int *pc, *pb, td;

    pb = (unsigned long int *)&(b.val);
    pc = (unsigned long int *)&(c.val);

    aa = a;
    bb = b;
    *pc = 0; *(pc+1) = 0;
    if (DRM_UI64Les(a, b) || ((*pb==0) && (*(pb+1)==0)) )
        return c;

    while(!DRM_UI64Les(aa,bb)) {
        td = 1;
        if ( !TEST_TOP_BIT(b) ) {
            while(1) {
                if ( TEST_BIT(b,64-td) )
                    break;  /* b > aa if a has top bit on and b will shift a bit off msb */
                if ( DRM_UI64Les(aa,DRM_UI64ShL(b,td)) )
                    break;
                td++;
            }
        }
        td--;

        if (td > 31)
            _SetBit(pc+1, td-32);
        else
            _SetBit(pc, td);

        aa = DRM_UI64Sub(aa,DRM_UI64ShL(b,td));
    }
    return c;
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64Mod(const DRM_UINT64 a, const DRM_UINT64 b)
{
    DRM_UINT64 aa, bb;
    unsigned long int td, *pb;

    pb = (unsigned long int *)&(b.val);

    aa = a;
    bb = b;
    if (DRM_UI64Les(aa, bb)) {
        return aa;
    }
    if ( (*pb==0) && (*(pb+1)==0) )
        return b;

    while(!DRM_UI64Les(aa,bb)) {
        td = 1;
        if ( !TEST_TOP_BIT(b) ) {
            while(1) {
                if ( TEST_BIT(b,64-td) )
                    break;  /* b > aa if a has top bit on and b will shift a bit off msb */
                if ( DRM_UI64Les(aa,DRM_UI64ShL(b,td)) )
                    break;
                td++;
            }
        }
        td--;

        aa = DRM_UI64Sub(aa,DRM_UI64ShL(b,td));
    }
    return aa;
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64And(const DRM_UINT64 a, const DRM_UINT64 b)
{
    DRM_UINT64 c;
    long int *pa, *pb, *pc;
    pa = (long int *)&(a.val);
    pb = (long int *)&(b.val);
    pc = (long int *)&(c.val);

    *pc = *pa & *pb;
    pc++; pa++; pb++;
    *pc = *pa & *pb;
    return c;
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64ShR(const DRM_UINT64 a,const unsigned int b)
{
    DRM_UINT64 c;
    unsigned long int *pa, *pc, td;
    pa = (unsigned long int *)&(a.val);
    pc = (unsigned long int *)&(c.val);

    if (b > 63) {
        *pc = 0;
        pc++;
        *pc = 0;
    }
    else if (b > 31){
        *pc = *(pa+1) >> (b - 32);
        pc++;
        *pc = 0;
    }
    else if (b > 0) {
        *pc = *pa >> b;
        td  = *(pa+1) << (32 - b);
        *pc |= td;
        pc++; pa++;
        *pc = *pa >> b;
    }
    else {
        *pc = *pa ;
        pc++; pa++;
        *pc = *pa ;
    }
    return c;

}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64ShL(const DRM_UINT64 a,const unsigned int b)
{
    DRM_UINT64 c;
    unsigned long int *pa, *pc, td;
    pa = (unsigned long int *)&(a.val);
    pc = (unsigned long int *)&(c.val);

    if (b > 63) {
        *pc = 0;
        pc++;
        *pc = 0;
    }
    else if (b > 31){
        *pc = 0;
        pc++;
        *pc = *pa << (b - 32);
    }
    else if (b >0) {
        *pc = *pa << b;
        td  = *pa >> (32 - b);
        pc++; pa++;
        *pc = *pa << b;
        *pc |= td;
    }
    else {
        *pc = *pa ;
        pc++; pa++;
        *pc = *pa ;
    }
    return c;
}


DRM_API int DRM_CALL DRM_UI64Eql(const DRM_UINT64 a, const DRM_UINT64 b)
{
    long int *pa, *pb;
    pa = (long int *)&(a.val);
    pb = (long int *)&(b.val);

    return ((*pa == *pb)&&(*(pa+1) == *(pb+1)));
}


DRM_API int DRM_CALL DRM_UI64Les(const DRM_UINT64 a, const DRM_UINT64 b)
{
    unsigned long int *pa, *pb;
    pa = (unsigned long int *)&(a.val);
    pb = (unsigned long int *)&(b.val);

    if (*(pa+1) != *(pb+1))
        return (*(pa+1) < *(pb+1));
    else
        return (*pa < *pb);
}


DRM_API DRM_UINT64 DRM_CALL DRM_UI64(const unsigned long int b)
{
    DRM_UINT64 c;
    unsigned long int *pa;
    pa = (unsigned long int *)&(c.val);
    *pa = b;
    pa++;
    *pa = 0;

    return c;
}

DRM_API DRM_UINT64 DRM_CALL DRM_UI64HL(const unsigned long int a, const unsigned long int b)
{
    DRM_UINT64 c;
    unsigned long int *pa;
    pa = (unsigned long int *)&(c.val);
    *pa = b;
    pa++;
    *pa = a;
    return c;
}

DRM_API unsigned long int DRM_CALL DRM_UI64High32( DRM_UINT64 a )
{
    unsigned long int *pa;
    pa = (unsigned long int *)&(a.val);
    pa++;
    return *pa;
}

DRM_API unsigned long int DRM_CALL DRM_UI64Low32( DRM_UINT64 a )
{
    unsigned long int *pa;
    pa = (unsigned long int *)&(a.val);
    return *pa;
}

#endif /* !DRM_SUPPORT_NATIVE_64BIT_TYPES */

EXIT_PK_NAMESPACE_CODE;
