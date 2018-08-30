
[BITS 32]               ; 이하의 코드는 32비트 코드로 설정

global ReadCPUID, Change64BitKernel

SECTION .text       ; text 섹션(세그먼트)을 정의

ReadCPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi
    
    ;1첫번 파라미터를 eax 인자로 받고 cpuid 명령어 호출
    mov eax, dword [ ebp + 8 ]
    cpuid
    
    ;2번째 파라미터를 esi에 넣고 포인터임
    ;실제 포인터 주소에 값으삽입
    mov esi, dword [ ebp + 12 ]
    mov dword [ esi ], eax

    mov esi, dword [ ebp + 16 ]
    mov dword [ esi ], ebx


    mov esi, dword [ ebp + 20 ]
    mov dword [ esi ], ecx


    mov esi, dword [ ebp + 24 ]
    mov dword [ esi ], edx
                                

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret
    
Change64BitKernel:

    mov eax, cr4		;CR4 컨트롤 레지스터의 값을 EAX에 저장
    or eax, 0x620   	;PAE비트를 1로 설정
    mov cr4, eax    	;PAE비트가 1로 설정된 값을 저장
    
    mov eax, 0x100000	;PM4테이블 주소값을 CR3 레지스터에 설정
    mov cr3, eax
    
    mov ecx, 0xC0000080 ;IA32_EFFER MSR 레지스터 어드레스 저장
    rdmsr
        
    or eax, 0x0101		;EAX레지스터에 저장된 MSR의 하위 32미트에서 LME 비트 1로 설정
    wrmsr

    mov eax, cr0		;CR0 레지스터 저장
    or eax, 0xE000000E

    xor eax, 0x60000004

    mov cr0, eax
    
    jmp 0x08:0x200000	;64비트 세그먼트 셀렉터로 교체

