#ifndef INCLUDED_CYFITTERIAR_INC
#define INCLUDED_CYFITTERIAR_INC
    INCLUDE cydeviceiar_trm.inc

/* Clock_1 */
Clock_1__CTRL_REGISTER EQU CYREG_PERI_PCLK_CTL11
Clock_1__DIV_ID EQU 0x00000040
Clock_1__DIV_REGISTER EQU CYREG_PERI_DIV_16_CTL0
Clock_1__PA_DIV_ID EQU 0x000000FF

/* LED_Red_1 */
LED_Red_1__0__DR EQU CYREG_GPIO_PRT2_DR
LED_Red_1__0__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Red_1__0__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Red_1__0__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Red_1__0__HSIOM EQU CYREG_HSIOM_PORT_SEL2
LED_Red_1__0__HSIOM_MASK EQU 0x0000000F
LED_Red_1__0__HSIOM_SHIFT EQU 0
LED_Red_1__0__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_1__0__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Red_1__0__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_1__0__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Red_1__0__MASK EQU 0x01
LED_Red_1__0__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Red_1__0__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Red_1__0__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Red_1__0__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Red_1__0__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Red_1__0__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Red_1__0__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Red_1__0__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Red_1__0__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Red_1__0__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Red_1__0__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Red_1__0__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Red_1__0__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Red_1__0__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Red_1__0__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Red_1__0__PC EQU CYREG_GPIO_PRT2_PC
LED_Red_1__0__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Red_1__0__PORT EQU 2
LED_Red_1__0__PS EQU CYREG_GPIO_PRT2_PS
LED_Red_1__0__SHIFT EQU 0
LED_Red_1__DR EQU CYREG_GPIO_PRT2_DR
LED_Red_1__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Red_1__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Red_1__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Red_1__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_1__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Red_1__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_1__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Red_1__MASK EQU 0x01
LED_Red_1__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Red_1__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Red_1__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Red_1__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Red_1__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Red_1__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Red_1__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Red_1__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Red_1__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Red_1__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Red_1__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Red_1__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Red_1__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Red_1__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Red_1__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Red_1__PC EQU CYREG_GPIO_PRT2_PC
LED_Red_1__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Red_1__PORT EQU 2
LED_Red_1__PS EQU CYREG_GPIO_PRT2_PS
LED_Red_1__SHIFT EQU 0

/* LED_Red_2 */
LED_Red_2__0__DR EQU CYREG_GPIO_PRT2_DR
LED_Red_2__0__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Red_2__0__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Red_2__0__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Red_2__0__HSIOM EQU CYREG_HSIOM_PORT_SEL2
LED_Red_2__0__HSIOM_MASK EQU 0x000000F0
LED_Red_2__0__HSIOM_SHIFT EQU 4
LED_Red_2__0__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_2__0__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Red_2__0__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_2__0__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Red_2__0__MASK EQU 0x02
LED_Red_2__0__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Red_2__0__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Red_2__0__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Red_2__0__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Red_2__0__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Red_2__0__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Red_2__0__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Red_2__0__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Red_2__0__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Red_2__0__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Red_2__0__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Red_2__0__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Red_2__0__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Red_2__0__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Red_2__0__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Red_2__0__PC EQU CYREG_GPIO_PRT2_PC
LED_Red_2__0__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Red_2__0__PORT EQU 2
LED_Red_2__0__PS EQU CYREG_GPIO_PRT2_PS
LED_Red_2__0__SHIFT EQU 1
LED_Red_2__DR EQU CYREG_GPIO_PRT2_DR
LED_Red_2__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Red_2__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Red_2__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Red_2__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_2__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Red_2__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Red_2__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Red_2__MASK EQU 0x02
LED_Red_2__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Red_2__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Red_2__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Red_2__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Red_2__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Red_2__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Red_2__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Red_2__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Red_2__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Red_2__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Red_2__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Red_2__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Red_2__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Red_2__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Red_2__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Red_2__PC EQU CYREG_GPIO_PRT2_PC
LED_Red_2__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Red_2__PORT EQU 2
LED_Red_2__PS EQU CYREG_GPIO_PRT2_PS
LED_Red_2__SHIFT EQU 1

