# file      BootLoader.asm
# date      2017/09/15
# author    blessnhs
#           Copyright(c)2017 All rights reserved by blessnhs
# brief     JG OS BootLoader

[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:LEVEL1

TOTALSECTORCOUNT:	 dw	0x01	;maxium boot loader size
KERNEL32SECTORCOUNT: dw 0x02    ;32bit kernel sector size
BOOTSTRAPPROCESSOR:  db 0x01    ;is bsp processor
ISGRAPHICMODE:       db 0x01  ;is graphic mode

LEVEL1:

	xor ax,ax					; ax register
    mov ax, 0x07C0  			;
    mov ds, ax      			; ds
    mov ax, 0xB800
    mov es, ax      			; ES	세그먼트 시작 주소를 0x07c0로 변경

    mov ax, 0x0000
    mov ss, ax      			; init stack 0 ~ 0xfffe(64k)
    mov sp, 0xFFFE
    mov bp, 0xFFFE

    mov byte[ BOOTDRIVE ], dl  ; 부트 드라이버 저장

	mov ax, 0x1112
    int 0x10

;	mov si,    0

.SCREENCLEARLOOP:   			;clear screen

    mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x07

    add si, 2

    cmp si, 80 * 50 * 2

    jl .SCREENCLEARLOOP
    push STARTUP_MESSAGE		; message
    push 0						; x
    push 0						; y
    call PRINTMESSAGE			; call print
    add  sp, 6

    push IMAGE_LOADING_MESSAGE
    push 1
    push 0
    call PRINTMESSAGE
    add  sp, 6

LEVEL2:


    ; 0 IS FLOPPY DISK
    mov ax, 0
    mov dl, byte [ BOOTDRIVE ]
    int 0x13
    jc  HANDLEDISKERROR



	; read disk parameter
    mov     ah, 0x08
    mov     dl, byte [ BOOTDRIVE ]		; read driver
    int     0x13
    jc      HANDLEDISKERROR

    mov     byte [ MAXHEAD ], dh        ; set max header count
    mov     al, cl                      ; sector track save info
    and     al, 0x3f					; 하위 6비트 섹터 정보

    mov     byte [ MAXSECTOR ], al      ;
    mov     byte [ MAXTRACK ], ch       ; 트랙 정보 하위 8비트

    mov si, 0x1000						; 복사할 시작 주소

    mov es, si							; 0x1000:0000(0x10000)
    mov bx, 0x0000


    mov di, word [ TOTALSECTORCOUNT ]
    ;mov di, 1146

READDATA:

    cmp di, 0							; 정해진 섹터를 다 읽었으면 종료
    je  READEND
    sub di, 0x1							; 하나 차감

    mov ah, 0x02						; 바이오스 read 함수 호출 서비스 번호 2번 read sector
    mov al, 0x1							; sector 1 읽어오기
    mov ch, byte [ TRACKNUMBER ]		; 트랙번호
    mov cl, byte [ SECTORNUMBER ]		; 섹터 번호
    mov dh, byte [ HEADNUMBER ]			; 헤드 번호
    mov dl, byte [ BOOTDRIVE ]			; 부트 번호
    int 0x13
    jc HANDLEDISKERROR

    add si, 0x0020						;512바이트 읽었으니 더함
    mov es, si							;es 세그먼트 레지스터에 반영

    mov al, byte [ SECTORNUMBER ]		;섹터 번호 저장
    add al, 0x01						;섹터 번호 증가
    mov byte [ SECTORNUMBER ], al		;증가시킨 섹터 번호 저장
    cmp al, byte [ MAXSECTOR ]			;마지막 섹터 번호 인가
    jle READDATA

    add byte [ HEADNUMBER ], 0x01		; 마지막 섹터까지 읽었다면 헤드 증가
    mov byte [ SECTORNUMBER ], 0x01

    mov al, byte [ MAXHEAD ]			;
    cmp byte [ HEADNUMBER ], al			; 마지막 헤드 번호이면 트랙 증가
    jg .ADDTRACK

    jmp READDATA

.ADDTRACK:
    mov byte [ HEADNUMBER ], 0x00		; 헤드 번호를 초기화 시키고
    add byte [ TRACKNUMBER ], 0x01		; 트랙번호를 증가 시킨다.
    jmp READDATA

READEND:

    push LOADING_COMPLETE_MESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    add  sp, 6


	;비디오 모드 정보 바이오스 함수 호출
    mov ax, 0x4F01						;비디오 모드 정보 호출 번호
    mov cx, 0x117						;1024x768 해상도 16비트 설정
    mov bx, 0x07E0
    mov es, bx
    mov di, 0x00

    int 0x10
    cmp ax, 0x004F
    jne VBEERROR

    cmp byte [ ISGRAPHICMODE ], 0x00	;비디오 모드로 시작하지 않는다면
    je JUMPTOPROTECTEDMODE				;바로 32bit 보호 모드로 시작

    mov ax, 0x4F02						;비디오 모드 전환 호출번호
    mov bx, 0x4117						;1024x768 해상도 16비트

    int 0x10
    cmp ax, 0x004F
    jne VBEERROR						;에러 발생시 이동

    jmp JUMPTOPROTECTEDMODE

VBEERROR:
    push CHANGE_GRAPHIC_MODE_FAIL
    push 2
    push 0
    call PRINTMESSAGE
    add  sp, 6
    jmp $

JUMPTOPROTECTEDMODE:
    jmp 0x1000:0x0000		;64k 위치로 이동

HANDLEDISKERROR:
    push DISK_ERROR_MESSAGE
    push 1
    push 20
    call PRINTMESSAGE

    jmp $

PRINTMESSAGE:
    push bp
    mov bp, sp

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800

    mov es, ax

    mov ax, word [ bp + 6 ]
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [ bp + 4 ]
    mov si, 2
    mul si
    add di, ax

    ; 출력할 문자열의 어드레스
    mov si, word [ bp + 8 ]

.MESSAGELOOP:
    mov cl, byte [ si ]

    cmp cl, 0
    je .MESSAGEEND

    mov byte [ es: di ], cl

    add si, 1
    add di, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

STARTUP_MESSAGE:    		db  	'JG OS Boot Loader Begin.', 0 ; 출력할 메시지 정의
DISK_ERROR_MESSAGE:       	db  	'DISK Error', 0
IMAGE_LOADING_MESSAGE:    	db  	'OS Image Loading...', 0
LOADING_COMPLETE_MESSAGE: 	db  	'OS Image Complete', 0
CHANGE_GRAPHIC_MODE_FAIL:  	db  	'Change Graphic Mode Fail', 0

SECTORNUMBER:           	db  	0x02
HEADNUMBER:             	db  	0x00
TRACKNUMBER:            	db  	0x00

BOOTDRIVE:              	db 		0x00
MAXSECTOR:             		db 		0x00
MAXHEAD:               		db 		0x00
MAXTRACK:              		db 		0x00

times 510 - ( $ - $$ )    	db    	0x00

db 0x55
db 0xAA


