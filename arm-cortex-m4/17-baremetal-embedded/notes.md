# Baremetal Embedded

The goal is to create executable and run it on the target hardware. The target hardware is ARM Cortex-M4.

## Cross-Compilation and Toolchain

### What is cross-compilation? 

Cross-compilation is the process of compiling code on one platform and running it on another platform. In this case, we are compiling code on a host machine and running it on the target hardware (ARM Cortex-M7).

### Cross-compliation toolchain

* A collection of binaries which allows you to compile, assemble, link your application
* Also contains binaries to debug your application on the target
* Toolchain also comes with other binaries which help you to analyze the executable:
    * disset different sections of the executable
    * disaseemble
    * extract symbol and size information
    * convert executable to other formats such as bin, ihex
    * Provices 'C' standard libraries

### Popular toolchains
1. GNU Tools (GCC) for ARM Embedded Processors (free and open source)
1. armcc from ARM Ltd. (shipped with Keil, code restriction version, paid)

### Downloading

1. [GNU Tools for ARM Embedded Processors](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

## Installing

In macos, it is installed in: `/Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/bin`. Add this to the PATH.

Some of the important binaries are:

1. Compiler, linker, assembler
    *`arm-none-eabi-gcc`

1. Assembler:
    * `arm-none-eabi-as`

1. Linker:
    * `arm-none-eabi-ld`

1. Elf file analyzer
    * `arm-none-eabi-readelf`
    * `arm-none-eabi-objdump`
    * `arm-none-eabi-nm`
    

1. Format converters:
    * `arm-none-eabi-objcopy`

## Build Process

1. Preprocessing: All the preprocessor directives are resolved. `main.c` gets converted to `main.i`. All the `#include`, `#define`, `#ifdef` are resolved.
    
1. Code generation stage: Translating a source file into assembly language. Higher level language code statement will ve coverted into processor architectural level mnemonics. `main.i` gets converted to `main.s`. This is done by the compiler.

1. Assembler stage: Assembly level mnemonics are converted into opcodes (machine code for instructions). `main.s` gets converted to `main.o` (relocatable object file). This is done by the assembler.
    * The `.o` files, they dont have the final addresses of the functions. They are relocatable. They can be moved to any address in the memory.

1. Linker stage: The linker takes all the `.o` files and links them together to create the final executable. It also combines the `.o` files with the standard libraries and other libraries. The linker also resolves the addresses of the functions. `main.o` gets converted to `main.elf` (Executable and Linkable Format). This is done by the linker.
    * Then you can convert the `main.elf` to `main.bin` or `main.hex` using `arm-none-eabi-objcopy`.

All of these steps can be done using the `arm-none-eabi-gcc` command.

## Compilation and compiler flags

```
arm-none-eabi-gcc -c -mcpu=cortex-m7 -mthumb -g main.c -o main.o 
```

This command above will compile the `main.c` file and create the `main.o` file.
    * The `-c` flag tells the compiler to only compile and not link. 
    * The `-o` flag tells the compiler to output the file to `main.o`.
    * The `-mcpu=cortex-m7` flag tells the compiler that the target architecture is ARM Cortex-M7.
    * The `-mthumb` flag tells the compiler that the target architecture is ARM Thumb.
    * The `-g` flag tells the compiler to include the debug information in the executable.

Fore more details check the [GCC GNU Compiler Collection](https://gcc.gnu.org/onlinedocs/gcc-13.3.0/gcc/)

if we want to stop after the compilation stage, we can use the `-S` flag. This will stop after the assembly stage.

```
arm-none-eabi-gcc -S -mcpu=cortex-m7 -mthumb -g main.c -o main.s
```

## Analaizing relocatable object file

* main.o is in elf format (executable and linkable format)
* elf is a standard file format for object files and executables files when you use GCC
* A file format standard describes a way of organizing various elements (data, read-only data, code, uninitialized data, etc) of a program in different sections.

main.o only contains machine code file. It does not contain the final addresses of the functions. It is relocatable. It can be moved to any address in the memory.


```
// To see the sections of the main.o file
arm-none-eabi-objdump -h main.o 

// To see the disassembly of the main.o file
arm-none-eabi-objdump -D main.o

arm-none-eabi-objdump -d main.o 
```

## Code and Data of a program

Program is a collection of code and data. The code is the instructions that the processor executes. The data is the information that the program uses to execute the instructions.
* Instructions are stored in the flash memory.
* Data is stored in the RAM.
    * Why RAM not flash? Because data are just variables and they may change during the execution of the program. Flash is read-only memory.
    * But it can be stored in flash or ram. Constants can be stored in flash.

main.o (elf ) can contains:
* .data section: Contains the initialized data. This is the data that is initialized by the programmer. This is stored in the RAM.
* .bss section: Contains the uninitialized data. This is the data that is not initialized by the programmer. This is stored in the RAM.
* .text section: Contains the code. This is stored in the flash memory (ROM).
* .rodata section: Contains the read-only data (constants). This is stored in the flash memory (ROM).
* user defined sections: Contains the user defined sections which are defined by the programmer.
* some special sections added by the compiler and linker (ROM)

## Linker and Locator

`main.o` and `led.o` has its own sections. The linker combines these sections to create the final executable.

* Use the linker to merge similar sections of differenet object files and to resolve all undefined symbols of different object files.
* Locator (part of linker) takes the help of a linker script to understand how you wish to merge different sections and assigns mentioned addresses to different sections. 

Then the linker creates the final executable. Storage of the final executable is done in the flash memory.

|storage of final executable in code memory|
| --    |
| Unused Code Memory |
| .data (initialized global and static variables) |
| .rodata (read-only data) |
| .text (code) |
| Vector Table |

* Startup code needs to include the vector table. The vector table is the first thing that the processor looks at when it boots up. The vector table contains the addresses of the interrupt service routines (ISR). The vector table is stored in the flash memory.
* .data section will be recopied to the RAM during the startup code. This is because the data section is initialized by the programmer and it needs to be copied to the RAM. It is copied from flash to RAM during the startup code.
    * To copy .data to RAM, you need to know the boundaries of the .data section. We should create symbols for the start and end of the .data section. The startup code will use these symbols to copy the .data section to the RAM.
* .bss section will be initialized as well by the startup code.

## Different data of a program and related sections

* `global_un_data`: uninitialized global variable. It is stored in the .bss section. Will move to RAM during the startup code.
* `global_i_data`: initialized global variable. It is stored in .data section. should be stored in ROM and copied to RAM during the startup code.
* `global_un_s_data`: uninitialized global static variable. Since it doesnt carry any information, it is stored in the .bss section instead of .data section. 
* `global_i_s_data`: initialized global static variable. It is stored in the .data section. Will be stored in ROM and copied to RAM during the startup code. 
* `local_un_data`: uninitialized local variable. It is stored in the stack. 
* `local_i_data`: initialized local variable. It is stored in the stack.
* `local_un_s_data`: uninitialized local static variable. It is like a global private data, but it is private to the function. It is stored in the .bss section.
* `local_i_s_data`: initialized local static variable. It is like a global private data, but it is private to the function. It is stored in the .data section.

LMA: Load Memory Address -> The address where the data is stored in the flash memory.
VMA: Virtual Memory Address -> The address where the data is stored in the RAM.

## .bss vs .data

* All the unitialized global variables and uninitialized static variables are stored in the .bss section. 
* Since those variables do not have any initial values, they are not required to be stored in the .data section since .data section consumers flash space. Imagine what would happen if there is a large global uninitialized array in the program, and if that is kept in the .data section, it would unnecessarily consume flash space yet carries no useful information at all.
* .bss section does not consume any flash space unlike .data section.
* You must reserve RAM space for the .bass section by knowing its size and initiliaze those memory space to zero. This is typically done in startup code.
* Linker helps you to determine the final size of the .bss section. So, obtain the size information from a linker script symbols.
 
##  Startup file of a project 

The importance of startup files:
* the startup file is responsible for setting up the right environment for the main user code to run
* Code written in startup file runs before main(). so you can say startup file calls main().
* Some part of the startup code file is the target (Processor) dependent.
* Startup code taks care of vector table placement in code memory as required by the ARM cortex Mx processor.
* Startup code may also take care of stack reinitialization.
* Startup code is responsible of .data, .bss section initialization in main memory.

Creating vector table

1. Create an array to hold MPS and handler addresses
1. Instruct the compiler not to include the above array in .data section but in a different user defined section. Because it needs to go to the vector table section.


## Linker Script

* Linker script is a text file which explains how different sections of the object files should be merged to create the final executable.
* Linker and locator combination assigns unique absolute addresses to different sections of the output file by referring to address information mentioned in the linker script.
* Linker script also includes the code and data memory address and size information.
* Linker script are written using the GNU linked command language (ld).
* GNU linker script has the file extension of .ld
* You must supply linker script at the linking phase to the linker using -T option.

### ENTRY command

* This command is used to set the "Entry point address" informaton in the header of final elf file generated.
* In our case, "Reset handler" is the entry point
* The debugger use this information to locate the first function to execute
* Not a mandatory comand to use, but required when you debug the elf file using a debugger (GDB)
* Syntax: ENTRY(symbol_name)

```ld
ENTRY(Reset_Handler)
```

### MEMORY command

* This command allows you to describe the different memories present in the target and their start address and size information
* The linker uses information mentioned in this command to assign addresses to merged sections.
* The information is given under this command also helps the linker to calculate total code and data memory consumed so far and throw an error message if data, code, heap, or stack areas can not fit into available size
* By using memory command, you can fine-tune various memories available in your target and allow different sections to occupy different memory areas.
* Typically, one linker script has one MEMORY command.

Syntax:
```
MEMORY
{
    memory_name (attr) : ORIGIN = origin_address, LENGTH = length
    memory_name (attr) : ORIGIN = origin_address, LENGTH = length
    ...
}
```

Linker script memory [reference](https://sourceware.org/binutils/docs/ld/MEMORY.html).

For example:
| Microcontroller | Flash size (in KB) | SRAM1 size (n KB) | SRAM2 size (in KB) |
| -- | -- | -- | -- |
| STM32F4VGT6 | 1024 | 112 | 16 |

```ld
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 112K
    SRAM2 (rwx) : ORIGIN = 0x2001C000, LENGTH = 16K

    // you can combine SRAM1 and SRAM2
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}
```

### SECTIONS command

* Sections command is used to create different output sections in the final elf executable generated.
* Important command by which you can instruct the linker how to merge the input sections to yield an output section
* This command also controls the order in which different output sections appear in the eld file generated
* By using this command, you also mention the placement of a section in a memory region. For example, you instruct the linker to place the .text section in the FLASH memory region and the .data section in the SRAM memory region.

Syntax:
```
SECTIONS
{
    section_name : AT (ldadr) { contents }
    section_name : { contents }
    ...
}
```

For example:
```ld
SECTIONS
{
    /* This section should include .text section of all input files */
    .text : 
    {
        //merge all .isr_vector sections of all input files
        //merge all .text sections of all input files
        //merge all .rodata sections of all input files
    } > (vma) AT > (lma)

    /* This section should include .data section of all input files */
    .data : 
    {
        //here merge all .data sections of all input files
    } > (vma) AT > (lma)
}
```

### Location counter (.)

|storage of final executable in code memory (FLASH)|
| --    |
| Unused Code Memory |
| .data (initialized global and static variables) |
| .rodata (read-only data) |
| .text (code) |
| Vector Table |

| Data memory (SRAM) |
| --    |
| stack |
| unused sram |
| heap |
| .bss (uninitialized global and static variables) |
| .data (initialized global and static variables) |

We need to know the start and end of the .data section to copy it to the RAM. We can use the linker script to create symbols for the start and end of the .data section.

The boundary can be track using the location counter (.) in the linker script.

* This is a special linker symbol denoted by a dot (.)
* This symbol is called "location counter" because it keeps track of the current location in the output file.
* You can use this symbol inside the linker script to track and define boundaries of various sections
* You can also set location counter to any specific value while writing linker script
* Location counter should appear only inside the SECTIONS command of the linker script
* The location counter is incremented by size of the output section

### Linker script symbol

* a symbol is a name of an address
* a symbol declaration is not equivalent to a variable declaration in C. 

Symbol table in `main.o`:

| Address | Symbol Value |
| -- | -- |
| 0x08000000 | _start |
| 0x08000004 | _etext |
| 0x08000008 | my_variable |
| 0x20000000 | func1 |

In the linker script, you can create a symbol for the start and end of the .data section.

```ld
SECTIONS
{
  .text :
  {
    *(.isr_vector)    
    *(.text)          
    *(.rodata)        
    end_of_text = .;  // create a symbol for the end of the .text section
  }> FLASH 
}
```

The first time we generate the final.elf file, here is the header layout:
```
arm-none-eabi-objdump -h final.elf
```

Result:
```
final.elf 

final.elf:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         00000918  08000000  08000000  00001000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000004  24000000  08000918  00002000  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000064  24000004  0800091c  00002004  2**2
                  ALLOC
  3 .comment      00000045  00000000  00000000  00002004  2**0
                  CONTENTS, READONLY
  4 .ARM.attributes 0000002e  00000000  00000000  00002049  2**0
                  CONTENTS, READONLY
```



```
 *fill*         0x08000926        0x2 
 .rodata        0x08000928        0x9 main.o
                0x08000928                const_v_1
                0x0800092c                const_v_2
                0x08000930                const_v_3
                0x08000931                        _etext = .
```

* `fill` above is create to add padding so that the next section can start at the next 4 byte boundary.
* Since `.rodata` is the end of the `.text` section, linker wont add any padding to the `.rodata` section. Hence we need to add alignment to the `.rodata` section.

Now because we added alignment to the `.text` section, the `.data` section will start at the next 4 byte boundary. 

```ld
 *(.rodata)
 .rodata        0x08000928        0x9 main.o
                0x08000928                const_v_1
                0x0800092c                const_v_2
                0x08000930                const_v_3
                0x08000934                        . = ALIGN (0x4)
 *fill*         0x08000931        0x3 
                0x08000934                        _etext = .
```

## OpenOCD

* The open on-chip debugger aims to provide debugging, in-system programming, and boundary-scan testing for embedded target devices.

### Programming adapters

* Programming adapters are used to get access to the debug interface of the target with native protocols signaling such as SWD or JTAG since HOST doesn't have these interfaces.
* It does protocol conversion. For example, commands and messages coming from host application in the form of USB packets will be converted to equivalent debug interface signaling (SWD or JTAG) and vice versa.
* Mainly debug adapter helps you to download and debug the code

### Download code using OpenOCD

1. Download and install OpenOCD
1. Install telnet
1. Run OpenOCD with the board configuration file
1. Connect to OpenOCD using telnet
1. Issue commands over telnet or GDB client to OpenOCD to download and debug the code 