#ifndef PREFERENCES_H
#define	PREFERENCES_H


//---------------------------------------------------------------------

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define DESC_BANK1_WRITE_START_ADDR  ((uint32_t)0x08020400)
#define DESC_BANK1_WRITE_END_ADDR    ((uint32_t)0x08020800)


#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08020000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x08020400)

#ifdef STM32F10X_XL
  #define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
  #define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */
#endif
