# file		makefile
# brief		IA-32e 모드 커널을 빌드하기 위한 make 파일

################################################################################
# 빌드 환경 및 규칙 설정 
################################################################################
# 컴파일러 및 링커 정의
NASM64 = nasm -f elf64
GCC64 =  x86_64-elf-gcc.exe -c -m64 -ffreestanding
LD64 = x86_64-elf-ld.exe -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x200000

# 바이너리 이미지 생성을 위한 OBJCOPY 옵션 정의
OBJCOPY64 = x86_64-elf-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

# 디렉터리 정의
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY	= Source
USB_SOURCEDIRECTORY	= Source/USB
AUDIO_SOURCEDIRECTORY	= Source/AUDIO
HID_SOURCEDIRECTORY	= Source/HID
STORAGE_SOURCEDIRECTORY	= Source/STORAGE
FATEXT_SOURCEDIRECTORY	= Source/FATEXT
NETWORK_SOURCEDIRECTORY	= Source/NETWORK
NETPROTOCOL_SOURCEDIRECTORY	= Source/NETPROTOCOL
CDI_SOURCEDIRECTORY	= Source/CDI
FAT32_SOURCEDIRECTORY	= Source/FAT32
SATA_SOURCEDIRECTORY	= Source/SATA
MODULE_SOURCEDIRECTORY	= Source/MODULE

################################################################################
# 빌드 항목 및 빌드 방법 설정
################################################################################
# 기본적으로 빌드를 수행할 목록
all: prepare Kernel64.bin

# 오브젝트 파일이 위치할 디렉터리를 생성
prepare:
	mkdir -p $(OBJECTDIRECTORY)


# 커널의 C 소스 파일에 대한 의존성 정보 생성
dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependancy Search Complete ===

# 디렉터리를 오브젝트 파일 디렉터리로 이동해서 의존성 파일 및 실행 파일을 생성
ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel64.elf

# 커널 이미지를 바이너리 파일로 변환하여 IA-32e 모드 바이너리 생성
Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIRECTORY)/Kernel64.elf $@
		
# 소스 파일을 제외한 나머지 파일 정리	
clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

################################################################################
# Make에 의해 다시 호출되는 부분, Temp 디렉터리를 기준으로 수행됨
################################################################################
# 빌드할 어셈블리어 엔트리 포인트 소스 파일 정의, Temp 디렉터리를 기준으로 설정
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIRECTORY)/EntryPoint.s
ENTRYPOINTOBJECTFILE = EntryPoint.o
# 빌드할 C 소스 파일 정의, Temp 디렉터리를 기준으로 설정
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c ../$(SOURCEDIRECTORY)/*/*.c ../$(SOURCEDIRECTORY)/*/*/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES =  $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES)))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

# 어셈블리어 엔트리 포인트 빌드
$(ENTRYPOINTOBJECTFILE): $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -o $@ $<



# .c 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<
	
%.o: ../$(USB_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	
	
%.o: ../$(AUDIO_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	
	
%.o: ../$(HID_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	

%.o: ../$(STORAGE_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	

%.o: ../$(FATEXT_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<		
	
%.o: ../$(NETWORK_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	
		
%.o: ../$(NETPROTOCOL_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<		
	
%.o: ../$(CDI_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<		
	
%.o: ../$(FAT32_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	
	
%.o: ../$(SATA_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	

%.o: ../$(MODULE_SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<	

# .asm 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM64) -o $@ $<

# 실제 의존성에 관련된 파일을 생성
InternalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > Dependency.dep

# 실제 커널 이미지를 빌드
Kernel64.elf: $(ENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD64) -o $@ $^

# 현재 디릭토리의 파일 중, dependency 파일이 있으면 make에 포함	
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
