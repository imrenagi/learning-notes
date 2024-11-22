ENTRY(Reset_Handler)


/* Highest address of the user mode stack */
_estack = ORIGIN(DTCM) + LENGTH(DTCM); /* end of Ram type memory */

/* A symbol description */
/* required amount of heap */
_Min_Heap_Size = 0x200; 

/* required amount of stack */
_Min_Stack_Size = 0x400; 

/* User flash memory bank 1 */
__FLASH_BEGIN  = 0x08000000;
/* 64KB flash */
__FLASH_SIZE   = 0x00010000;

/* 0x24000000 is the start of AXI SRAM1 */
__RAM_BEGIN    = 0x24000000;
/* 456 KB of RAM */
__RAM_SIZE     = 0x71C00;

__RAM_NONCACHEABLEBUFFER_SIZE = 0x400;

/* Memories definition */
MEMORY
{
  SRAM       (xrw) : ORIGIN = __RAM_BEGIN,    LENGTH = __RAM_SIZE
  
  /* __RAM_BEGIN + __RAM_SIZE = 0x2407_1C00 */
  /* RAM_NONCACHEABLEBUFFER = 0x2407_2000 */
  RAM_NONCACHEABLEBUFFER (xrw) : ORIGIN = __RAM_BEGIN + __RAM_SIZE,  LENGTH = __RAM_NONCACHEABLEBUFFER_SIZE

  /* 
  ITCM_RAM is in range 0x00000000 - 0x0002FFFF 
  However, 0x00010000 - 0x07FFFFFF is reserved
  */
  ITCM      (xrw) : ORIGIN = 0x00000000,    LENGTH = 0x00010000
  
  /*
  0x20000000 - 0x2002FFFF DTCM - RAM
  0x20030000 - 0x23FFFFFF Reserved  
  */
  DTCM       (rw) : ORIGIN = 0x20000000,    LENGTH = 0x00010000
  
  /*
  0x38008000 - 0x387FFFFF Reserved
  0x30004000 - 0x30007FFF SRAM2
  0x30000000 - 0x30003FFF SRAM1
  SRAMAHB combines SRAMAHB1 and SRAMAHB2
  */
  SRAMAHB   (rw)  : ORIGIN = 0x30000000,  LENGTH = 0x00008000

  /*
  0x38801000 - 0x3FFFFFFF Reserved
  0x38800000 - 0x38800FFF Backup SRAM
  */
  BKPSRAM   (rw)  : ORIGIN = 0x38800000,  LENGTH = 0x00001000

  FLASH     (xrw) : ORIGIN = __FLASH_BEGIN, LENGTH = __FLASH_SIZE
}



SECTIONS
{
  .text :
  {
    *(.isr_vector)     /* Vector table */
    *(.text)           /* consider .text sections (code) of all input */
    *(.rodata)         /* read-only data (constants) */
    . = ALIGN(4);       /* align next data on 4 byte boundry */
    _etext = .;        /* define a global symbols at end of code */

    /*
    Below we decide where we want to store this section by providing 2 addresses.
    The first address is the address where the section will be stored in the output file.
    The second address is the address where the section will be stored in the memory.

    Question: do we relocate this section to RAM or keep it in FLASH?
    Answer: We keep it in FLASH
    Hence, the first address is the address where the section will be stored in the output file.
    VMA and LMA are the same, which is __FLASH_BEGIN

    > (vma) AT> (lma)
    > FLASH AT> FLASH
    // since vma and lma is the same, we can just write:
    > FLASH
    */
  }> FLASH 

  /* 
    This data is relocatable to SRAM.
    Then load address is FLASH, but run address or the virtual memory address is SRAM.
  */
  .data :
  {
    /* location counter always track vma. in this case, it is SRAM */
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* consider all .data sections (initialized data) */
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */

  }> SRAM AT> FLASH

  .bss :
  {
    _sbss = .;         /* define a global symbol at bss start */
    *(.bss)            /* consider all .bss sections (uninitialized data) */
    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */    
    /*
    This will not get stored in FLASH, but in SRAM. It only has VMA, no LMA.
    */
  }> SRAM

  .ARM.attributes 0 : { *(.ARM.attributes) }
}