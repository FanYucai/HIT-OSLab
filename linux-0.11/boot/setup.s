!
! SYS_SIZE is the number of clicks (16 bytes) to be loaded.
! 0x3000 is 0x30000 bytes = 196kB, more than enough for current
! versions of linux
!
SYSSIZE = 0x3000
!
!	bootsect.s		(C) 1991 Linus Torvalds
!
! bootsect.s is loaded at 0x7c00 by the bios-startup routines, and moves
! iself out of the way to address 0x90000, and jumps there.
!
! It then loads 'setup' directly after itself (0x90200), and the system
! at 0x10000, using BIOS interrupts. 
!
! NOTE! currently system is at most 8*65536 bytes long. This should be no
! problem, even in the future. I want to keep it simple. This 512 kB
! kernel size should be enough, especially as this doesn't contain the
! buffer cache as in minix
!
! The loader has been made as simple as possible, and continuos
! read errors will result in a unbreakable loop. Reboot by hand. It
! loads pretty fast by getting whole sectors at a time whenever possible.

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

SETUPLEN = 2				! nr of setup-sectors
BOOTSEG  = 0x07c0			! original address of boot-sector
INITSEG  = 0x9000			! we move boot here - out of the way
SETUPSEG = 0x9020			! setup starts here
SYSSEG   = 0x1000			! system loaded at 0x10000 (65536).
ENDSEG   = SYSSEG + SYSSIZE		! where to stop loading

! ROOT_DEV:	0x000 - same type of floppy as boot.
!		0x301 - first partition on first drive etc
ROOT_DEV = 0x306

entry _start
_start:
! Print some inane message
	mov 	ax,#SETUPSEG
	mov 	es,ax
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#32
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10



.org 508
root_dev:
	.word ROOT_DEV
boot_flag:
	.word 0xAA55

! ok, the read went well so we get current cursor position and save it for
! posterity.

	mov	ax,#INITSEG	! this is done in bootsect already, but...
	mov	ds,ax
	mov	ah,#0x03	! read cursor pos
	xor	bh,bh
	int	0x10		! save it in known place, con_init fetches
	mov	[0],dx		! it from 0x90000.
! Get memory size (extended mem, kB)

	mov	ah,#0x88
	int	0x15
	mov	[2],ax

! Get video-card data:

	mov	ah,#0x0f
	int	0x10
	mov	[4],bx		! bh = display page
	mov	[6],ax		! al = video mode, ah = window width

! check for EGA/VGA and some config parameters

	mov	ah,#0x12
	mov	bl,#0x10
	int	0x10
	mov	[8],ax
	mov	[10],bx
	mov	[12],cx

! Get hd0 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0080
	mov	cx,#0x10
	rep
	movsb

! Get hd1 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x46]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0090
	mov	cx,#0x10
	rep
	movsb

! print some datas about the hardware:  
! print the cursor position  
    
	mov 	ax,#SETUPSEG
	mov 	es,ax
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#13
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg2
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

              MOV       AX,#INITSEG  
              MOV       DS,AX   !SET THE DATA SEG ADDRESS, EVEN IT HAS BEEN SET.  
                    
              MOV       CX,#4  
              MOV       DX,[0]  
     PRINT_DIGIT1:             !print the data  
              ROL       DX,#4  
              MOV       AX,#0XE0F  
              AND       AL,DL  
              ADD       AL,#0X30  
              CMP       AL,#0X3A  
              JL        OUTP1  
              ADD       AL, #0X07  
       OUTP1:  
              INT       0X10  
              LOOP      PRINT_DIGIT1  
!print size of the extended memory:  

 	mov 	ax,#SETUPSEG
	mov 	es,ax
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#14
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg3
	mov	ax,#0x1301		! write string, move cursor
	int	0x10
                
              INT       0X10  
	      MOV       AX,#INITSEG  
              MOV       DS,AX   !SET THE DATA SEG ADDRESS, EVEN IT HAS BEEN SET.  
                    
              MOV       CX,#4  
              MOV       DX,[2]  
            PRINT_DIGIT2:  
              ROL       DX,#4  
              MOV       AX,#0XE0F  
              AND       AL,DL  
              ADD       AL,#0X30  
              CMP       AL,#0X3A  
              JL        OUTP2  
              ADD       AL, #0X07  
       OUTP2:  
              INT       0X10  
              LOOP      PRINT_DIGIT2    
    
print_nl:      
              mov       ax,#0xe0d  
              int       0x10  
              mov       al,#0xa  
              int       0x10  


msg1:
	.byte 13,10
	.ascii "Now we are in SETUP, yeah!"
	.byte 13,10,13,10

msg2:
	.byte 13,10
	.ascii "Cursor POS:"

msg3:
	.byte 13,10
	.ascii "Memory SIZE:"




.text
endtext:
.data
enddata:
.bss
endbss:
