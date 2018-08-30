# file		ISR.asm
# date      2009/01/24
# author	kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief		인터럽트 서비스 루틴(ISR) 관련된 소스 파일

[BITS 64]           ; 이하의 코드는 64비트 코드로 설정

SECTION .text       ; text 섹션(세그먼트)을 정의

; 외부에서 정의된 함수를 쓸 수 있도록 선언함(Import)
extern CommonExceptionHandler, CommonInterruptHandler, KeyboardHandler
extern TimerHandler, DeviceNotAvailableHandler, HDDHandler, MouseHandler

; C 언어에서 호출할 수 있도록 이름을 노출함(Export)
; 예외(Exception) 처리를 위한 ISR
global ISRDivideError, ISRDebug, ISRNMI, ISRBreakPoint, ISROverflow
global ISRBoundRangeExceeded, ISRInvalidOpcode, ISRDeviceNotAvailable, ISRDoubleFault,
global ISRCoprocessorSegmentOverrun, ISRInvalidTSS, ISRSegmentNotPresent
global ISRStackSegmentFault, ISRGeneralProtection, ISRPageFault, ISR15
global ISRFPUError, ISRAlignmentCheck, ISRMachineCheck, ISRSIMDError, ISRETCException

; 인터럽트(Interrupt) 처리를 위한 ISR
global ISRTimer, ISRKeyboard, ISRSlavePIC, ISRSerial2, ISRSerial1, ISRParallel2
global ISRFloppy, ISRParallel1, ISRRTC, ISRReserved, ISRNotUsed1, ISRNotUsed2
global ISRMouse, ISRCoprocessor, ISRHDD1, ISRHDD2, ISRETCInterrupt

; 콘텍스트를 저장하고 셀렉터를 교체하는 매크로
%macro SAVECONTEXT 0       ; 파라미터를 전달받지 않는 SAVECONTEXT 매크로 정의
    ; RBP 레지스터부터 GS 세그먼트 셀렉터까지 모두 스택에 삽입
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov ax, ds      ; DS 세그먼트 셀렉터와 ES 세그먼트 셀렉터는 스택에 직접
    push rax        ; 삽입할 수 없으므로, RAX 레지스터에 저장한 후 스택에 삽입
    mov ax, es
    push rax
    push fs
    push gs	

    ; 세그먼트 셀렉터 교체
    mov ax, 0x10    ; AX 레지스터에 커널 데이터 세그먼트 디스크립터 저장
    mov ds, ax      ; DS 세그먼트 셀렉터부터 FS 세그먼트 셀렉터까지 모두
    mov es, ax      ; 커널 데이터 세그먼트로 교체
   	mov gs, ax
   	mov fs, ax
%endmacro           ; 매크로 끝


; 콘텍스트를 복원하는 매크로
%macro LOADCONTEXT 0   ; 파라미터를 전달받지 않는 SAVECONTEXT 매크로 정의
    ; GS 세그먼트 셀렉터부터 RBP 레지스터까지 모두 스택에서 꺼내 복원
    pop gs
    pop fs
    pop rax
    mov es, ax      ; ES 세그먼트 셀렉터와 DS 세그먼트 셀렉터는 스택에서 직접
    pop rax         ; 꺼내 복원할 수 없으므로, RAX 레지스터에 저장한 뒤에 복원
    mov ds, ax
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp        
%endmacro       ; 매크로 끝

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	예외 핸들러
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; #0, Divide Error ISR
ISRDivideError:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 0
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #1, Debug ISR
ISRDebug:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 1
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #2, NMI ISR
ISRNMI:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 2
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #3, BreakPoint ISR
ISRBreakPoint:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 3
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #4, Overflow ISR
ISROverflow:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 4
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #5, Bound Range Exceeded ISR
ISRBoundRangeExceeded:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 5
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #6, Invalid Opcode ISR
ISRInvalidOpcode:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 6
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #7, Device Not Available ISR
ISRDeviceNotAvailable:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 7
    call DeviceNotAvailableHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #8, Double Fault ISR
ISRDoubleFault:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호와 에러 코드를 삽입하고 핸들러 호출
    mov rdi, 8
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #9, Coprocessor Segment Overrun ISR
ISRCoprocessorSegmentOverrun:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 9
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #10, Invalid TSS ISR
ISRInvalidTSS:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호와 에러 코드를 삽입하고 핸들러 호출
    mov rdi, 10
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #11, Segment Not Present ISR
ISRSegmentNotPresent:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호와 에러 코드를 삽입하고 핸들러 호출
    mov rdi, 11
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #12, Stack Segment Fault ISR
ISRStackSegmentFault:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호와 에러 코드를 삽입하고 핸들러 호출
    mov rdi, 12
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #13, General Protection ISR
ISRGeneralProtection:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호와 에러 코드를 삽입하고 핸들러 호출
    mov rdi, 13
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거    
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #14, Page Fault ISR
ISRPageFault:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호와 에러 코드를 삽입하고 핸들러 호출
    mov rdi, 14
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #15, Reserved ISR
ISR15:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 15
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #16, FPU Error ISR
ISRFPUError:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 16
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #17, Alignment Check ISR
ISRAlignmentCheck:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 17
    mov rsi, qword [ rbp + 8 ]
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    add rsp, 8      ; 에러 코드를 스택에서 제거
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #18, Machine Check ISR
ISRMachineCheck:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 18
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #19, SIMD Floating Point Exception ISR
ISRSIMDError:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 19
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #20~#31, Reserved ISR
ISRETCException:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 예외 번호를 삽입하고 핸들러 호출
    mov rdi, 20
    call CommonExceptionHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	인터럽트 핸들러
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; #32, 타이머 ISR
ISRTimer:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 32    
    call TimerHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #33, 키보드 ISR
ISRKeyboard:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 33
    call KeyboardHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원

; #34, 슬레이브 PIC ISR
ISRSlavePIC:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 34
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #35, 시리얼 포트 2 ISR
ISRSerial2:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 35
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #36, 시리얼 포트 1 ISR
ISRSerial1:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 36
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #37, 패러렐 포트 2 ISR
ISRParallel2:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 37
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #38, 플로피 디스크 컨트롤러 ISR
ISRFloppy:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 38
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #39, 패러렐 포트 1 ISR
ISRParallel1:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 39
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #40, RTC ISR
ISRRTC:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 40
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #41, 예약된 인터럽트의 ISR
ISRReserved:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 41
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #42, 사용 안함
ISRNotUsed1:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 42
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #43, 사용 안함
ISRNotUsed2:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 43
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #44, 마우스 ISR
ISRMouse:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 44
    call MouseHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #45, 코프로세서 ISR
ISRCoprocessor:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 45
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #46, 하드 디스크 1 ISR
ISRHDD1:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 46
    call HDDHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
    
; #47, 하드 디스크 2 ISR
ISRHDD2:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 47
    call HDDHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
      
; #48 이외의 모든 인터럽트에 대한 ISR              
ISRETCInterrupt:
    SAVECONTEXT    ; 콘텍스트를 저장한 뒤 셀렉터를 커널 데이터 디스크립터로 교체
    
    ; 핸들러에 인터럽트 번호를 삽입하고 핸들러 호출
    mov rdi, 48
    call CommonInterruptHandler
    
    LOADCONTEXT    ; 콘텍스트를 복원
    iretq           ; 인터럽트 처리를 완료하고 이전에 수행하던 코드로 복원
