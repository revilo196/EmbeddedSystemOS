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


switchContext FUNCTION  ; start of function 

    EXPORT switchContext 
	;alten Kontext Sichern
	PUSH { r1,r0,lr }
	PUSH { r4-r7 }
	PUSH { r8-r12 }  
	
	; ALTEN STACKPOINTER im PCB SPEICHERN
	STR SP, [R0,#0x00] ;r0 addresse zu altem stackpointer
	
	LDR SP, [R1,#0x00]  ;swap stackpointer R1(ADRESSE ZU NEUEN STACKPOINTER im PCB)

   	; hole den neuen Kontext
	; vomneuen Stack 
	pop { r8-r12 }      
	pop { r4-r7 }
	pop { r1,r0,lr }  ;!! R1 und R0 halten argumente zum start eines neuen Task's
	
    BX  LR

	ENDFUNC  ; end of function

	
firstContext FUNCTION 
	EXPORT firstContext
		
	mov sp, r0
	; hole den neuen Kontext
	; vom neuen Stack 
	pop { r8-r12 }      
	pop { r4-r7 }
	pop { r1,r0,pc } ;!! R1 und R0 halten argumente zum start eines neuen Task's
	
	ENDFUNC  ; end of function
	
	END