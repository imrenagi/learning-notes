# C Learning Notes

## Const & Volatile Qualifier

### Const

#### Use case 1: Constant data
```c
uint8_t data = 5; // data can be changedz

uint8_t const data = 10; // data cant be changed by using value assignment. 
// but you still can change this by using pointer
```

Changing it with pointer
```c
uint8_t const data = 10;
// type cast might be done here since &data is uint8_t const * 
uint8_t *ptr = (uint8_t)&data;
// value should be updated
*ptr = 1;
```

> **const** doesnt mean that the value will never changed. It is there to ensure that programmers shouldn't try to mopdify the value

However, if you put it in global scope, the const will be written in ROM or FLASH memory. Hence you can't update it even by using pointer.

#### Use cae 2: Modifiable pointer and constant data
```c
uint8_t const *ptr = (uint8_t *)0x40000000;

ptr = (uint8_t *)0x50000000; //allowed

*ptr = 80; // is not allowed
```

Ptr is modifable but the data pointed by the pointer is not (read-only).

ptr is a pointer pointing to const data of type uint8_t

This usually can be used to prevent use from modifying the value pointed by the pointer. Can be used to guard a function:
```c
void copy_src_to_dst(uint8_t const *src, uint8_t *dst, uint32_t len) {
}
```
#### Use case 3: Modifiable data and constant pointer
```c
uint8_t *const ptr = (uint8_t)0x40000000;
// read: constant pointer pointing to data of type uint8_t

ptr = (uint8_t *)0x50000000; //not allowed

*ptr = 80; // allowed
```

Example:
```c
// This can be use to guard so that pointer is not updated by the function used
void update_user_data(uint8_t *const age) {
  if (age != NULL) {
    *age = getUserAge();
  }
}
```

#### Use case 4: Const data and const pointer
```c
uint8_t const *const ptr = (uint8_t)0x70000000;
// read: ptr is const pointer pointing to const data of type uint8_t

ptr = (uint8_t *)0x70000000; // not allowed
*ptr = 100; // not allowed
```
Example: 
```c
uint32_t read_status_register(uint32 const *const pStatusReg) {
    return (*pStatusReg);
}
```
This is used just to read data in a const pointer.

### Volatile

- Qualifier that is used to instruct compiler not to perform optimization on a variable
- It tells compiler that the value could be changed at anytime without programmer consent. So optimization will be turned off for that variable on the assembly level..
- This can be used when there is possibility of unexpected changes from the software or hardware.
When to used;
- Memory-mapped peripheral registers of microcontroller
- Multiple task accessing global variable(read/write) in RTOS multithreaded application
- When a global variable is used to share data between main code and ISR (Interrupt Service Routine)

#### Use case 1: Volatile data
```c
uint8_t volatile mydata;
// mydata is a volatile variable of type uint8_t
```

#### Use case 2:  Non-volatile pointer to volatile data
```c
uint8_t volatile *pData;
//pData is a non-volatile pointer pointing to volatile variable of type uint8_t


// this mean data pointed by the pointer is volatile by nature. so dont do any 
// read or write optimization on the data
```

Use this when we are trying to access memory mapped registers.

Example:
```c
uint32_t volatile *pClockControlRegister = (uint32_t *)0xFFAABBCC;
```

Example:
```c
uint8_t volatile g_button_pressed = 0;

// this not need to be volatile since it is not accessed by main thread and ISR
uint8_t g_button_counter = 1;

// ISR
void EXIT0_IRQHandler(void) {
    g_button_pressed = 1;
}
```

### Using both Const and Volatile
```c
uint8_t volatile *const pReg = (uint8_t *)0x40000000;
// pReg is a constant pointer just to guard this from being changed by the programmer. 
// but data pointing by the address can be unexpectedly changed

uint8_t const volatile *const pReg = (uint8_t *)0x40000000;
// pReg is a constant pointer to guard from being changed by the programmer. 
// The pointer is also pointing to the const volatile data. But we tell the programmer 
// not to change the data the pointer  pointed to.
// This can be used in read-only register which is still possible to get updated 
// by the hardward. 
```