/* Button_Left */
Button_Left__0__DR EQU CYREG_GPIO_PRT3_DR
Button_Left__0__DR_CLR EQU CYREG_GPIO_PRT3_DR_CLR
Button_Left__0__DR_INV EQU CYREG_GPIO_PRT3_DR_INV
Button_Left__0__DR_SET EQU CYREG_GPIO_PRT3_DR_SET
Button_Left__0__HSIOM EQU CYREG_HSIOM_PORT_SEL3
Button_Left__0__HSIOM_MASK EQU 0x0000000F
Button_Left__0__HSIOM_SHIFT EQU 0
Button_Left__0__INTCFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Left__0__INTR EQU CYREG_GPIO_PRT3_INTR
Button_Left__0__INTR_CFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Left__0__INTSTAT EQU CYREG_GPIO_PRT3_INTR
Button_Left__0__MASK EQU 0x01
Button_Left__0__PA__CFG0 EQU CYREG_UDB_PA3_CFG0
Button_Left__0__PA__CFG1 EQU CYREG_UDB_PA3_CFG1
Button_Left__0__PA__CFG10 EQU CYREG_UDB_PA3_CFG10
Button_Left__0__PA__CFG11 EQU CYREG_UDB_PA3_CFG11
Button_Left__0__PA__CFG12 EQU CYREG_UDB_PA3_CFG12
Button_Left__0__PA__CFG13 EQU CYREG_UDB_PA3_CFG13
Button_Left__0__PA__CFG14 EQU CYREG_UDB_PA3_CFG14
Button_Left__0__PA__CFG2 EQU CYREG_UDB_PA3_CFG2
Button_Left__0__PA__CFG3 EQU CYREG_UDB_PA3_CFG3
Button_Left__0__PA__CFG4 EQU CYREG_UDB_PA3_CFG4
Button_Left__0__PA__CFG5 EQU CYREG_UDB_PA3_CFG5
Button_Left__0__PA__CFG6 EQU CYREG_UDB_PA3_CFG6
Button_Left__0__PA__CFG7 EQU CYREG_UDB_PA3_CFG7
Button_Left__0__PA__CFG8 EQU CYREG_UDB_PA3_CFG8
Button_Left__0__PA__CFG9 EQU CYREG_UDB_PA3_CFG9
Button_Left__0__PC EQU CYREG_GPIO_PRT3_PC
Button_Left__0__PC2 EQU CYREG_GPIO_PRT3_PC2
Button_Left__0__PORT EQU 3
Button_Left__0__PS EQU CYREG_GPIO_PRT3_PS
Button_Left__0__SHIFT EQU 0
Button_Left__DR EQU CYREG_GPIO_PRT3_DR
Button_Left__DR_CLR EQU CYREG_GPIO_PRT3_DR_CLR
Button_Left__DR_INV EQU CYREG_GPIO_PRT3_DR_INV
Button_Left__DR_SET EQU CYREG_GPIO_PRT3_DR_SET
Button_Left__INTCFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Left__INTR EQU CYREG_GPIO_PRT3_INTR
Button_Left__INTR_CFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Left__INTSTAT EQU CYREG_GPIO_PRT3_INTR
Button_Left__MASK EQU 0x01
Button_Left__PA__CFG0 EQU CYREG_UDB_PA3_CFG0
Button_Left__PA__CFG1 EQU CYREG_UDB_PA3_CFG1
Button_Left__PA__CFG10 EQU CYREG_UDB_PA3_CFG10
Button_Left__PA__CFG11 EQU CYREG_UDB_PA3_CFG11
Button_Left__PA__CFG12 EQU CYREG_UDB_PA3_CFG12
Button_Left__PA__CFG13 EQU CYREG_UDB_PA3_CFG13
Button_Left__PA__CFG14 EQU CYREG_UDB_PA3_CFG14
Button_Left__PA__CFG2 EQU CYREG_UDB_PA3_CFG2
Button_Left__PA__CFG3 EQU CYREG_UDB_PA3_CFG3
Button_Left__PA__CFG4 EQU CYREG_UDB_PA3_CFG4
Button_Left__PA__CFG5 EQU CYREG_UDB_PA3_CFG5
Button_Left__PA__CFG6 EQU CYREG_UDB_PA3_CFG6
Button_Left__PA__CFG7 EQU CYREG_UDB_PA3_CFG7
Button_Left__PA__CFG8 EQU CYREG_UDB_PA3_CFG8
Button_Left__PA__CFG9 EQU CYREG_UDB_PA3_CFG9
Button_Left__PC EQU CYREG_GPIO_PRT3_PC
Button_Left__PC2 EQU CYREG_GPIO_PRT3_PC2
Button_Left__PORT EQU 3
Button_Left__PS EQU CYREG_GPIO_PRT3_PS
Button_Left__SHIFT EQU 0

/* Button_Left_Int */
Button_Left_Int__INTC_CLR_EN_REG EQU CYREG_CM0_ICER
Button_Left_Int__INTC_CLR_PD_REG EQU CYREG_CM0_ICPR
Button_Left_Int__INTC_MASK EQU 0x02
Button_Left_Int__INTC_NUMBER EQU 1
Button_Left_Int__INTC_PRIOR_MASK EQU 0xC000
Button_Left_Int__INTC_PRIOR_NUM EQU 3
Button_Left_Int__INTC_PRIOR_REG EQU CYREG_CM0_IPR0
Button_Left_Int__INTC_SET_EN_REG EQU CYREG_CM0_ISER
Button_Left_Int__INTC_SET_PD_REG EQU CYREG_CM0_ISPR

/* LED_Green_1 */
LED_Green_1__0__DR EQU CYREG_GPIO_PRT2_DR
LED_Green_1__0__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Green_1__0__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Green_1__0__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Green_1__0__HSIOM EQU CYREG_HSIOM_PORT_SEL2
LED_Green_1__0__HSIOM_MASK EQU 0x00F00000
LED_Green_1__0__HSIOM_SHIFT EQU 20
LED_Green_1__0__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_1__0__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Green_1__0__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_1__0__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Green_1__0__MASK EQU 0x20
LED_Green_1__0__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Green_1__0__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Green_1__0__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Green_1__0__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Green_1__0__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Green_1__0__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Green_1__0__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Green_1__0__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Green_1__0__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Green_1__0__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Green_1__0__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Green_1__0__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Green_1__0__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Green_1__0__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Green_1__0__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Green_1__0__PC EQU CYREG_GPIO_PRT2_PC
LED_Green_1__0__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Green_1__0__PORT EQU 2
LED_Green_1__0__PS EQU CYREG_GPIO_PRT2_PS
LED_Green_1__0__SHIFT EQU 5
LED_Green_1__DR EQU CYREG_GPIO_PRT2_DR
LED_Green_1__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Green_1__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Green_1__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Green_1__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_1__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Green_1__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_1__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Green_1__MASK EQU 0x20
LED_Green_1__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Green_1__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Green_1__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Green_1__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Green_1__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Green_1__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Green_1__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Green_1__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Green_1__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Green_1__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Green_1__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Green_1__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Green_1__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Green_1__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Green_1__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Green_1__PC EQU CYREG_GPIO_PRT2_PC
LED_Green_1__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Green_1__PORT EQU 2
LED_Green_1__PS EQU CYREG_GPIO_PRT2_PS
LED_Green_1__SHIFT EQU 5

/* LED_Green_2 */
LED_Green_2__0__DR EQU CYREG_GPIO_PRT2_DR
LED_Green_2__0__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Green_2__0__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Green_2__0__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Green_2__0__HSIOM EQU CYREG_HSIOM_PORT_SEL2
LED_Green_2__0__HSIOM_MASK EQU 0x000F0000
LED_Green_2__0__HSIOM_SHIFT EQU 16
LED_Green_2__0__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_2__0__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Green_2__0__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_2__0__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Green_2__0__MASK EQU 0x10
LED_Green_2__0__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Green_2__0__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Green_2__0__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Green_2__0__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Green_2__0__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Green_2__0__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Green_2__0__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Green_2__0__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Green_2__0__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Green_2__0__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Green_2__0__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Green_2__0__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Green_2__0__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Green_2__0__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Green_2__0__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Green_2__0__PC EQU CYREG_GPIO_PRT2_PC
LED_Green_2__0__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Green_2__0__PORT EQU 2
LED_Green_2__0__PS EQU CYREG_GPIO_PRT2_PS
LED_Green_2__0__SHIFT EQU 4
LED_Green_2__DR EQU CYREG_GPIO_PRT2_DR
LED_Green_2__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Green_2__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Green_2__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Green_2__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_2__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Green_2__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Green_2__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Green_2__MASK EQU 0x10
LED_Green_2__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Green_2__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Green_2__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Green_2__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Green_2__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Green_2__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Green_2__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Green_2__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Green_2__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Green_2__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Green_2__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Green_2__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Green_2__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Green_2__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Green_2__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Green_2__PC EQU CYREG_GPIO_PRT2_PC
LED_Green_2__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Green_2__PORT EQU 2
LED_Green_2__PS EQU CYREG_GPIO_PRT2_PS
LED_Green_2__SHIFT EQU 4

/* Button_Right */
Button_Right__0__DR EQU CYREG_GPIO_PRT3_DR
Button_Right__0__DR_CLR EQU CYREG_GPIO_PRT3_DR_CLR
Button_Right__0__DR_INV EQU CYREG_GPIO_PRT3_DR_INV
Button_Right__0__DR_SET EQU CYREG_GPIO_PRT3_DR_SET
Button_Right__0__HSIOM EQU CYREG_HSIOM_PORT_SEL3
Button_Right__0__HSIOM_MASK EQU 0x000000F0
Button_Right__0__HSIOM_SHIFT EQU 4
Button_Right__0__INTCFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Right__0__INTR EQU CYREG_GPIO_PRT3_INTR
Button_Right__0__INTR_CFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Right__0__INTSTAT EQU CYREG_GPIO_PRT3_INTR
Button_Right__0__MASK EQU 0x02
Button_Right__0__PA__CFG0 EQU CYREG_UDB_PA3_CFG0
Button_Right__0__PA__CFG1 EQU CYREG_UDB_PA3_CFG1
Button_Right__0__PA__CFG10 EQU CYREG_UDB_PA3_CFG10
Button_Right__0__PA__CFG11 EQU CYREG_UDB_PA3_CFG11
Button_Right__0__PA__CFG12 EQU CYREG_UDB_PA3_CFG12
Button_Right__0__PA__CFG13 EQU CYREG_UDB_PA3_CFG13
Button_Right__0__PA__CFG14 EQU CYREG_UDB_PA3_CFG14
Button_Right__0__PA__CFG2 EQU CYREG_UDB_PA3_CFG2
Button_Right__0__PA__CFG3 EQU CYREG_UDB_PA3_CFG3
Button_Right__0__PA__CFG4 EQU CYREG_UDB_PA3_CFG4
Button_Right__0__PA__CFG5 EQU CYREG_UDB_PA3_CFG5
Button_Right__0__PA__CFG6 EQU CYREG_UDB_PA3_CFG6
Button_Right__0__PA__CFG7 EQU CYREG_UDB_PA3_CFG7
Button_Right__0__PA__CFG8 EQU CYREG_UDB_PA3_CFG8
Button_Right__0__PA__CFG9 EQU CYREG_UDB_PA3_CFG9
Button_Right__0__PC EQU CYREG_GPIO_PRT3_PC
Button_Right__0__PC2 EQU CYREG_GPIO_PRT3_PC2
Button_Right__0__PORT EQU 3
Button_Right__0__PS EQU CYREG_GPIO_PRT3_PS
Button_Right__0__SHIFT EQU 1
Button_Right__DR EQU CYREG_GPIO_PRT3_DR
Button_Right__DR_CLR EQU CYREG_GPIO_PRT3_DR_CLR
Button_Right__DR_INV EQU CYREG_GPIO_PRT3_DR_INV
Button_Right__DR_SET EQU CYREG_GPIO_PRT3_DR_SET
Button_Right__INTCFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Right__INTR EQU CYREG_GPIO_PRT3_INTR
Button_Right__INTR_CFG EQU CYREG_GPIO_PRT3_INTR_CFG
Button_Right__INTSTAT EQU CYREG_GPIO_PRT3_INTR
Button_Right__MASK EQU 0x02
Button_Right__PA__CFG0 EQU CYREG_UDB_PA3_CFG0
Button_Right__PA__CFG1 EQU CYREG_UDB_PA3_CFG1
Button_Right__PA__CFG10 EQU CYREG_UDB_PA3_CFG10
Button_Right__PA__CFG11 EQU CYREG_UDB_PA3_CFG11
Button_Right__PA__CFG12 EQU CYREG_UDB_PA3_CFG12
Button_Right__PA__CFG13 EQU CYREG_UDB_PA3_CFG13
Button_Right__PA__CFG14 EQU CYREG_UDB_PA3_CFG14
Button_Right__PA__CFG2 EQU CYREG_UDB_PA3_CFG2
Button_Right__PA__CFG3 EQU CYREG_UDB_PA3_CFG3
Button_Right__PA__CFG4 EQU CYREG_UDB_PA3_CFG4
Button_Right__PA__CFG5 EQU CYREG_UDB_PA3_CFG5
Button_Right__PA__CFG6 EQU CYREG_UDB_PA3_CFG6
Button_Right__PA__CFG7 EQU CYREG_UDB_PA3_CFG7
Button_Right__PA__CFG8 EQU CYREG_UDB_PA3_CFG8
Button_Right__PA__CFG9 EQU CYREG_UDB_PA3_CFG9
Button_Right__PC EQU CYREG_GPIO_PRT3_PC
Button_Right__PC2 EQU CYREG_GPIO_PRT3_PC2
Button_Right__PORT EQU 3
Button_Right__PS EQU CYREG_GPIO_PRT3_PS
Button_Right__SHIFT EQU 1

