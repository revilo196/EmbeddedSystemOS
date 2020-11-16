  
; ////////////////////////////////////
; // SOURCE FILE   delay.s          //
; ////////////////////////////////////
  
;/**************************************************************************
; *  Eingebettete Betriebssystem    (EBSy)                                 *                                                                                                  *
; **************************************************************************
; *  PROJECT       Project1                                                *
; *  MODULE        delay.s                                                 *
; *  REVISION      1.0                                                     *
; *  LAST AUTHOR                                                           *
; *  LAST MODIFIED                                                         *
; *  RESPONSIBLE   Peter Raab                                              *
; **************************************************************************
; *  PURPOSE:                                                              *
; *   Delay of CPU execution; implements an exact time deley of R0 * 1ms   *
; *                                                                        *
; **************************************************************************
; *  REMARKS:                                                              *
; *   <none>                                                               *
; *                                                                        *
; **************************************************************************
; *  CHANGE HISTORY:                                                       *
; *  Revision   Date          Author      Description                      *
; *     1.0      06.09.2019    P. Raab     creation                        *
; *                                                                        *
; *************************************************************************/
  
  
  AREA  DELAY, CODE, READONLY    

  THUMB


delayms FUNCTION        ; start of function 
    EXPORT delayms 
    PUSH {R5}           ; store used registers

_D1  MOV R5, #6666		; -> 1ms

_D2  NOP				; No Operation
     NOP
     NOP
     SUBS R5,R5,#1		; R5--
     BNE _D2            ; R5 != 0?, go on
     NOP
	 NOP

    SUBS R0,R0,#1		; R0--
    BNE _D1			    ; R0 != 0?, go on
	NOP
	NOP


    POP {R5}

    BX  LR

    ENDFUNC  ; end of function
    END
	