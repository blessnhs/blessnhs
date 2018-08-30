
# 기본적으로 빌드를 수행할 목록
all: BootLoader Kernel32 Kernel64 ImageMaker Disk.img 


ImageMaker:

	make -c ThirdParty/ImageMaker

# 부트 로더 빌드를 위해 부트 로더 디렉터리에서 make 실행
BootLoader:
	@echo 
	@echo ============== Build Boot Loader ===============
	@echo 
	
	make -C Boot

	@echo 
	@echo =============== Build Complete ===============
	@echo 
	
# 보호 모드 커널 이미지를 빌드하기 위해 보호 모드 디렉터리에서 make 실행
Kernel32:
	@echo 
	@echo ============== Build 32bit Kernel ===============
	@echo 
	
	make -C Kernel/32bit

	@echo 
	@echo =============== Build Complete ===============
	@echo 

# IA-32e 모드 커널 이미지를 빌드하기 위해 IA-32e 모드 디렉터리에서 make 실행
Kernel64:
	@echo 
	@echo ============== Build 64bit Kernel ===============
	@echo 
	
	make -C Kernel/64bit

	@echo 
	@echo =============== Build Complete ===============
	@echo 

	
# OS 이미지 생성
Disk.img: Boot/BootLoader.bin Kernel/32bit/Kernel32.bin Kernel/64bit/Kernel64.bin
	@echo 
	@echo =========== Disk Image Build Start ===========
	@echo 

	./ImageMaker.exe $^

	@echo 
	@echo ============= All Build Complete =============
	@echo 
	
# 소스 파일을 제외한 나머지 파일 정리	
clean:
	make -C BootLoader clean
	make -C Kernel32 clean
	make -C Kernel64 clean
	make -C Application clean
	make -C Utility clean
	rm -f Disk.img	