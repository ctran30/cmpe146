/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void convertToBankAndSector (uint32_t memoryLocation);
uint_fast8_t bankSpace;
uint32_t secMask;

/* 3 chars */
char* string1 = "12345";
char string2[] = "12345";
const char string3[1024 * 150] = {"12345"};

void main(void){

    /* Addresses of strings */
//    printf("%x\n", string1);
//    printf("%x\n", &string2);
//    printf("%x\n", &string3);

    /* Print out contents of all strings */
    printf("Before modifications:\n");
    printf("string 1: %s\n", string1);
    printf("string 2: %s\n", string2);
    printf("string 3: %s\n", string3);

    /* Modify string2 */
    string2[0] = '0';
    string2[1] = '4';

    // unprotects string1
    convertToBankAndSector(0x259e8);
    ROM_FlashCtl_unprotectSector(bankSpace, secMask);

    // unprotects string3
    convertToBankAndSector(0xe4);
    ROM_FlashCtl_unprotectSector(bankSpace, secMask);

    /* Duplicate first two bytes of string2 into string1 and string3 */
    ROM_FlashCtl_programMemory(&string2, string1, 2);
    ROM_FlashCtl_programMemory(&string2, &string3, 2);

    /* Print out changes contents of all strings */
    printf("After modifications:\n");
    printf("string 1: %s\n", string1);
    printf("string 2: %s\n", string2);
    printf("string 3: %s\n", string3);
}

void convertToBankAndSector (unsigned int memoryLocation) {
    unsigned int mainMemLocation;
    if (memoryLocation > 0x20000) {
        mainMemLocation = 0x20000;
        bankSpace = FLASH_MAIN_MEMORY_SPACE_BANK1;
    } else if (memoryLocation < 0x20000) {
        mainMemLocation = 0x0;
        bankSpace = FLASH_MAIN_MEMORY_SPACE_BANK0;
    }
    secMask = ((mainMemLocation - memoryLocation)/4096);
}
