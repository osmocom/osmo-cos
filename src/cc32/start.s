/* low-level startup outines for CC32RS512 / ARM7TDMI
 * (C) 2006-2012 by Harald Welte <hwelte@hmw-consulting.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Configurable values */

.equ	ROM_Start,	0x0			/* Base address of ROM(flash) */
.equ	RAM_Start,	0x0C0000		/* Base address of RAM */
.equ	RAM_Size,	0x4000			/* For CC32RS512 */

.equ	SVC_Stack_Size,	128
.equ	IRQ_Stack_Size,	128
.equ	FIQ_Stack_Size,	128
.equ	ABT_Stack_Size,	128
.equ	SYS_Stack_Size,	256

/* Computed Values */

.equ 	FIQ_Stack,	RAM_Limit
.equ	IRQ_Stack,	FIQ_Stack-FIQ_Stack_Size
.equ	ABT_Stack,	IRQ_Stack-IRQ_Stack_Size
.equ	SVC_Stack,	ABT_Stack-ABT_Stack_Size
.equ	SYS_Stack,	SVC_Stack-SVC_Stack_Size

.equ	RAM_Limit,	RAM_Start+RAM_Size


/* Exception Vectors in RAM */

	.text
	.arm
	.section	.vectram, "ax"

#;------------------------------------------------------------------------------
#;- Section Definition
#;-----------------
#;-  Section    
#;- .internal_ram_top 	Top_Stack: used by the cstartup for vector initalisation 
#;-			management defined by ld and affect from ldscript 
#;------------------------------------------------------------------------------
	.section 	.internal_ram_top
	.code 32
	.align 	0
	.global	Top_Stack
Top_Stack:
	
/*------------------------------------------------------------------------------
*- Area Definition
*------------------------------------------------------------------------------
* .text is used instead of .section .text so it works with arm-aout too.  */
        .section 	.text.exceptions
        .text
	.global exceptions
exceptions: 
/*------------------------------------------------------------------------------
//*- Exception vectors 
//*--------------------
//*- These vectors can be read at address 0 or at RAM address
//*- They ABSOLUTELY requires to be in relative addresssing mode in order to
//*- guarantee a valid jump. For the moment, all are just looping.
//*- If an exception occurs before remap, this would result in an infinite loop.
//*- To ensure if a exeption occurs before start application to infinite loop.
//*------------------------------------------------------------------------------*/

                B	InitReset           /* 0x00 Reset handler */
undefvec:
                B	undefvec            /* 0x04 Undefined Instruction */
swivec:
                B	swivec              /* 0x08 Software Interrupt */
pabtvec:
                B	pabtvec             /* 0x0C Prefetch Abort */
dabtvec:
		b	dabtvec             /* 0x10 Data Abort */
rsvdvec:
		b	rsvdvec             /* 0x14 reserved  */
irqvec:
		b	IRQ_Handler_Entry	/* 0x18 IRQ */
fiqvec:
		b	FIQ_Handler_Entry	 /* 0x1c FIQ */

	.global IRQ_Handler_Entry
	.func   IRQ_Handler_Entry

FIQ_Handler_Entry:

		b	FIQ_Handler_Entry

/*- Save and r0 in FIQ_Register */
		/* FIXME: ISO7816 Slave driver */

		/*- Restore the Program Counter using the LR_fiq directly in the PC */
		subs        pc, lr, #4

IRQ_Handler_Entry:
		b	IRQ_Handler_Entry

		/*- Manage Exception Entry  */
		/*- Adjust and save LR_irq in IRQ stack  */
		sub         lr, lr, #4
		stmfd       sp!, {lr}

		/* FIXME */

		/*- Restore adjusted  LR_irq from IRQ stack directly in the PC */
		ldmia	sp!, {pc}^

	.size   IRQ_Handler_Entry, . - IRQ_Handler_Entry
	.endfunc
	.align 0
.RAM_TOP:
	.word	Top_Stack


	.global _startup
	.func _startup
InitReset:

.equ	MPUDisable,	0xfffffffe

	/* Disable memory protection unit */
	mrc	p15, 0, r0, c1, c0, 0
	and	r0, r0, #MPUDisable
	mcr	p15, 0, r0, c1, c0, 0

/*------------------------------------------------------------------------------
//*- Top of Stack Definition
//*-------------------------
//*- Interrupt and Supervisor Stack are located at the top of internal memory in 
//*- order to speed the exception handling context saving and restoring.
//*- ARM_MODE_SVC (Application, C) Stack is located at the top of the external memory.
//*------------------------------------------------------------------------------*/

          .EQU		ARM_MODE_FIQ,       0x11
          .EQU		ARM_MODE_IRQ,       0x12
          .EQU		ARM_MODE_SVC,       0x13
          .EQU		ARM_MODE_ABT,       0x17
          .EQU		ARM_MODE_SYS,       0x1F

          .EQU		I_BIT,              0x80
          .EQU		F_BIT,              0x40

/*------------------------------------------------------------------------------
//*- Setup the stack for each mode
//*-------------------------------*/
                ldr	r13, =FIQ_Stack
		mov	r0, r13

/*- Set up Fast Interrupt Mode and set FIQ Mode Stack*/
                msr     CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
		mov	r13, r0
		sub	r0, r0, #FIQ_Stack_Size

/*- Set up Interrupt Mode and set IRQ Mode Stack*/
                msr     CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack IRQ */
                sub     r0, r0, #IRQ_Stack_Size

/*- Set up Abort Mode and set Supervisor Mode Stack*/
                msr     CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack Sup */
                sub     r0, r0, #ABT_Stack_Size

/*- Set up Supervisor Mode and set Supervisor Mode Stack*/
                msr     CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
                mov     r13, r0                     /* Init stack Sup */
                sub     r0, r0, #SVC_Stack_Size

/*- Set up System Mode and Enable Interrupts and FIQ */
                msr     CPSR_c, #ARM_MODE_SYS
		mov	r13, r0

# Clear .bss section (Zero init)
                MOV     R0, #0
                LDR     R1, =__bss_start
                LDR     R2, =__bss_end
LoopZI:         CMP     R1, R2
                STRLO   R0, [R1], #4
                BLO     LoopZI

		/* prepare c function call to main */
		mov	r0, #0	/* argc = 0 */
		ldr	lr, =exit
		ldr	r10, =main

		bx	r10

	.size InitReset,.-InitReset
        .endfunc

/* "exit" dummy to avoid sbrk write read etc. needed by the newlib default "exit" */
        .global exit
        .func   exit
exit:
        b    .
		.size   exit, . - exit
        .endfunc


        .end