/* Button_Right_Int */
Button_Right_Int__INTC_CLR_EN_REG EQU CYREG_CM0_ICER
Button_Right_Int__INTC_CLR_PD_REG EQU CYREG_CM0_ICPR
Button_Right_Int__INTC_MASK EQU 0x04
Button_Right_Int__INTC_NUMBER EQU 2
Button_Right_Int__INTC_PRIOR_MASK EQU 0xC00000
Button_Right_Int__INTC_PRIOR_NUM EQU 3
Button_Right_Int__INTC_PRIOR_REG EQU CYREG_CM0_IPR0
Button_Right_Int__INTC_SET_EN_REG EQU CYREG_CM0_ISER
Button_Right_Int__INTC_SET_PD_REG EQU CYREG_CM0_ISPR

/* LED_Yellow_1 */
LED_Yellow_1__0__DR EQU CYREG_GPIO_PRT2_DR
LED_Yellow_1__0__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Yellow_1__0__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Yellow_1__0__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Yellow_1__0__HSIOM EQU CYREG_HSIOM_PORT_SEL2
LED_Yellow_1__0__HSIOM_MASK EQU 0x0000F000
LED_Yellow_1__0__HSIOM_SHIFT EQU 12
LED_Yellow_1__0__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_1__0__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_1__0__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_1__0__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_1__0__MASK EQU 0x08
LED_Yellow_1__0__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Yellow_1__0__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Yellow_1__0__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Yellow_1__0__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Yellow_1__0__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Yellow_1__0__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Yellow_1__0__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Yellow_1__0__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Yellow_1__0__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Yellow_1__0__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Yellow_1__0__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Yellow_1__0__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Yellow_1__0__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Yellow_1__0__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Yellow_1__0__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Yellow_1__0__PC EQU CYREG_GPIO_PRT2_PC
LED_Yellow_1__0__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Yellow_1__0__PORT EQU 2
LED_Yellow_1__0__PS EQU CYREG_GPIO_PRT2_PS
LED_Yellow_1__0__SHIFT EQU 3
LED_Yellow_1__DR EQU CYREG_GPIO_PRT2_DR
LED_Yellow_1__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Yellow_1__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Yellow_1__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Yellow_1__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_1__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_1__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_1__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_1__MASK EQU 0x08
LED_Yellow_1__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Yellow_1__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Yellow_1__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Yellow_1__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Yellow_1__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Yellow_1__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Yellow_1__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Yellow_1__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Yellow_1__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Yellow_1__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Yellow_1__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Yellow_1__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Yellow_1__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Yellow_1__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Yellow_1__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Yellow_1__PC EQU CYREG_GPIO_PRT2_PC
LED_Yellow_1__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Yellow_1__PORT EQU 2
LED_Yellow_1__PS EQU CYREG_GPIO_PRT2_PS
LED_Yellow_1__SHIFT EQU 3

