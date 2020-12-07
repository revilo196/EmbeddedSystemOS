; ////////////////////////////////////
; // SOURCE FILE   ebsy_os.s          //
; ////////////////////////////////////
  
;/**************************************************************************
; *  Eingebettete Betriebssystem    (EBSy)                                 *                                                                                                  *
; **************************************************************************
; *  PROJECT       Project1                                                *
; *  MODULE        ebsy_os.s                                               *
; *  REVISION      1.0                                                     *
; *  LAST AUTHOR                                                           *
; *  LAST MODIFIED                                                         *
; *  RESPONSIBLE   DLINDLEIN OWALTER                                       *
; **************************************************************************
; *  PURPOSE:                                                              *
; *     arg - R0 next stack pointer                                        *
; *     ret - R0 last stack pointer                                        *
; **************************************************************************
; *  REMARKS:                                                              *
; *   <none>                                                               *
; *                                                                        *
; **************************************************************************
; *  CHANGE HISTORY:                                                       *
; *  Revision   Date          Author      Description                      *
; *                                                                        *
; *                                                                        *
; *************************************************************************/
  

  AREA  DELAY, CODE, READONLY    

  THUMB

  EXTERN current_task
  EXTERN next_task

PendSV_Handler PROC
	LDR   R3, =current_task
	MRS   R0, PSP
	ADD   R0, #4 ; address um 4 nach oben, da der P-SP auf die erste freie adresse zeigt
	STMDB R0, {R4-R11}
	STR   R0, [R3]  ; den gepuscheten P-SP wieder speichen

	LDR   R3, =next_task
	LDR   R0, [R3];   ;r0 sollte jetzt die adresse des neue stacks beinahalten
	LDMIA R0, {R4-R11}
	SUB   R0, #4 ; wieder auf auf freien speicher zeigen
	MRS   PSP, R0

	LDR R0, =0xFFFFFFFD  ; mit dem NVIC in PSP mode springen
	BX R0

	ENDP


firstContext FUNCTION 
	EXPORT firstContext
	SUB R0, #4;  wieder auf auf freien speicher zeigen
	MRS PSP, r0
	
	LDR R0, =0x03
	MRC R0

	; hole den neuen Kontext
	; vom neuen Stack 
	pop { r4-r11 }      
	pop { r0-r3 }
	pop { r12,r2,r3,xPSR };
	
	MOV LR,  r2
	BX R3
	
	ENDFUNC  ; end of function
	
	END