/*******************************************************************************
* File Name: LCD_Seg_1.c
* Version 1.30
*
* Description:
*  This file provides the source code to the API for the Segment LCD component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LCD_Seg_1.h"

static void LCD_Seg_1_WriteFrameBuffer(reg32 regAddr[], uint32 bitNumber, uint32 state);
static uint32 LCD_Seg_1_ReadFrameBuffer(const reg32 regAddr[], uint32 bitNumber);

/* This section contains look-up tables for different kinds of displays. */
#ifdef LCD_Seg_1_7SEG
    static const uint8 LCD_Seg_1_7SegDigits[] = {
      /*  '0'    '1'    '2'    '3'    '4'    '5'    '6'    '7' */
        0x3fu, 0x06u, 0x5bu, 0x4fu, 0x66u, 0x6du, 0x7du, 0x07u,
      /*  '8'    '9'    'A'    'B'    'C'    'D'    'E'    'F'   null */
        0x7fu, 0x6fu, 0x77u, 0x7cu, 0x39u, 0x5eu, 0x79u, 0x71u, 0x00u};
#endif /* LCD_Seg_1_7SEG */

#ifdef LCD_Seg_1_14SEG
    static const uint16 LCD_Seg_1_14SegChars[] = {
    /*------------------------------------------------------------*/
    /*                           Blank                            */
    /*------------------------------------------------------------*/
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    /*------------------------------------------------------------*/
    /*         !       "       #       $       %       &       '  */
    /*------------------------------------------------------------*/
    0x0000u,0x0006u,0x0120u,0x3fffu,0x156du,0x2ee4u,0x2a8du,0x0200u,
    /*------------------------------------------------------------*/
    /* (       )       *       +       ,       -       .       /  */
    /*------------------------------------------------------------*/
    0x0a00u,0x2080u,0x3fc0u,0x1540u,0x2000u,0x0440u,0x1058u,0x2200u,
    /*------------------------------------------------------------*/
    /* 0       1       2       3       4       5       6       7  */
    /*------------------------------------------------------------*/
    0x223fu,0x0206u,0x045bu,0x040fu,0x0466u,0x0869u,0x047du,0x1201u,
    /*------------------------------------------------------------*/
    /* 8       9       :       ;       <       =       >       ?  */
    /*------------------------------------------------------------*/
    0x047fu,0x046fu,0x1100u,0x2100u,0x0a00u,0x0448u,0x2080u,0x1423u,
    /*------------------------------------------------------------*/
    /* @       A       B       C       D       E       F       G  */
    /*------------------------------------------------------------*/
    0x053bu,0x0477u,0x150fu,0x0039u,0x110Fu,0x0079u,0x0071u,0x043du,
    /*------------------------------------------------------------*/
    /* H       I       J       K       L       M       N       O  */
    /*------------------------------------------------------------*/
    0x0476u,0x1100u,0x001eu,0x0a70u,0x0038u,0x02b6u,0x08b6u,0x003fu,
    /*------------------------------------------------------------*/
    /* P       Q       R       S       T       U       V       W  */
    /*------------------------------------------------------------*/
    0x0473u,0x083fu,0x0C73u,0x046du,0x1101u,0x003eu,0x2230u,0x2836u,
    /*------------------------------------------------------------*/
    /* X       Y       Z       [       \       ]       ^       _  */
    /*------------------------------------------------------------*/
    0x2a80u,0x1462u,0x2209u,0x0039u,0x0880u,0x000fu,0x0003u,0x0008u,
    /*------------------------------------------------------------*/
    /* @       a       b       c       d       e       f       g  */
    /*------------------------------------------------------------*/
    0x053bu,0x0477u,0x150fu,0x0039u,0x110Fu,0x0079u,0x0071u,0x043du,
    /*------------------------------------------------------------*/
    /* h       i       j       k       l       m       n       o  */
    /*------------------------------------------------------------*/
    0x0476u,0x1100u,0x001eu,0x0a70u,0x0038u,0x02b6u,0x08b6u,0x003fu,
    /*------------------------------------------------------------*/
    /* p       q       r       s       t       u       v       w  */
    /*------------------------------------------------------------*/
    0x0473u,0x083fu,0x0C73u,0x046du,0x1101u,0x003eu,0x2230u,0x2836u,
    /*------------------------------------------------------------*/
    /* x       y       z       {       |       }        ~      O  */
    /*------------------------------------------------------------*/
    0x2a80u,0x1280u,0x2209u,0x0e00u,0x1100u,0x20C0u,0x0452u,0x003fu};
#endif /* LCD_Seg_1_14SEG */

#ifdef LCD_Seg_1_16SEG
    static const uint16 LCD_Seg_1_16SegChars[] = {
    /*------------------------------------------------------------*/
    /*                           Blank                            */
    /*------------------------------------------------------------*/
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,0x0000u,
    /*------------------------------------------------------------*/
    /*         !       "       #       $       %       &       '  */
    /*------------------------------------------------------------*/
    0x0000u,0x000cu,0x0480u,0xffffu,0x55bbu,0xdd99u,0xaa3bu,0x0800u,
    /*------------------------------------------------------------*/
    /* (       )       *       +       ,       -       .       /  */
    /*------------------------------------------------------------*/
    0x2800u,0x8200u,0xff00u,0x5500u,0x8000u,0x1100u,0x4160u,0x8800u,
    /*------------------------------------------------------------*/
    /* 0       1       2       3       4       5       6       7  */
    /*------------------------------------------------------------*/
    0x88ffu,0x000cu,0x1177u,0x103fu,0x118cu,0x21b3u,0x11fbu,0x4803u,
    /*------------------------------------------------------------*/
    /* 8       9       :       ;       <       =       >       ?  */
    /*------------------------------------------------------------*/
    0x11ffu,0x11bfu,0x4400u,0x8400u,0x2800u,0x1130u,0x8200u,0x5087u,
    /*------------------------------------------------------------*/
    /* @       A       B       C       D       E       F       G  */
    /*------------------------------------------------------------*/
    0x14f7u,0x11cfu,0x543fu,0x00f3u,0x443fu,0x01f3u,0x01c3u,0x10fbu,
    /*------------------------------------------------------------*/
    /* H       I       J       K       L       M       N       O  */
    /*------------------------------------------------------------*/
    0x11ccu,0x4400u,0x007eu,0x29c0u,0x00f0u,0x0accu,0x22ccu,0x00ffu,
    /*------------------------------------------------------------*/
    /* P       Q       R       S       T       U       V       W  */
    /*------------------------------------------------------------*/
    0x11c7u,0x20ffu,0x31c7u,0x11bbu,0x4403u,0x00fcu,0x88c0u,0xa0ccu,
    /*------------------------------------------------------------*/
    /* X       Y       Z       [       \       ]       ^       _  */
    /*------------------------------------------------------------*/
    0xaa00u,0x5184u,0x8833u,0x4412u,0x2200u,0x4421u,0x0006u,0x0030u,
    /*------------------------------------------------------------*/
    /* @       a       b       c       d       e       f       g  */
    /*------------------------------------------------------------*/
    0x14f7u,0x11cfu,0x543fu,0x00f3u,0x443fu,0x01f3u,0x01c3u,0x10fbu,
    /*------------------------------------------------------------*/
    /* h       i       j       k       l       m       n       o  */
    /*------------------------------------------------------------*/
    0x11ccu,0x4400u,0x007eu,0x29c0u,0x00f0u,0x0accu,0x22ccu,0x00ffu,
    /*------------------------------------------------------------*/
    /* p       q       r       s       t       u       v       w  */
    /*------------------------------------------------------------*/
    0x11c7u,0x20ffu,0x31c7u,0x11bbu,0x4403u,0x00fcu,0x88c0u,0xa0ccu,
    /*------------------------------------------------------------*/
    /* x       y       z       {       |       }        ~         */
    /*------------------------------------------------------------*/
    0xaa00u,0x4A00u,0x8833u,0x3800u,0x4400u,0x8300u,0x1144u,0x0000u};
#endif /* LCD_Seg_1_16SEG */

#ifdef LCD_Seg_1_DOT_MATRIX
    static const uint8 LCD_Seg_1_charDotMatrix[][LCD_Seg_1_DM_CHAR_WIDTH] = {
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x3Eu, 0x1Cu, 0x1Cu, 0x08u, 0x08u},
        {0x08u, 0x08u, 0x1Cu, 0x1Cu, 0x3Eu}, {0x02u, 0x06u, 0x08u, 0x10u, 0x20u}, {0x00u, 0x4Fu, 0x00u, 0x4Fu, 0x00u},
        {0x06u, 0x0Fu, 0x7Fu, 0x01u, 0x7Fu}, {0x48u, 0x56u, 0x55u, 0x35u, 0x09u}, {0x0Cu, 0x0Cu, 0x0Cu, 0x0Cu, 0x0Cu},
        {0x10u, 0x38u, 0x54u, 0x10u, 0x1Fu}, {0x04u, 0x02u, 0x7Fu, 0x02u, 0x04u}, {0x10u, 0x20u, 0x7Fu, 0x20u, 0x10u},
        {0x7Fu, 0x3Eu, 0x1Cu, 0x08u, 0x7Fu}, {0x7Fu, 0x08u, 0x1Cu, 0x3Eu, 0x7Fu}, {0x7Fu, 0x08u, 0x2Au, 0x1Cu, 0x08u},
        {0x08u, 0x1Cu, 0x2Au, 0x08u, 0x7Fu}, {0x02u, 0x0Eu, 0x3Eu, 0x0Eu, 0x02u}, {0x20u, 0x38u, 0x3Eu, 0x38u, 0x20u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x4Fu, 0x00u, 0x00u}, {0x00u, 0x07u, 0x00u, 0x07u, 0x00u},
        {0x14u, 0x7Fu, 0x14u, 0x7Fu, 0x14u}, {0x24u, 0x2Au, 0x7Fu, 0x2Au, 0x12u}, {0x23u, 0x13u, 0x08u, 0x64u, 0x62u},
        {0x36u, 0x49u, 0x55u, 0x22u, 0x50u}, {0x00u, 0x05u, 0x03u, 0x00u, 0x00u}, {0x00u, 0x1Cu, 0x22u, 0x41u, 0x00u},
        {0x00u, 0x41u, 0x22u, 0x1Cu, 0x00u}, {0x14u, 0x08u, 0x3Eu, 0x08u, 0x14u}, {0x08u, 0x08u, 0x3Eu, 0x08u, 0x08u},
        {0x00u, 0x50u, 0x30u, 0x00u, 0x00u}, {0x08u, 0x08u, 0x08u, 0x08u, 0x08u}, {0x00u, 0x60u, 0x60u, 0x00u, 0x00u},
        {0x20u, 0x10u, 0x08u, 0x04u, 0x02u}, {0x3Eu, 0x51u, 0x49u, 0x45u, 0x3Eu}, {0x00u, 0x42u, 0x7Fu, 0x40u, 0x00u},
        {0x42u, 0x61u, 0x51u, 0x49u, 0x46u}, {0x21u, 0x41u, 0x45u, 0x4Bu, 0x31u}, {0x18u, 0x14u, 0x12u, 0x7Fu, 0x10u},
        {0x27u, 0x45u, 0x45u, 0x45u, 0x39u}, {0x3Cu, 0x4Au, 0x49u, 0x49u, 0x30u}, {0x01u, 0x71u, 0x09u, 0x05u, 0x03u},
        {0x36u, 0x49u, 0x49u, 0x49u, 0x36u}, {0x06u, 0x49u, 0x49u, 0x29u, 0x1Eu}, {0x00u, 0x36u, 0x36u, 0x00u, 0x00u},
        {0x00u, 0x56u, 0x36u, 0x00u, 0x00u}, {0x08u, 0x14u, 0x22u, 0x41u, 0x00u}, {0x14u, 0x14u, 0x14u, 0x14u, 0x14u},
        {0x00u, 0x41u, 0x22u, 0x14u, 0x08u}, {0x02u, 0x01u, 0x51u, 0x09u, 0x06u}, {0x32u, 0x49u, 0x79u, 0x41u, 0x3Eu},
        {0x7Eu, 0x11u, 0x11u, 0x11u, 0x7Eu}, {0x7Fu, 0x49u, 0x49u, 0x49u, 0x36u}, {0x3Eu, 0x41u, 0x41u, 0x41u, 0x22u},
        {0x7Fu, 0x41u, 0x41u, 0x22u, 0x1Cu}, {0x7Fu, 0x49u, 0x49u, 0x49u, 0x41u}, {0x7Fu, 0x09u, 0x09u, 0x09u, 0x01u},
        {0x3Eu, 0x41u, 0x49u, 0x49u, 0x3Au}, {0x7Fu, 0x08u, 0x08u, 0x08u, 0x7Fu}, {0x00u, 0x41u, 0x7Fu, 0x41u, 0x00u},
        {0x20u, 0x40u, 0x41u, 0x3Fu, 0x01u}, {0x7Fu, 0x08u, 0x14u, 0x22u, 0x41u}, {0x7Fu, 0x40u, 0x40u, 0x40u, 0x40u},
        {0x7Fu, 0x02u, 0x0Cu, 0x02u, 0x7Fu}, {0x7Fu, 0x04u, 0x08u, 0x10u, 0x7Fu}, {0x3Eu, 0x41u, 0x41u, 0x41u, 0x3Eu},
        {0x7Fu, 0x09u, 0x09u, 0x09u, 0x06u}, {0x3Eu, 0x41u, 0x51u, 0x21u, 0x5Eu}, {0x7Fu, 0x09u, 0x19u, 0x29u, 0x46u},
        {0x46u, 0x49u, 0x49u, 0x49u, 0x31u}, {0x01u, 0x01u, 0x7Fu, 0x01u, 0x01u}, {0x3Fu, 0x40u, 0x40u, 0x40u, 0x3Fu},
        {0x1Fu, 0x20u, 0x40u, 0x20u, 0x1Fu}, {0x3Fu, 0x40u, 0x38u, 0x40u, 0x3Fu}, {0x63u, 0x14u, 0x08u, 0x14u, 0x63u},
        {0x07u, 0x08u, 0x70u, 0x08u, 0x07u}, {0x61u, 0x51u, 0x49u, 0x45u, 0x43u}, {0x00u, 0x7Fu, 0x41u, 0x41u, 0x00u},
        {0x15u, 0x16u, 0x7Cu, 0x16u, 0x15u}, {0x00u, 0x41u, 0x41u, 0x7Fu, 0x00u}, {0x04u, 0x02u, 0x01u, 0x02u, 0x04u},
        {0x40u, 0x40u, 0x40u, 0x40u, 0x40u}, {0x00u, 0x01u, 0x02u, 0x04u, 0x00u}, {0x20u, 0x54u, 0x54u, 0x54u, 0x78u},
        {0x7Fu, 0x48u, 0x44u, 0x44u, 0x38u}, {0x38u, 0x44u, 0x44u, 0x44u, 0x40u}, {0x38u, 0x44u, 0x44u, 0x48u, 0x7Fu},
        {0x38u, 0x54u, 0x54u, 0x54u, 0x18u}, {0x08u, 0x7Eu, 0x09u, 0x01u, 0x02u}, {0x0Cu, 0x52u, 0x52u, 0x52u, 0x3Eu},
        {0x7Fu, 0x08u, 0x04u, 0x04u, 0x78u}, {0x00u, 0x44u, 0x7Du, 0x40u, 0x00u}, {0x20u, 0x40u, 0x40u, 0x3Du, 0x00u},
        {0x7Fu, 0x10u, 0x28u, 0x44u, 0x00u}, {0x00u, 0x41u, 0x7Fu, 0x40u, 0x00u}, {0x7Cu, 0x04u, 0x18u, 0x04u, 0x78u},
        {0x7Cu, 0x08u, 0x04u, 0x04u, 0x78u}, {0x38u, 0x44u, 0x44u, 0x44u, 0x38u}, {0x7Cu, 0x14u, 0x14u, 0x14u, 0x08u},
        {0x08u, 0x14u, 0x14u, 0x18u, 0x7Cu}, {0x7Cu, 0x08u, 0x04u, 0x04u, 0x08u}, {0x48u, 0x54u, 0x54u, 0x54u, 0x20u},
        {0x04u, 0x3Fu, 0x44u, 0x40u, 0x20u}, {0x3Cu, 0x40u, 0x40u, 0x20u, 0x7Cu}, {0x1Cu, 0x20u, 0x40u, 0x20u, 0x1Cu},
        {0x3Cu, 0x40u, 0x30u, 0x40u, 0x3Cu}, {0x44u, 0x28u, 0x10u, 0x28u, 0x44u}, {0x0Cu, 0x50u, 0x50u, 0x50u, 0x3Cu},
        {0x44u, 0x64u, 0x54u, 0x4Cu, 0x44u}, {0x00u, 0x08u, 0x36u, 0x41u, 0x00u}, {0x00u, 0x00u, 0x7Fu, 0x00u, 0x00u},
        {0x00u, 0x41u, 0x36u, 0x08u, 0x00u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0x08u, 0x08u, 0x2Au, 0x1Cu, 0x08u},
        {0x44u, 0x44u, 0x5Fu, 0x44u, 0x44u}, {0x22u, 0x14u, 0x08u, 0x14u, 0x22u}, {0x1Cu, 0x3Eu, 0x3Eu, 0x3Eu, 0x1Cu},
        {0x7Fu, 0x41u, 0x41u, 0x41u, 0x7Fu}, {0x7Fu, 0x5Bu, 0x41u, 0x5Fu, 0x7Fu}, {0x7Fu, 0x4Du, 0x55u, 0x59u, 0x7Fu},
        {0x1Du, 0x15u, 0x17u, 0x00u, 0x00u}, {0x15u, 0x15u, 0x1Fu, 0x00u, 0x00u}, {0x17u, 0x08u, 0x74u, 0x56u, 0x5Du},
        {0x17u, 0x08u, 0x24u, 0x32u, 0x79u}, {0x35u, 0x1Fu, 0x28u, 0x34u, 0x7Au}, {0x08u, 0x14u, 0x2Au, 0x14u, 0x22u},
        {0x22u, 0x14u, 0x2Au, 0x14u, 0x08u}, {0x08u, 0x04u, 0x08u, 0x10u, 0x08u}, {0x14u, 0x0Au, 0x14u, 0x28u, 0x14u},
        {0x2Au, 0x55u, 0x2Au, 0x55u, 0x2Au}, {0x24u, 0x3Bu, 0x2Au, 0x7Eu, 0x2Au}, {0x40u, 0x3Fu, 0x15u, 0x15u, 0x7Fu},
        {0x46u, 0x20u, 0x1Fu, 0x20u, 0x46u}, {0x24u, 0x14u, 0x7Fu, 0x18u, 0x24u}, {0x24u, 0x14u, 0x7Fu, 0x14u, 0x24u},
        {0x44u, 0x6Au, 0x79u, 0x6Au, 0x44u}, {0x40u, 0x44u, 0x7Fu, 0x44u, 0x40u}, {0x7Fu, 0x49u, 0x49u, 0x49u, 0x7Fu},
        {0x02u, 0x4Cu, 0x30u, 0x0Cu, 0x02u}, {0x04u, 0x04u, 0x3Cu, 0x04u, 0x44u}, {0x49u, 0x55u, 0x7Fu, 0x55u, 0x49u},
        {0x3Au, 0x45u, 0x45u, 0x45u, 0x39u}, {0x40u, 0x3Eu, 0x10u, 0x10u, 0x1Eu}, {0x08u, 0x54u, 0x3Eu, 0x15u, 0x08u},
        {0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu}, {0x55u, 0x2Au, 0x55u, 0x2Au, 0x55u}, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u},
        {0x70u, 0x50u, 0x70u, 0x00u, 0x00u}, {0x00u, 0x00u, 0x0Fu, 0x01u, 0x01u}, {0x40u, 0x40u, 0x70u, 0x00u, 0x00u},
        {0x10u, 0x20u, 0x40u, 0x00u, 0x00u}, {0x00u, 0x18u, 0x18u, 0x00u, 0x00u}, {0x0Au, 0x0Au, 0x4Au, 0x2Au, 0x1Eu},
        {0x04u, 0x44u, 0x34u, 0x14u, 0x0Cu}, {0x20u, 0x10u, 0x78u, 0x04u, 0x00u}, {0x18u, 0x08u, 0x4Cu, 0x48u, 0x38u},
        {0x48u, 0x48u, 0x78u, 0x48u, 0x48u}, {0x48u, 0x28u, 0x18u, 0x78u, 0x08u}, {0x08u, 0x7Cu, 0x08u, 0x28u, 0x18u},
        {0x40u, 0x48u, 0x48u, 0x78u, 0x40u}, {0x54u, 0x54u, 0x54u, 0x7Cu, 0x00u}, {0x18u, 0x00u, 0x58u, 0x40u, 0x38u},
        {0x08u, 0x08u, 0x08u, 0x08u, 0x08u}, {0x01u, 0x41u, 0x3Du, 0x09u, 0x07u}, {0x10u, 0x08u, 0x78u, 0x02u, 0x01u},
        {0x06u, 0x02u, 0x43u, 0x22u, 0x1Eu}, {0x42u, 0x42u, 0x7Eu, 0x42u, 0x42u}, {0x22u, 0x12u, 0x0Au, 0x7Fu, 0x02u},
        {0x42u, 0x3Fu, 0x02u, 0x42u, 0x3Eu}, {0x0Au, 0x0Au, 0x7Fu, 0x0Au, 0x0Au}, {0x08u, 0x46u, 0x42u, 0x22u, 0x1Eu},
        {0x04u, 0x03u, 0x42u, 0x3Eu, 0x02u}, {0x42u, 0x42u, 0x42u, 0x42u, 0x7Eu}, {0x02u, 0x4Fu, 0x22u, 0x1Fu, 0x02u},
        {0x4Au, 0x4Au, 0x40u, 0x20u, 0x1Cu}, {0x42u, 0x22u, 0x12u, 0x2Au, 0x46u}, {0x02u, 0x3Fu, 0x42u, 0x4Au, 0x46u},
        {0x06u, 0x48u, 0x40u, 0x20u, 0x1Eu}, {0x08u, 0x46u, 0x4Au, 0x32u, 0x1Eu}, {0x0Au, 0x4Au, 0x3Eu, 0x09u, 0x08u},
        {0x0Eu, 0x00u, 0x4Eu, 0x20u, 0x1Eu}, {0x04u, 0x45u, 0x3Du, 0x05u, 0x04u}, {0x00u, 0x7Fu, 0x08u, 0x10u, 0x00u},
        {0x44u, 0x24u, 0x1Fu, 0x04u, 0x04u}, {0x40u, 0x42u, 0x42u, 0x42u, 0x40u}, {0x42u, 0x2Au, 0x12u, 0x2Au, 0x06u},
        {0x22u, 0x12u, 0x7Au, 0x16u, 0x22u}, {0x00u, 0x40u, 0x20u, 0x1Fu, 0x00u}, {0x78u, 0x00u, 0x02u, 0x04u, 0x78u},
        {0x3Fu, 0x44u, 0x44u, 0x44u, 0x44u}, {0x02u, 0x42u, 0x42u, 0x22u, 0x1Eu}, {0x04u, 0x02u, 0x04u, 0x08u, 0x30u},
        {0x32u, 0x02u, 0x7Fu, 0x02u, 0x32u}, {0x02u, 0x12u, 0x22u, 0x52u, 0x0Eu}, {0x00u, 0x2Au, 0x2Au, 0x2Au, 0x40u},
        {0x38u, 0x24u, 0x22u, 0x20u, 0x1Eu}, {0x40u, 0x28u, 0x10u, 0x28u, 0x06u}, {0x0Au, 0x3Eu, 0x4Au, 0x4Au, 0x4Au},
        {0x04u, 0x7Fu, 0x04u, 0x14u, 0x0Cu}, {0x40u, 0x42u, 0x42u, 0x7Eu, 0x40u}, {0x4Au, 0x4Au, 0x4Au, 0x4Au, 0x7Eu},
        {0x04u, 0x05u, 0x45u, 0x15u, 0x1Cu}, {0x0Fu, 0x40u, 0x20u, 0x1Fu, 0x00u}, {0x7Cu, 0x00u, 0x7Eu, 0x40u, 0x30u},
        {0x7Eu, 0x40u, 0x20u, 0x10u, 0x08u}, {0x7Eu, 0x7Eu, 0x7Eu, 0x7Eu, 0x7Eu}, {0x0Eu, 0x02u, 0x42u, 0x22u, 0x1Eu},
        {0x42u, 0x42u, 0x40u, 0x20u, 0x18u}, {0x02u, 0x04u, 0x01u, 0x02u, 0x00u}, {0x07u, 0x05u, 0x07u, 0x00u, 0x00u},
        {0x38u, 0x44u, 0x48u, 0x30u, 0x4Cu}, {0x20u, 0x55u, 0x55u, 0x55u, 0x38u}, {0xFEu, 0x15u, 0x15u, 0x15u, 0x0Au},
        {0x28u, 0x54u, 0x54u, 0x44u, 0x20u}, {0xFFu, 0x10u, 0x10u, 0x08u, 0x1Fu}, {0x38u, 0x44u, 0x4Cu, 0x54u, 0x24u},
        {0xFCu, 0x12u, 0x11u, 0x11u, 0x0Eu}, {0x0Eu, 0xA1u, 0xA1u, 0xA1u, 0x7Fu}, {0x20u, 0x40u, 0x3Cu, 0x04u, 0x04u},
        {0x04u, 0x04u, 0x00u, 0x0Eu, 0x00u}, {0x40u, 0x80u, 0x81u, 0x7Fu, 0x00u}, {0x0Au, 0x04u, 0x0Au, 0x00u, 0x00u},
        {0x18u, 0x24u, 0x7Eu, 0x24u, 0x10u}, {0x14u, 0x7Fu, 0x54u, 0x40u, 0x40u}, {0x7Cu, 0x0Au, 0x05u, 0x05u, 0x78u},
        {0x38u, 0x45u, 0x45u, 0x45u, 0x38u}, {0xFFu, 0x12u, 0x11u, 0x11u, 0x0Eu}, {0x0Eu, 0x11u, 0x11u, 0x12u, 0xFFu},
        {0x3Cu, 0x4Au, 0x4Au, 0x4Au, 0x3Cu}, {0x30u, 0x28u, 0x10u, 0x28u, 0x18u}, {0x58u, 0x64u, 0x04u, 0x64u, 0x58u},
        {0x3Cu, 0x41u, 0x40u, 0x21u, 0x7Cu}, {0x63u, 0x55u, 0x4Au, 0x41u, 0x41u}, {0x44u, 0x3Cu, 0x04u, 0x7Cu, 0x44u},
        {0x45u, 0x29u, 0x11u, 0x29u, 0x45u}, {0x0Fu, 0x90u, 0x90u, 0x90u, 0x7Fu}, {0x14u, 0x14u, 0x7Cu, 0x14u, 0x12u},
        {0x44u, 0x3Cu, 0x14u, 0x14u, 0x74u}, {0x7Cu, 0x14u, 0x1Cu, 0x14u, 0x7Cu}, {0x10u, 0x10u, 0x54u, 0x10u, 0x10u},
        {0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}
        };
#endif /* LCD_Seg_1_DOT_MATRIX */

/* Start of customizer generated code */
static const uint32 LCD_Seg_1_dividersLS[][LCD_Seg_1_CONTRAST_DIVIDERS_NUMBER] = {
{0xD5u, 0xD5u, 0xD5u, 0xD5u, 0xD5u, 0xD5u, 0xD5u, 0xD5u, 0x6Bu, 0x00u},
{0x6Au, 0x6Au, 0x6Au, 0x6Au, 0x6Au, 0x6Au, 0x6Au, 0x6Au, 0x77u, 0x84u}};
static const uint32 LCD_Seg_1_dividersHS[][LCD_Seg_1_CONTRAST_DIVIDERS_NUMBER] = {
{0x001Eu, 0x001Bu, 0x0017u, 0x0014u, 0x0011u, 0x000Du, 0x000Au, 0x0007u, 0x0003u, 0x0000u},
{0x0000u, 0x0001u, 0x0001u, 0x0001u, 0x0002u, 0x0002u, 0x0002u, 0x0003u, 0x0003u, 0x0003u}};

uint32 LCD_Seg_1_initVar = 0u;
static uint32 LCD_Seg_1_modeState = LCD_Seg_1_LCD_MODE;
static uint32 LCD_Seg_1_invertState = LCD_Seg_1_STATE_NORMAL;
static uint32 LCD_Seg_1_contrastState = LCD_Seg_1_CONTRAST;
static uint32 LCD_Seg_1_commons[] = {LCD_Seg_1_Com0, LCD_Seg_1_Com1};


/*******************************************************************************
* Function Name: LCD_Seg_1_WriteFrameBuffer
********************************************************************************
*
* Summary:
*  Sets or clears a particular bit in the frame buffer.
*
* Parameters:
*  reg32 regAddress[]: Frame buffer register address.
*  bitNumber : The predefined packed number that points to the bit's location
*              in the frame buffer.
*  state : Specifies bit state.
*
* Return:
*  None.
*
*******************************************************************************/
static void LCD_Seg_1_WriteFrameBuffer(reg32 regAddr[], uint32 bitNumber, uint32 state)
{
    uint32 row;
    uint32 port;
    uint32 pin;
    uint32 mask;

    /* Extract the bit information to locate desired bit in the frame buffer */
    row  = LCD_Seg_1_EXTRACT_ROW(bitNumber) % LCD_Seg_1_MAX_BUFF_ROWS;
    port = LCD_Seg_1_EXTRACT_PORT(bitNumber) + ((LCD_Seg_1_EXTRACT_ROW(bitNumber) /
           LCD_Seg_1_MAX_BUFF_ROWS) * LCD_Seg_1_DATA_REGS_OFFSET);
    pin  = LCD_Seg_1_EXTRACT_PIN(bitNumber) * LCD_Seg_1_MAX_BUFF_ROWS;

    /* Write new bit value to the frame buffer. */
    mask = (uint32)(~((uint32)(LCD_Seg_1_PIXEL_STATE_ON << (pin + row))));

    regAddr[port] = (regAddr[port] & mask) | (uint32)((state & LCD_Seg_1_PIXEL_STATE_ON) << (pin + row));
}


/*******************************************************************************
* Function Name: LCD_Seg_1_ReadFrameBuffer
********************************************************************************
*
* Summary:
*  Gets state of a particular bit in the frame buffer.
*
* Parameters:
*  reg32 regAddress[]: Frame buffer register address.
*  bitNumber : The predefined packed number that points to the bit's location
*              in the frame buffer.
*
* Return:
*  Returns the current status of the specified bit in the frame buffer.
*
*******************************************************************************/
static uint32 LCD_Seg_1_ReadFrameBuffer(const reg32 regAddr[], uint32 bitNumber)
{
    uint32 row;
    uint32 port;
    uint32 pin;
    uint32 pixelState;

    /* Extract the bit information to locate desired bit in the frame buffer */
    row  = LCD_Seg_1_EXTRACT_ROW(bitNumber) % LCD_Seg_1_MAX_BUFF_ROWS;
    port = LCD_Seg_1_EXTRACT_PORT(bitNumber) + ((LCD_Seg_1_EXTRACT_ROW(bitNumber) /
           LCD_Seg_1_MAX_BUFF_ROWS) * LCD_Seg_1_DATA_REGS_OFFSET);
    pin  = LCD_Seg_1_EXTRACT_PIN(bitNumber) * LCD_Seg_1_MAX_BUFF_ROWS;

    pixelState = (uint32)(regAddr[port] >> (pin + row)) & LCD_Seg_1_PIXEL_STATE_ON;

    return(pixelState);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Init
********************************************************************************
*
* Summary:
*  Initialize/Restore default Segment LCD configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Block will be stopped to change settings.
*
*******************************************************************************/
void LCD_Seg_1_Init(void)
{
    LCD_Seg_1_Stop();

    /* Set sub-frame and dead time dividers */
    LCD_Seg_1_DIVIDER_REG = LCD_Seg_1_INIT_DIVIDERS;

    /* Set Configuration Register */
    LCD_Seg_1_CONTROL_REG = LCD_Seg_1_INIT_CONTROL;

    /* Need to clear display to start normal work.
    *  LCD_Seg_1_ClearDisplay() function initializes Commons in the frame buffer as well.
    */
    LCD_Seg_1_ClearDisplay();
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Enable
********************************************************************************
*
* Summary:
*  Enables the Segment LCD.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LCD_Seg_1_modeState - holds the current LCD speed mode (LS/HS).
*
*******************************************************************************/
void LCD_Seg_1_Enable(void)
{
    if (LCD_Seg_1_SPEED_LS == LCD_Seg_1_modeState)
    {
        LCD_Seg_1_CONTROL_REG = (LCD_Seg_1_CONTROL_REG & ((uint32)(~LCD_Seg_1_ENABLE_MASK)))
                                       | LCD_Seg_1_LS_EN;
    }
    else /* (LCD_Seg_1_SPEED_HS == LCD_Seg_1_modeState) */
    {
        LCD_Seg_1_CONTROL_REG = (LCD_Seg_1_CONTROL_REG & ((uint32)(~LCD_Seg_1_ENABLE_MASK)))
                                       | LCD_Seg_1_HS_EN;
    }
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Start
********************************************************************************
*
* Summary:
*  Initialize the Segment LCD with default customizer values and enables the
*  Segment LCD.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LCD_Seg_1_initVar - is used to indicate initial configuration of
*  this component. The variable is initialized to zero and set to 1 the
*  first time LCD_Seg_1_Start() is called. This allows for component
*  initialization without re-initialization in all subsequent calls to the
*  LCD_Seg_1_Start() routine.
*
*******************************************************************************/
void LCD_Seg_1_Start(void)
{
    /* If not Initialized then initialize all required hardware and software */
    if (LCD_Seg_1_initVar == 0u)
    {
        LCD_Seg_1_initVar = 1u;
        LCD_Seg_1_Init();
    }

    LCD_Seg_1_Enable();
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Stop
********************************************************************************
*
* Summary:
*  Disables the Segment LCD.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_Stop(void)
{
    LCD_Seg_1_CONTROL_REG &= (uint32)(~LCD_Seg_1_ENABLE_MASK);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_SetSpeedMode
********************************************************************************
*
* Summary:
*  Sets HS or LS LCD speed mode.
*
* Parameters:
*  mode : Sets the mode of the LCD clock speed operation:
*    Define                         Description
*     LCD_Seg_1_SPEED_LS      Low Speed mode.
*     LCD_Seg_1_SPEED_HS      High Speed mode.
*
* Return:
*  None.
*
* Global variables:
*  LCD_Seg_1_modeState - holds the current LCD speed mode (LS/HS).
*  LCD_Seg_1_contrastState - holds the current LCD contrast.
*
*******************************************************************************/
void LCD_Seg_1_SetSpeedMode(uint32 mode)
{
    uint32 contrast = (LCD_Seg_1_contrastState / 10u) - 1u;

    if (LCD_Seg_1_modeState != mode)
    {
        LCD_Seg_1_CONTROL_REG = (LCD_Seg_1_CONTROL_REG & ((uint32)(~LCD_Seg_1_MODE)))
                                       | (((uint32)(mode  << LCD_Seg_1_MODE_SHIFT)) & LCD_Seg_1_MODE);

        if (LCD_Seg_1_SPEED_LS == mode)
        {
            /* Set sub-frame and dead time dividers */
            LCD_Seg_1_DIVIDER_REG = (LCD_Seg_1_DIVIDER_REG & ((uint32)(~LCD_Seg_1_DIVIDER_MASK)))
                            | LCD_Seg_1_dividersLS[LCD_Seg_1_SUBFR_DIVS][contrast]
                            | ((uint32)((uint32)LCD_Seg_1_dividersLS[LCD_Seg_1_DEAD_DIVS][contrast] <<
                                                LCD_Seg_1_DEAD_DIV_MASK_SHIFT));

            LCD_Seg_1_modeState = LCD_Seg_1_SPEED_LS;
        }
        else /* (LCD_Seg_1_SPEED_HS == mode) */
        {
            /* Set sub-frame and dead time dividers */
            LCD_Seg_1_DIVIDER_REG = (LCD_Seg_1_DIVIDER_REG & ((uint32)(~LCD_Seg_1_DIVIDER_MASK)))
                            | LCD_Seg_1_dividersHS[LCD_Seg_1_SUBFR_DIVS][contrast]
                            | ((uint32)((uint32)LCD_Seg_1_dividersHS[LCD_Seg_1_DEAD_DIVS][contrast] <<
                                                LCD_Seg_1_DEAD_DIV_MASK_SHIFT));

            LCD_Seg_1_modeState = LCD_Seg_1_SPEED_HS;
        }
    }
}


/*******************************************************************************
* Function Name: LCD_Seg_1_SetOperationMode
********************************************************************************
*
* Summary:
*  Sets PWM or Digital Correlation LCD operation mode.
*
* Parameters:
*  mode : Sets the mode of the LCD operation:
*    Define                         Description
*     LCD_Seg_1_MODE_PWM      PWM operation mode.
*     LCD_Seg_1_MODE_DIG_COR  Digital Correlation mode.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_SetOperationMode(uint32 mode)
{
    LCD_Seg_1_CONTROL_REG = (LCD_Seg_1_CONTROL_REG & ((uint32)(~LCD_Seg_1_OP_MODE)))
                                   | (((uint32)(mode << LCD_Seg_1_OP_MODE_SHIFT)) & LCD_Seg_1_OP_MODE);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_SetBiasType
********************************************************************************
*
* Summary:
*  Sets bias type for PWM operation mode.
*
* Parameters:
*  bias : Sets the bias type for PWM operation mode:
*    Define                         Description
*     LCD_Seg_1_BIAS_1_2      1/2 Bias.
*     LCD_Seg_1_BIAS_1_3      1/3 Bias.
*     LCD_Seg_1_BIAS_1_4      1/4 Bias. Not supported in Low Speed mode.
*     LCD_Seg_1_BIAS_1_5      1/5 Bias. Not supported in Low Speed mode.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_SetBiasType(uint32 bias)
{
    LCD_Seg_1_CONTROL_REG = (LCD_Seg_1_CONTROL_REG & ((uint32)(~LCD_Seg_1_BIAS_MASK)))
                                 | (((uint32)(bias << LCD_Seg_1_BIAS_MASK_SHIFT)) & LCD_Seg_1_BIAS_MASK);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_SetWaveformType
********************************************************************************
*
* Summary:
*  Sets the LCD driving Waveform Type.
*
* Parameters:
*  type : Sets the Waveform Type:
*    Define                         Description
*     LCD_Seg_1_TYPE_A        Waveform type A.
*     LCD_Seg_1_TYPE_B        Waveform type B.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_SetWaveformType(uint32 type)
{
    LCD_Seg_1_CONTROL_REG = (LCD_Seg_1_CONTROL_REG & ((uint32)(~LCD_Seg_1_TYPE)))
                                   | (((uint32)(type << LCD_Seg_1_TYPE_SHIFT)) & LCD_Seg_1_TYPE);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_SetContrast
********************************************************************************
*
* Summary:
*  Sets the contrast control using "Dead Period" digital modulation.
*
* Parameters:
*  contrast : Sets the contrast for the LCD glass in percentage. Valid range
*             from 10% to 100% in 10% increments. Valid range can be restricted
*             because of dividers size (for LS mode 8 bit and for HS mode 16
*             bit). For greater frequencies, certain ratios between contrast and
*             frame rate can not be achieved.
*
* Return:
*  Pass or fail based on a validity check of the contrast value.
*    Define                     Description
*     CYRET_SUCCESS              Function completed successfully.
*     CYRET_BAD_PARAM            Evaluation of contrast parameter is failed.
*
* Global variables:
*  LCD_Seg_1_modeState - holds the current LCD speed mode (LS/HS).
*  LCD_Seg_1_contrastState - holds the current LCD contrast.
*
*******************************************************************************/
uint32 LCD_Seg_1_SetContrast(uint32 contrast)
{
    uint32 status = CYRET_BAD_PARAM;
    uint32 contrastIndex;

    if ((LCD_Seg_1_MIN_CONTRAST <= contrast) && (contrast <= LCD_Seg_1_MAX_CONTRAST))
    {
        contrastIndex = (contrast / 10u) - 1u;
        if (LCD_Seg_1_SPEED_LS == LCD_Seg_1_modeState)
        {
            if (LCD_Seg_1_dividersLS[LCD_Seg_1_DEAD_DIVS][contrastIndex] !=
                LCD_Seg_1_dividersLS[LCD_Seg_1_DEAD_DIVS][contrastIndex + 1u])
            {
                /* Set sub-frame and dead time dividers */
                LCD_Seg_1_DIVIDER_REG = (LCD_Seg_1_DIVIDER_REG
                        & ((uint32)(~LCD_Seg_1_DIVIDER_MASK)))
                        | LCD_Seg_1_dividersLS[LCD_Seg_1_SUBFR_DIVS][contrastIndex]
                        | ((uint32)((uint32)LCD_Seg_1_dividersLS[LCD_Seg_1_DEAD_DIVS][contrastIndex] <<
                                            LCD_Seg_1_DEAD_DIV_MASK_SHIFT));
                status = CYRET_SUCCESS;
            }
        }
        else /* (LCD_Seg_1_SPEED_HS == LCD_Seg_1_modeState) */
        {
            if (LCD_Seg_1_dividersHS[LCD_Seg_1_DEAD_DIVS][contrastIndex] !=
                LCD_Seg_1_dividersHS[LCD_Seg_1_DEAD_DIVS][contrastIndex + 1u])
            {
                /* Set sub-frame and dead time dividers */
                LCD_Seg_1_DIVIDER_REG = (LCD_Seg_1_DIVIDER_REG
                        & ((uint32)(~LCD_Seg_1_DIVIDER_MASK)))
                        | LCD_Seg_1_dividersHS[LCD_Seg_1_SUBFR_DIVS][contrastIndex]
                        | ((uint32)((uint32)LCD_Seg_1_dividersHS[LCD_Seg_1_DEAD_DIVS][contrastIndex] <<
                                            LCD_Seg_1_DEAD_DIV_MASK_SHIFT));
                status = CYRET_SUCCESS;
            }
        }

        LCD_Seg_1_contrastState = contrast; /* Saves new contrast value */
    }

    return(status);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_WriteInvertState
********************************************************************************
*
* Summary:
*  Inverts the display based on an input parameter.
*
* Parameters:
*  invertState : Sets the invert state of the display:
*    Define                            Description
*     LCD_Seg_1_STATE_NORMAL     Normal non inverted display.
*     LCD_Seg_1_STATE_INVERTED   Inverted display.
*
* Return:
*  None.
*
* Global variables:
*  LCD_Seg_1_invertState - holds the current LCD state (Inverted display
*                                 or Normal (non inverted) display).
*  LCD_Seg_1_commons[] - holds the pixel locations for common lines.
*
*******************************************************************************/
void LCD_Seg_1_WriteInvertState(uint32 invertState)
{
    uint32 i;
    uint32 j;

    if (invertState != LCD_Seg_1_invertState)
    {
        /* Invert entire frame buffer */
        LCD_Seg_1_DATA0_REG = (uint32)(~LCD_Seg_1_DATA0_REG);
        LCD_Seg_1_DATA1_REG = (uint32)(~LCD_Seg_1_DATA1_REG);
        LCD_Seg_1_DATA2_REG = (uint32)(~LCD_Seg_1_DATA2_REG);
        LCD_Seg_1_DATA3_REG = (uint32)(~LCD_Seg_1_DATA3_REG);
        LCD_Seg_1_DATA4_REG = (uint32)(~LCD_Seg_1_DATA4_REG);

        /* Reinitialize the commons */
        for (i = 0u; i < LCD_Seg_1_COM_NUM; i++)
        {
            /* Clear commons pin data after frame buffer inverting */
            for (j = 0u; j < LCD_Seg_1_COM_NUM; j++)
            {
                LCD_Seg_1_WriteFrameBuffer(LCD_Seg_1_DATA0_PTR,
                ((LCD_Seg_1_commons[i] & ((uint32)(~LCD_Seg_1_ROW_MASK))) |
                ((uint32)(j << LCD_Seg_1_ROW_SHIFT))), LCD_Seg_1_PIXEL_STATE_OFF);
            }

            /* Set pin data for commons */
            LCD_Seg_1_WriteFrameBuffer(LCD_Seg_1_DATA0_PTR, LCD_Seg_1_commons[i],
                                              LCD_Seg_1_PIXEL_STATE_ON);
        }

        LCD_Seg_1_invertState = invertState;
    }
}


/*******************************************************************************
* Function Name: LCD_Seg_1_ReadInvertState
********************************************************************************
*
* Summary:
*  Returns the current value of the display invert state: normal or inverted.
*
* Parameters:
*  None.
*
* Return:
*  The invert state of the display:
*    Define                            Description
*     LCD_Seg_1_STATE_NORMAL     Normal non inverted display.
*     LCD_Seg_1_STATE_INVERTED   Inverted display.
*
* Global variables:
*  LCD_Seg_1_invertState - holds the current LCD state (Inverted display
*                                 or Normal (non inverted) display).
*
*******************************************************************************/
uint32 LCD_Seg_1_ReadInvertState(void)
{
    return (LCD_Seg_1_invertState);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_ClearDisplay
********************************************************************************
*
* Summary:
*  This function clears the display and the associated frame buffer RAM.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LCD_Seg_1_commons[] - holds the pixel locations for common lines.
*  LCD_Seg_1_invertState - holds the current LCD state (Inverted display
*                                 or Normal (non inverted) display).
*
*******************************************************************************/
void LCD_Seg_1_ClearDisplay(void)
{
    uint32 i;

    /* Clear entire frame buffer to all zeroes */
    LCD_Seg_1_DATA0_REG = 0Lu;
    LCD_Seg_1_DATA1_REG = 0Lu;
    LCD_Seg_1_DATA2_REG = 0Lu;
    LCD_Seg_1_DATA3_REG = 0Lu;
    LCD_Seg_1_DATA4_REG = 0Lu;

    /* Reinitialize the commons */
    for (i = 0u; i < LCD_Seg_1_COM_NUM; i++)
    {
        LCD_Seg_1_WriteFrameBuffer(LCD_Seg_1_DATA0_PTR, LCD_Seg_1_commons[i],
                                          LCD_Seg_1_PIXEL_STATE_ON);
    }

    /* If we were in inverted state before the display was cleared, then we must
    * call WriteInvertState() as there is no separate API to clear inverted
    * display.
    */
    if (LCD_Seg_1_invertState == LCD_Seg_1_STATE_INVERTED)
    {
        LCD_Seg_1_WriteInvertState(LCD_Seg_1_STATE_INVERTED);
    }
}


/*******************************************************************************
* Function Name: LCD_Seg_1_WritePixel
********************************************************************************
*
* Summary:
*  Sets or clears a pixel based on the input parameter PixelState. The pixel is
*  addressed by a packed number. This packed number comes from pixel mapping
*  table of Display helpers. Pixel mapping table used to map the helper function
*  pixel to the actual frame buffer pixel. All pixels have packed number defines
*  which are resides in component header file.
*
* Parameters:
*  pixelNumber : The predefined packed number that points to the pixel's
*                location in the frame buffer. The LSB are the bit position in
*                the byte, the LSB+1 is the byte address in the multiplex row
*                and the MSB-1 is the multiplex row number.
*  pixelState : The pixelNumber specified is set to this pixel state:
*    Define                                Description
*     LCD_Seg_1_PIXEL_STATE_ON        Pixel on.
*     LCD_Seg_1_PIXEL_STATE_OFF       Pixel off.
*     LCD_Seg_1_PIXEL_STATE_INVERT    Pixel invert.
*
* Return:
*  Pass or fail based on a range check of the pixelNumber address:
*    Define                     Description
*     CYRET_SUCCESS              Function completed successfully.
*     CYRET_BAD_PARAM            Evaluation of pixelNumber parameter is failed.
*
*******************************************************************************/
uint32 LCD_Seg_1_WritePixel(uint32 pixelNumber, uint32 pixelState)
{
    uint32 status = CYRET_BAD_PARAM;

    if ((LCD_Seg_1_NOT_CON != pixelNumber) && (pixelState <= LCD_Seg_1_PIXEL_STATE_INVERT))
    {
        if (LCD_Seg_1_PIXEL_STATE_INVERT == pixelState)
        {
            /* Invert actual pixel state */
            pixelState = LCD_Seg_1_ReadPixel(pixelNumber);
            pixelState = ((uint32)(~pixelState)) & LCD_Seg_1_PIXEL_STATE_ON;
        }

        /* Write new pixel's value to the frame buffer. */
        LCD_Seg_1_WriteFrameBuffer(LCD_Seg_1_DATA0_PTR, pixelNumber, pixelState);

        status = CYRET_SUCCESS;
    }

    return(status);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_ReadPixel
********************************************************************************
*
* Summary:
*  Reads the state of a pixel in the frame buffer. The pixel is addressed by a
*  packed number. This packed number comes from pixel mapping table of Display
*  helpers. Pixel mapping table used to map the helper function pixel to the
*  actual frame buffer pixel. All pixels have packed number defines which are
*  resides in component header file.
*
* Parameters:
*  pixelNumber : The predefined packed number that points to the pixel's
*                location in the frame buffer. The LSB are the bit position in
*                the byte, the LSB+1 is the byte address in the multiplex row
*                and the MSB-1 is the multiplex row number.
*
* Return:
*  Returns the current status of the PixelNumber specified:
*    Define                                Description
*     LCD_Seg_1_PIXEL_STATE_ON        Pixel on.
*     LCD_Seg_1_PIXEL_STATE_OFF       Pixel off.
*     LCD_Seg_1_PIXEL_UNKNOWN_STATE   Not assigned pixel.
*
*******************************************************************************/
uint32 LCD_Seg_1_ReadPixel(uint32 pixelNumber)
{
    uint32 pixelState = LCD_Seg_1_PIXEL_UNKNOWN_STATE;

    if (LCD_Seg_1_NOT_CON != pixelNumber)
    {
        pixelState = LCD_Seg_1_ReadFrameBuffer(LCD_Seg_1_DATA0_PTR, pixelNumber);
    }

    return(pixelState);
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Write7SegDigit_0
********************************************************************************
*
* Summary:
*  This function displays a hexadecimal digit on an array of 7-segment display
*  elements. Digits can be hexadecimal values in the range of 0 to 9 and A to F.
*  The customizer Display Helpers facility must be used to define the pixel set
*  associated with the 7-segment display elements. Multiple 7-segment display
*  elements can be defined in the frame buffer and are addressed through the
*  suffix (n) in the function name. This function is only included if a
*  7-segment display element is defined in the component customizer.
*
* Parameters:
*  digit : Unsigned integer value in the range of 0 to 15 to be displayed as a
*          hexadecimal digit. The ASCII numbers of a hexadecimal characters are
*          also valid. In case of a invalid digit value displays 0 in position
*          specified.
*  position : Position of the digit as counted right to left starting at 0 on
*             the right. If the position is outside the defined display area,
*             the character will not be displayed.
*
* Return: 
*  None.
*
* Global variables:
*  LCD_Seg_1_7SegDigits[] - used as a look-up table for 7 Segment helper.
*  Holds decoded digit's pixel reflection for the helper.
*
*  LCD_Seg_1_DIGIT_NUM_0 - holds the maximum digit number for the helper.
*
*******************************************************************************/
void LCD_Seg_1_Write7SegDigit_0(uint32 digit, uint32 position)
{
    static const uint32 LCD_Seg_1_disp0[1u][7u] = {{LCD_Seg_1_NOT_CON, LCD_Seg_1_NOT_CON, LCD_Seg_1_NOT_CON,
    LCD_Seg_1_NOT_CON, LCD_Seg_1_H7SEG0_E, LCD_Seg_1_H7SEG0_F, LCD_Seg_1_H7SEG0_G}};
    uint32 i;

    /* if digit < 16 then do nothing (we have correct data) */
    if (digit <= 16u)
    {
        /* nothing to do, as we have correct digit value */
    }
    /* if digit <= 0x39 then digit is ASCII code of a number (0-9) */
    else if (digit <= 0x39u)
    {
        digit -= 0x30u;
    }
    /* if digit <= 0x46 then digit is ASCII code of a hex number (A-F) */
    else if (digit <= 0x46u)
    {
        digit -= 0x37u;
    }
    /* else we have invalid digit, and we will print '0' instead */
    else
    {
        digit = 0u;
    }

    if ((position / LCD_Seg_1_DIGIT_NUM_0) == 0u)
    {
        position = (LCD_Seg_1_DIGIT_NUM_0 - position) - 1u;
        for (i = 0u; i < LCD_Seg_1_7SEG_PIX_NUM; i++)
        {
            (void)LCD_Seg_1_WritePixel(LCD_Seg_1_disp0[position][i],
            (((uint32)((uint32)LCD_Seg_1_7SegDigits[digit] >> i)) & LCD_Seg_1_PIXEL_STATE_ON));
        }
    }
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Write7SegNumber_0
********************************************************************************
*
* Summary:
*  This function displays a 16-bit integer value on a 1- to 5-digit array of
*  7-segment display elements. The customizer Display Helpers facility must be
*  used to define the pixel set associated with the 7-segment display element/s.
*  Multiple 7-segment display element groups can be defined in the frame buffer
*  and are addressed through the suffix (n) in the function name. Sign 
*  conversion, sign display, decimal points, and other custom features must be
*  handled by application-specific user code. This function is only included if
*  a 7-segment display element is defined in the component customizer.
*
* Parameters:
*  value:    Unsigned integer value to be displayed.
*  position: The position of the least significant digit as counted right to
*            left starting at 0 on the right. If the defined display contains 
*            fewer digits then the value requires for display for the most 
*            significant digit/s will not be displayed.
*  mode:     Sets the display mode.
*    Define                         Description
*     LCD_Seg_1_NO_LEADING_ZEROES          No leading zeroes.
*     LCD_Seg_1_LEADING_ZEROES             Leading zeroes are displayed.
*
* Return: 
*  None.
*
* Global variables:
*  LCD_Seg_1_DIGIT_NUM_0 - holds the maximum digit number for the helper.
*
*******************************************************************************/
void LCD_Seg_1_Write7SegNumber_0(uint32 value, uint32 position, uint32 mode)
{ 
    uint32 i;
    uint32 num;

    position = position % LCD_Seg_1_DIGIT_NUM_0;

    /* While current digit position in the range of display keep processing the output */
    for (i = position; i < LCD_Seg_1_DIGIT_NUM_0; i++)
    {
        num = value % 10u;
        if((0u == value) && (0u == mode))
        {
            /* In case of 'no leading zeros' each extra digit will be cleared */
            LCD_Seg_1_Write7SegDigit_0(LCD_Seg_1_7SEG_BLANK_DIG, i);
        }
        else
        {
            /* Display subsequent digit or '0' if 'no leading zeros' mode */
            LCD_Seg_1_Write7SegDigit_0(num, i);
        }
        value = value / 10u;
    }
}


/* [] END OF FILE */
