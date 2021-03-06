/*
* Copyright (c) 2012, Mauro Scomparin
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Mauro Scomparin nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Mauro Scomparin ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Mauro Scomparin BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* File:         LM4F.ld.
* Author:       Mauro Scomparin <http://scompoprojects.worpress.com>.
* Version:      1.0.0.
* Description:  Linker description file for LM4FXXX microcontrollers.
*/


/*
* Memory definition:
* FLASH:    start point 0x00,       lenght 0x40000.
* SRAM:     start point 0x20000000  length 0x8000.
* STACK:    start point 0x20008000  length 0x0
*/
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

/*
* Sections definitions:
*
* .text         - machine instructions.
* .data         - initialized data defined in the program.
* .bss          - un-initialized global and static variables (to be initialized to 0 before starting main).
* .heap         - it's allocated at runtime with malloc().
* .stack        - this section it's used to signal to the C startup code the end address of ram as it should
*                 be for a c program, and I place it at the beginning of the NVIC table so it can be loaded
*                 at reset.
*/
SECTIONS
{   
    /* This section it's the code, containing the NVIC Vector table that must start at 0x0
    *   Look at the LM4F120H5QR datasheet for details. (Table 2-8. Exception Types)
    */
    .text : 
    {
        _start_text = .;    /* This is an index to the beginning of .text segment. */
        KEEP(*(.nvic_table)) /* I should keep the NVIC ISR Table because it's needed by the processor to start. */
        *(.text.*)          /* This contains the code after the ISR table. */
        *(.rodata.*)        /* Read only data.  */
        _end_text = .;      /* This is an index to the end of .text segment. */
    }>FLASH
    
    /* 
    * .data segment must be placed in RAM but it's originally stored in FLASH
    * So I set the data segment in ram, but I specify the load address with the AT
    * keyword to set that right after the .text section.
    * (Look at the LD documentation. (Optional Section Attributes)) 
    */
    .data :
    {
        _start_data = .;
        *(.data);
        _end_data = .;
    } >RAM AT >FLASH

    /* 
    * .bss contains the unitialized variables and must be set as 0x0 during runtime.
    * It should be loaded in RAM and particula care should be taken initializing them in the startup file. 
    */
    .bss : 
    {
        _start_bss = .;     /* This is an index to the beginning of .bss segment. */
        *(.bss)             /* The un-initialized data should go there.  */
        *(COMMON)           /* All the other stuff should be put there */
        _end_bss = .;       /* End index for .bss segment */
    }>RAM
    
    /* 
    * .heap contains the heap section (memory allocated at runtime).
    * Basically it's used by malloc() calls.
    * Should go to ram after bss.
    *
    .heap : 
    {
        _HEAP =.;
    }>RAM
*/    
    /* 
    * .stack it's used in the startup c code to know what to put in the first NVIC location. 
    * (It's loaded at startup on R13, (stack pointer))
    */
    .stack 0x20008000: 
    {
        _STACK_TOP = .;
    }
}