Example:
```c
uint32_t volatile *const pClockControlRegister = (uint32_t *)0xFFAABBCC;
// *const is to guard the pointer

uint32_t const volatile *const pPinAInputRegister = (uint32_t *)0xFFAABBCC;
// volatile because it can still be updated by the hardware.
// const volatile to guard from programmer to change it.
```

## Structures

### Struct
```c
struct CarModel {
    uint8_t carNumber
    uint32_t carPrice
};

struct CarModel carBMW = {1, 2};
struct CarModel carBMW = {.carNumber = 1, .carPrice = 2};


sizeof(carBMW); // 4 bytes because of data alighment
sizeof(struct CarModel);

// to access the data 
carBMW.carNumber
carBMW.carPrice

//  
struct CarModel car;

uint8_t *carPtr;
carPtr = (uint8_t*)&car;
```

Struct cant use itself as the member of the struct.
```c
struct CarModel {
    uint8_t carNumber
    uint32_t carPrice
    
    struct CarModel carBMW; // this is not allowed
};
```

Struct can store pointer to itself: 
```c
struct CarModel {
    uint8_t carNumber
    uint32_t carPrice
    
    struct CarModel *carBMW; // allowed
    
    // nested structure is also allowed
    struct {
        uint8_t data;
    } moreData;
};
```

Typedef:
```c
typedef struct {
    //....
} CarModel_t;

CarModel_t carBMW, carFord;
```


Pointer:
```c
struct CarModel car;

uint8_t *carPtr = (uint8_t*)&car;

// we can change the first element if we now the length:
*carPtr = 0x55;

struct CarModel *pCar;
pData = &car;

// this is equal to setting 100 to *(address to first member element data 1)
pData->carNumber = 100;
```

### Bit fields

Instead of using bit shift and bit mask to extract bit information, we can use bit fields in a struct instead:
```c
/*
 * This structure we used to store various fields of the packet in to variables
 * The variable of this structure consumes 4 bytes in the memory
*/
struct Packet
{
        uint32_t crc               :2;
        uint32_t status            :1;
        uint32_t payload           :12;
        uint32_t bat               :3;
        uint32_t sensor            :3;
        uint32_t longAddr          :8;
        uint32_t shortAddr         :2;
        uint32_t addrMode          :1;
};

uint32_t packetValue;
scanf("%u", &packetValue);

struct Packet packet;

packet.crc       =   (uint8_t) (packetValue & 0x3);
packet.status    =   (uint8_t) ( (packetValue >> 2) & 0x1 );
packet.payload   =   (uint16_t) ( (packetValue >> 3) & 0xFFF );
packet.bat       =   (uint8_t) ( (packetValue >> 15) & 0x7 );
packet.sensor    =   (uint8_t) ( (packetValue >> 18) & 0x7 );
packet.longAddr  =   (uint8_t) ( (packetValue >> 21) & 0xFF );
packet.shortAddr =   (uint8_t) ( (packetValue >> 29) & 0x3 );
packet.addrMode  =   (uint8_t) ( (packetValue >> 31) & 0x1 );
```

### Union

- Same as struct but all of its members start from the same location in memory. 
- A union can only be used to represent only one of its members
- Only use this when the member is mutually exclusive
```c
union address {
    uint16_t shortAddr;
    uint32_t longAddr;
}

union address addr;
addr.shortAddr = 0xABCD;
addr.longAddr = 0xEEEECCCC;

printf("short addr = %#X\n",addr.shortAddr);
printf("long addr = %#X\n",addr.longAddr);

// this will print
0xCCCC   // because the longAddr overwrote the shortAddr
0xEEEECCCC
```

Using combination between union and struct
```c
union packet {
     uint32_t packetValue
     struct {
        uint32_t crc               :2;
        uint32_t status            :1;
        uint32_t payload           :12;
        uint32_t bat               :3;
        uint32_t sensor            :3;
        uint32_t longAddr          :8;
        uint32_t shortAddr         :2;
        uint32_t addrMode          :1;     
     }data;
}

union packet p;
scanf("%u", &p.packetValue);
// automatically we can access the nested struct based on the data stored in packetValue
printf("%d",p.data.crc);
```

Exercises
```c
typedef struct {
        uint32_t gpioa_en                                :1;
        uint32_t gpiob_en                                :1;
        uint32_t gpioc_en                                :1;
        uint32_t gpiod_en                                :1;
        uint32_t gpioe_en                                :1;
        uint32_t gpiof_en                                :1;
        uint32_t gpiog_en                                :1;
        uint32_t gpioh_en                                :1;
        uint32_t gpioi_en                                :1;
        uint32_t gpioj_en                                :1;
        uint32_t gpiok_en                                :1;
        uint32_t reserved1                                :8;
        uint32_t crc_en                                        :1;
        uint32_t reserved2                                :1;
        uint32_t bdma_en                                :1;
        uint32_t reserved3                                :2;
        uint32_t adc3_en                                 :1;
        uint32_t hsem_en                                 :1;
        uint32_t reserved4                                :2;
        uint32_t bkpram_en                                 :1;
        uint32_t reserved5                                :3;
}RCC_AHB4ENR_t;

typedef struct {
        uint32_t pin_0   :2;
        uint32_t pin_1   :2;
        uint32_t pin_2   :2;
        uint32_t pin_3   :2;
        uint32_t pin_4   :2;
        uint32_t pin_5   :2;
        uint32_t pin_6   :2;
        uint32_t pin_7   :2;
        uint32_t pin_8   :2;
        uint32_t pin_9   :2;
        uint32_t pin_10  :2;
        uint32_t pin_11  :2;
        uint32_t pin_12  :2;
        uint32_t pin_13  :2;
        uint32_t pin_14  :2;
        uint32_t pin_15  :2;
}GPIOx_MODER_t;

typedef struct {
        uint32_t pin_0   :1;
        uint32_t pin_1   :1;
        uint32_t pin_2   :1;
        uint32_t pin_3   :1;
        uint32_t pin_4   :1;
        uint32_t pin_5   :1;
        uint32_t pin_6   :1;
        uint32_t pin_7   :1;
        uint32_t pin_8   :1;
        uint32_t pin_9   :1;
        uint32_t pin_10  :1;
        uint32_t pin_11  :1;
        uint32_t pin_12  :1;
        uint32_t pin_13  :1;
        uint32_t pin_14  :1;
        uint32_t pin_15  :1;
        uint32_t reserved: 16;
}GPIOx_ODR_t;

typedef struct {
        uint32_t pin_0   :1;
        uint32_t pin_1   :1;
        uint32_t pin_2   :1;
        uint32_t pin_3   :1;
        uint32_t pin_4   :1;
        uint32_t pin_5   :1;
        uint32_t pin_6   :1;
        uint32_t pin_7   :1;
        uint32_t pin_8   :1;
        uint32_t pin_9   :1;
        uint32_t pin_10  :1;
        uint32_t pin_11  :1;
        uint32_t pin_12  :1;
        uint32_t pin_13  :1;
        uint32_t pin_14  :1;
        uint32_t pin_15  :1;
        uint32_t reserved: 16;
}GPIOx_IDR_t;

typedef struct {
        uint32_t pin_0   :2;
        uint32_t pin_1   :2;
        uint32_t pin_2   :2;
        uint32_t pin_3   :2;
        uint32_t pin_4   :2;
        uint32_t pin_5   :2;
        uint32_t pin_6   :2;
        uint32_t pin_7   :2;
        uint32_t pin_8   :2;
        uint32_t pin_9   :2;
        uint32_t pin_10  :2;
        uint32_t pin_11  :2;
        uint32_t pin_12  :2;
        uint32_t pin_13  :2;
        uint32_t pin_14  :2;
        uint32_t pin_15  :2;
}GPIOx_PUPDR_t;

int main(void) {
        RCC_AHB4ENR_t volatile *const ptr_clock_register = (RCC_AHB4ENR_t*)(0x58024400 + 0x140);
        GPIOx_MODER_t volatile *const ptr_gpioi_mode_register = (GPIOx_MODER_t*)0x58022000;
        GPIOx_ODR_t volatile *const ptr_gpioi_out_data_register = (GPIOx_ODR_t*)(0x58022000 + 0x14);

        GPIOx_MODER_t volatile *const ptr_gpiok_mode_register = (GPIOx_MODER_t*)0x58022800;
        GPIOx_PUPDR_t volatile *const ptr_gpiok_pull_register = (GPIOx_PUPDR_t*)(0x58022800 + 0x0C);
        GPIOx_IDR_t const volatile *const ptr_gpiok_in_data_register = (GPIOx_IDR_t*)(0x58022800 + 0x10);

        //enabling gpio clock
        ptr_clock_register->gpioi_en = 1;
        ptr_clock_register->gpiok_en = 1;

        //set gpio mode
        ptr_gpioi_mode_register->pin_15 = 1; //output mode
        ptr_gpiok_mode_register->pin_2 = 0;

        // set pull-up resistor
        ptr_gpiok_pull_register->pin_2 = 1; //input mode

        while(1) {
                // test bit
                if (ptr_gpiok_in_data_register->pin_2 == 1) {
                        ptr_gpioi_out_data_register->pin_15 = 1;
                } else {
                        ptr_gpioi_out_data_register->pin_15 = 0;
                }
        };
}
```

## String

```c
// string variable
// this string will get copied from the flash to the stack/RAM
char msg1[] = "Hello world"

// string literal
char *msg = "hello world"
// the address pointed by the pointer is actually address in the flash.
// thus, this cant be changed.
```
Thus, to guard the string literal, we can use const:
```c
char const *msg = "hello world";
```

### Inputting String
```c
char name[30];
// the name input will store to the name until it found \n character
scanf("%[^\n]s", &name);
```

## Preprocessor Directive

* Macros --> #define <identifier> <value>

  * Macro is just text replacement. 
    ```c
    #define PIN_8 8
    #define LED_8 PIN_8
    #define FLASH_BASE_ADDRESS 0x80000000UL
    ```
  * Function like macros.
    - Need to be careful when using macro with multiple operand.
    - Use parentheses generously to prevent issue with the operations

    ```c
    #define PI (3.14f)
    #define AREA_OF_CIRCLE(r) (PI * (r) * (r))

    #define ITM_Port32(n) (*((volatile unsigned long *)(0xE0000000+(4*n))))
    ```

* File inclusion:

    ```c
    #include <stdint.h>
    #include "imre.h" // for custom header file, use quotes
    ```

* Conditional compilation

    * ifndef
        ```c
        #ifndef __MAIN_H_
        #define __MAIN_H_

        #endif
        ```
    * Ifdef -> checks whether the identifier is defined in the program or not. if it is defined, then include the code block.
        ```c
        // test.h
        #ifdef NEW_FEATURE
        //code block
        #endif

        // main.c
        #define NEW_FEATURE

        #include "test.h"
        ```

    *  The `NEW_FEATURE` can be defined by the approach above or injected during compilation using -D option.

    *  defined -> used when we want to check the definition of multiple macros
        ```c
        #if defined(NEW_FEATURE) && define(ANOTHER_FLAG) 

        #endif
        ```

Example:

```c
#include <stdio.h>

// You might need to define these explicitly if not automatically defined.
// For example, in your build system, or by using compiler flags:
// - For Windows: `-D WIN32`
// - For Linux: `-D __linux__`

#ifdef _WIN32 // or _WIN64, depending on which windows target you want to check
    #include <windows.h>
    // Windows-specific code here
#elif defined(__linux__)
    #include <unistd.h>
    // Linux-specific code here
#else
    #error "Unsupported platform"
#endif

int main() {
#ifdef _WIN32
    printf("This is Windows.\n");
    // More Windows-specific code
#elif defined(__linux__)
    printf("This is Linux.\n");
    // More Linux-specific code
#endif

    return 0;
}
```

* Others

    *  Using error will cause compilation error.
    ```c
    #if !defined(NEW_FEATURE) && !define(ANOTHER_FLAG) 
        error "no macro defined"
    #endif
    ```

    * Using warning will show warning on compilation
    ```c
    #error --> stop the program
    #pragma --> just warning
    ```