/* LED_Yellow_2 */
LED_Yellow_2__0__DR EQU CYREG_GPIO_PRT2_DR
LED_Yellow_2__0__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Yellow_2__0__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Yellow_2__0__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Yellow_2__0__HSIOM EQU CYREG_HSIOM_PORT_SEL2
LED_Yellow_2__0__HSIOM_MASK EQU 0x00000F00
LED_Yellow_2__0__HSIOM_SHIFT EQU 8
LED_Yellow_2__0__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_2__0__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_2__0__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_2__0__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_2__0__MASK EQU 0x04
LED_Yellow_2__0__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Yellow_2__0__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Yellow_2__0__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Yellow_2__0__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Yellow_2__0__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Yellow_2__0__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Yellow_2__0__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Yellow_2__0__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Yellow_2__0__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Yellow_2__0__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Yellow_2__0__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Yellow_2__0__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Yellow_2__0__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Yellow_2__0__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Yellow_2__0__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Yellow_2__0__PC EQU CYREG_GPIO_PRT2_PC
LED_Yellow_2__0__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Yellow_2__0__PORT EQU 2
LED_Yellow_2__0__PS EQU CYREG_GPIO_PRT2_PS
LED_Yellow_2__0__SHIFT EQU 2
LED_Yellow_2__DR EQU CYREG_GPIO_PRT2_DR
LED_Yellow_2__DR_CLR EQU CYREG_GPIO_PRT2_DR_CLR
LED_Yellow_2__DR_INV EQU CYREG_GPIO_PRT2_DR_INV
LED_Yellow_2__DR_SET EQU CYREG_GPIO_PRT2_DR_SET
LED_Yellow_2__INTCFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_2__INTR EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_2__INTR_CFG EQU CYREG_GPIO_PRT2_INTR_CFG
LED_Yellow_2__INTSTAT EQU CYREG_GPIO_PRT2_INTR
LED_Yellow_2__MASK EQU 0x04
LED_Yellow_2__PA__CFG0 EQU CYREG_UDB_PA2_CFG0
LED_Yellow_2__PA__CFG1 EQU CYREG_UDB_PA2_CFG1
LED_Yellow_2__PA__CFG10 EQU CYREG_UDB_PA2_CFG10
LED_Yellow_2__PA__CFG11 EQU CYREG_UDB_PA2_CFG11
LED_Yellow_2__PA__CFG12 EQU CYREG_UDB_PA2_CFG12
LED_Yellow_2__PA__CFG13 EQU CYREG_UDB_PA2_CFG13
LED_Yellow_2__PA__CFG14 EQU CYREG_UDB_PA2_CFG14
LED_Yellow_2__PA__CFG2 EQU CYREG_UDB_PA2_CFG2
LED_Yellow_2__PA__CFG3 EQU CYREG_UDB_PA2_CFG3
LED_Yellow_2__PA__CFG4 EQU CYREG_UDB_PA2_CFG4
LED_Yellow_2__PA__CFG5 EQU CYREG_UDB_PA2_CFG5
LED_Yellow_2__PA__CFG6 EQU CYREG_UDB_PA2_CFG6
LED_Yellow_2__PA__CFG7 EQU CYREG_UDB_PA2_CFG7
LED_Yellow_2__PA__CFG8 EQU CYREG_UDB_PA2_CFG8
LED_Yellow_2__PA__CFG9 EQU CYREG_UDB_PA2_CFG9
LED_Yellow_2__PC EQU CYREG_GPIO_PRT2_PC
LED_Yellow_2__PC2 EQU CYREG_GPIO_PRT2_PC2
LED_Yellow_2__PORT EQU 2
LED_Yellow_2__PS EQU CYREG_GPIO_PRT2_PS
LED_Yellow_2__SHIFT EQU 2

/* Miscellaneous */
CYDEV_BCLK__HFCLK__HZ EQU 48000000
CYDEV_BCLK__HFCLK__KHZ EQU 48000
CYDEV_BCLK__HFCLK__MHZ EQU 48
CYDEV_BCLK__SYSCLK__HZ EQU 48000000
CYDEV_BCLK__SYSCLK__KHZ EQU 48000
CYDEV_BCLK__SYSCLK__MHZ EQU 48
CYDEV_CHIP_DIE_LEOPARD EQU 1
CYDEV_CHIP_DIE_PSOC4A EQU 12
CYDEV_CHIP_DIE_PSOC5LP EQU 19
CYDEV_CHIP_DIE_PSOC5TM EQU 20
CYDEV_CHIP_DIE_TMA4 EQU 2
CYDEV_CHIP_DIE_UNKNOWN EQU 0
CYDEV_CHIP_FAMILY_FM0P EQU 4
CYDEV_CHIP_FAMILY_FM3 EQU 5
CYDEV_CHIP_FAMILY_FM4 EQU 6
CYDEV_CHIP_FAMILY_PSOC3 EQU 1
CYDEV_CHIP_FAMILY_PSOC4 EQU 2
CYDEV_CHIP_FAMILY_PSOC5 EQU 3
CYDEV_CHIP_FAMILY_UNKNOWN EQU 0
CYDEV_CHIP_FAMILY_USED EQU CYDEV_CHIP_FAMILY_PSOC4
CYDEV_CHIP_JTAG_ID EQU 0x1A1711AA
CYDEV_CHIP_MEMBER_3A EQU 1
CYDEV_CHIP_MEMBER_4A EQU 12
CYDEV_CHIP_MEMBER_4C EQU 18
CYDEV_CHIP_MEMBER_4D EQU 8
CYDEV_CHIP_MEMBER_4E EQU 4
CYDEV_CHIP_MEMBER_4F EQU 13
CYDEV_CHIP_MEMBER_4G EQU 2
CYDEV_CHIP_MEMBER_4H EQU 11
CYDEV_CHIP_MEMBER_4I EQU 17
CYDEV_CHIP_MEMBER_4J EQU 9
CYDEV_CHIP_MEMBER_4K EQU 10
CYDEV_CHIP_MEMBER_4L EQU 16
CYDEV_CHIP_MEMBER_4M EQU 15
CYDEV_CHIP_MEMBER_4N EQU 6
CYDEV_CHIP_MEMBER_4O EQU 5
CYDEV_CHIP_MEMBER_4P EQU 14
CYDEV_CHIP_MEMBER_4Q EQU 7
CYDEV_CHIP_MEMBER_4U EQU 3
CYDEV_CHIP_MEMBER_5A EQU 20
CYDEV_CHIP_MEMBER_5B EQU 19
CYDEV_CHIP_MEMBER_FM3 EQU 24
CYDEV_CHIP_MEMBER_FM4 EQU 25
CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE1 EQU 21
CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE2 EQU 22
CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE3 EQU 23
CYDEV_CHIP_MEMBER_UNKNOWN EQU 0
CYDEV_CHIP_MEMBER_USED EQU CYDEV_CHIP_MEMBER_4F
CYDEV_CHIP_DIE_EXPECT EQU CYDEV_CHIP_MEMBER_USED
CYDEV_CHIP_DIE_ACTUAL EQU CYDEV_CHIP_DIE_EXPECT
CYDEV_CHIP_REV_LEOPARD_ES1 EQU 0
CYDEV_CHIP_REV_LEOPARD_ES2 EQU 1
CYDEV_CHIP_REV_LEOPARD_ES3 EQU 3
CYDEV_CHIP_REV_LEOPARD_PRODUCTION EQU 3
CYDEV_CHIP_REV_PSOC4A_ES0 EQU 17
CYDEV_CHIP_REV_PSOC4A_PRODUCTION EQU 17
CYDEV_CHIP_REV_PSOC5LP_ES0 EQU 0
CYDEV_CHIP_REV_PSOC5LP_PRODUCTION EQU 0
CYDEV_CHIP_REV_PSOC5TM_ES0 EQU 0
CYDEV_CHIP_REV_PSOC5TM_ES1 EQU 1
CYDEV_CHIP_REV_PSOC5TM_PRODUCTION EQU 1
CYDEV_CHIP_REV_TMA4_ES EQU 17
CYDEV_CHIP_REV_TMA4_ES2 EQU 33
CYDEV_CHIP_REV_TMA4_PRODUCTION EQU 17
CYDEV_CHIP_REVISION_3A_ES1 EQU 0
CYDEV_CHIP_REVISION_3A_ES2 EQU 1
CYDEV_CHIP_REVISION_3A_ES3 EQU 3
CYDEV_CHIP_REVISION_3A_PRODUCTION EQU 3
CYDEV_CHIP_REVISION_4A_ES0 EQU 17
CYDEV_CHIP_REVISION_4A_PRODUCTION EQU 17
CYDEV_CHIP_REVISION_4C_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4D_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4E_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4F_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA EQU 0
CYDEV_CHIP_REVISION_4F_PRODUCTION_256K EQU 0
CYDEV_CHIP_REVISION_4G_ES EQU 17
CYDEV_CHIP_REVISION_4G_ES2 EQU 33
CYDEV_CHIP_REVISION_4G_PRODUCTION EQU 17
CYDEV_CHIP_REVISION_4H_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4I_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4J_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4K_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4L_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4M_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4N_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4O_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4P_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4Q_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_4U_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_5A_ES0 EQU 0
CYDEV_CHIP_REVISION_5A_ES1 EQU 1
CYDEV_CHIP_REVISION_5A_PRODUCTION EQU 1
CYDEV_CHIP_REVISION_5B_ES0 EQU 0
CYDEV_CHIP_REVISION_5B_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_FM3_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_FM4_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_PDL_FM0P_TYPE1_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_PDL_FM0P_TYPE2_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_PDL_FM0P_TYPE3_PRODUCTION EQU 0
CYDEV_CHIP_REVISION_USED EQU CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA
CYDEV_CHIP_REV_EXPECT EQU CYDEV_CHIP_REVISION_USED
CYDEV_CONFIG_READ_ACCELERATOR EQU 1
CYDEV_CONFIG_UNUSED_IO_AllowButWarn EQU 0
CYDEV_CONFIG_UNUSED_IO_AllowWithInfo EQU 1
CYDEV_CONFIG_UNUSED_IO_Disallowed EQU 2
CYDEV_CONFIG_UNUSED_IO EQU CYDEV_CONFIG_UNUSED_IO_Disallowed
CYDEV_CONFIGURATION_COMPRESSED EQU 1
CYDEV_CONFIGURATION_MODE_COMPRESSED EQU 0
CYDEV_CONFIGURATION_MODE EQU CYDEV_CONFIGURATION_MODE_COMPRESSED
CYDEV_CONFIGURATION_MODE_DMA EQU 2
CYDEV_CONFIGURATION_MODE_UNCOMPRESSED EQU 1
CYDEV_DEBUG_PROTECT_KILL EQU 4
CYDEV_DEBUG_PROTECT_OPEN EQU 1
CYDEV_DEBUG_PROTECT EQU CYDEV_DEBUG_PROTECT_OPEN
CYDEV_DEBUG_PROTECT_PROTECTED EQU 2
CYDEV_DEBUGGING_DPS_Disable EQU 3
CYDEV_DEBUGGING_DPS_SWD EQU 2
CYDEV_DEBUGGING_DPS EQU CYDEV_DEBUGGING_DPS_SWD
CYDEV_DEBUGGING_ENABLE EQU 1
CYDEV_DFT_SELECT_CLK0 EQU 10
CYDEV_DFT_SELECT_CLK1 EQU 11
CYDEV_DMA_CHANNELS_AVAILABLE EQU 8
CYDEV_HEAP_SIZE EQU 0x80
CYDEV_IMO_TRIMMED_BY_USB EQU 0
CYDEV_IMO_TRIMMED_BY_WCO EQU 0
CYDEV_INTR_NUMBER_DMA EQU 21
CYDEV_IS_EXPORTING_CODE EQU 0
CYDEV_IS_IMPORTING_CODE EQU 0
CYDEV_PROJ_TYPE EQU 0
CYDEV_PROJ_TYPE_BOOTLOADER EQU 1
CYDEV_PROJ_TYPE_LAUNCHER EQU 5
CYDEV_PROJ_TYPE_LOADABLE EQU 2
CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER EQU 4
CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER EQU 3
CYDEV_PROJ_TYPE_STANDARD EQU 0
CYDEV_STACK_SIZE EQU 0x0800
CYDEV_USE_BUNDLED_CMSIS EQU 1
CYDEV_VARIABLE_VDDA EQU 1
CYDEV_VDDA_MV EQU 3300
CYDEV_VDDD_MV EQU 3300
CYDEV_VDDR_MV EQU 3300
CYDEV_WDT_GENERATE_ISR EQU 1
CYIPBLOCK_m0s8bless_VERSION EQU 2
CYIPBLOCK_m0s8cpussv2_VERSION EQU 1
CYIPBLOCK_m0s8csd_VERSION EQU 1
CYIPBLOCK_m0s8ioss_VERSION EQU 1
CYIPBLOCK_m0s8lcd_VERSION EQU 2
CYIPBLOCK_m0s8lpcomp_VERSION EQU 2
CYIPBLOCK_m0s8peri_VERSION EQU 1
CYIPBLOCK_m0s8scb_VERSION EQU 2
CYIPBLOCK_m0s8srssv2_VERSION EQU 1
CYIPBLOCK_m0s8tcpwm_VERSION EQU 2
CYIPBLOCK_m0s8udbif_VERSION EQU 1
CYIPBLOCK_s8pass4al_VERSION EQU 1
DMA_CHANNELS_USED__MASK EQU 0
CYDEV_BOOTLOADER_ENABLE EQU 0

#endif /* INCLUDED_CYFITTERIAR_INC */