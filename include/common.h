#ifndef __COMMON_H_
#define __COMMON_H_	1

#undef	_LINUX_CONFIG_H
#define _LINUX_CONFIG_H 1	

typedef unsigned char		uchar;
typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;

#include <config.h>
#include <linux/bitops.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/ptrace.h>
#include <stdarg.h>
#if defined(CONFIG_PCI) && defined(CONFIG_440)
#include <pci.h>
#endif
#if defined(CONFIG_8xx)
#include <asm/8xx_immap.h>
#if defined(CONFIG_MPC852)	|| defined(CONFIG_MPC852T)	|| \
    defined(CONFIG_MPC859)	|| defined(CONFIG_MPC859T)	|| \
    defined(CONFIG_MPC859DSL)	|| \
    defined(CONFIG_MPC866)	|| defined(CONFIG_MPC866T)	|| \
    defined(CONFIG_MPC866P)
# define CONFIG_MPC866_FAMILY 1
#elif defined(CONFIG_MPC870) \
   || defined(CONFIG_MPC875) \
   || defined(CONFIG_MPC880) \
   || defined(CONFIG_MPC885)
# define CONFIG_MPC885_FAMILY   1
#endif
#if   defined(CONFIG_MPC860)	   \
   || defined(CONFIG_MPC860T)	   \
   || defined(CONFIG_MPC866_FAMILY) \
   || defined(CONFIG_MPC885_FAMILY)
# define CONFIG_MPC86x 1
#endif
#elif defined(CONFIG_5xx)
#include <asm/5xx_immap.h>
#elif defined(CONFIG_MPC5xxx)
#include <mpc5xxx.h>
#elif defined(CONFIG_MPC8220)
#include <asm/immap_8220.h>
#elif defined(CONFIG_8260)
#if   defined(CONFIG_MPC8247) \
   || defined(CONFIG_MPC8248) \
   || defined(CONFIG_MPC8271) \
   || defined(CONFIG_MPC8272)
#define CONFIG_MPC8272_FAMILY	1
#endif
#if defined(CONFIG_MPC8272_FAMILY)
#define CONFIG_MPC8260	1
#endif
#include <asm/immap_8260.h>
#endif
#ifdef CONFIG_MPC85xx
#include <mpc85xx.h>
#include <asm/immap_85xx.h>
#endif
#ifdef CONFIG_MPC83XX
#include <mpc83xx.h>
#include <asm/immap_83xx.h>
#endif
#ifdef	CONFIG_4xx
#include <ppc4xx.h>
#endif
#ifdef CONFIG_HYMOD
#include <board/hymod/hymod.h>
#endif
#ifdef CONFIG_ARM
#define asmlinkage	
#endif

#include <part.h>
#include <flash.h>
#include <image.h>

#ifdef	DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#define debugX(level,fmt,args...) if (DEBUG>=level) printf(fmt,##args);
#else
#define debug(fmt,args...)
#define debugX(level,fmt,args...)
#endif	

typedef void (interrupt_handler_t)(void *);

#include <asm/u-boot.h> 
#include <asm/global_data.h>	

#if defined(CONFIG_TQM823M) || defined(CONFIG_TQM850M) || \
    defined(CONFIG_TQM855M) || defined(CONFIG_TQM860M) || \
    defined(CONFIG_TQM862M) || defined(CONFIG_TQM866M) || \
    defined(CONFIG_TQM885D)
# ifndef CONFIG_TQM8xxM
#  define CONFIG_TQM8xxM
# endif
#endif
#if defined(CONFIG_TQM823L) || defined(CONFIG_TQM850L) || \
    defined(CONFIG_TQM855L) || defined(CONFIG_TQM860L) || \
    defined(CONFIG_TQM862L) || defined(CONFIG_TQM8xxM)
# ifndef CONFIG_TQM8xxL
#  define CONFIG_TQM8xxL
# endif
#endif

#ifndef CONFIG_SERIAL_MULTI

#if defined(CONFIG_8xx_CONS_SMC1) || defined(CONFIG_8xx_CONS_SMC2) \
 || defined(CONFIG_8xx_CONS_SCC1) || defined(CONFIG_8xx_CONS_SCC2) \
 || defined(CONFIG_8xx_CONS_SCC3) || defined(CONFIG_8xx_CONS_SCC4)

#define CONFIG_SERIAL_MULTI	1

#endif

#endif 

#define min(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x < __y) ? __x : __y; })

#define max(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x > __y) ? __x : __y; })

#ifdef CONFIG_SERIAL_SOFTWARE_FIFO
void	serial_buffered_init (void);
void	serial_buffered_putc (const char);
void	serial_buffered_puts (const char *);
int	serial_buffered_getc (void);
int	serial_buffered_tstc (void);
#endif 

void	hang		(void) __attribute__ ((noreturn));

long int initdram (int);
int	display_options (void);
void	print_size (ulong, const char *);

void	main_loop	(void);
int	run_command	(const char *cmd, int flag);
int	readline	(const char *const prompt);
void	init_cmd_timeout(void);
void	reset_cmd_timeout(void);

void	board_init_f  (ulong);
void	board_init_r  (gd_t *, ulong);
int	checkboard    (void);
int	checkflash    (void);
int	checkdram     (void);
char *	strmhz(char *buf, long hz);
int	last_stage_init(void);
extern ulong monitor_flash_len;

void flash_perror (int);

int	autoscript (ulong addr);

void	print_image_hdr (image_header_t *hdr);

extern ulong load_addr;		

int	env_init     (void);
void	env_relocate (void);
char	*getenv	     (char *);
int	getenv_r     (char *name, char *buf, unsigned len);
int	saveenv	     (void);
#ifdef CONFIG_PPC		
void inline setenv   (char *, char *);
#else
void	setenv	     (char *, char *);
#endif 
#ifdef CONFIG_ARM
# include <asm/mach-types.h>
# include <asm/setup.h>
# include <asm/u-boot-arm.h>	
#endif 
#ifdef CONFIG_I386		
# include <asm/u-boot-i386.h>
#endif 

#ifdef CONFIG_AUTO_COMPLETE
int env_complete(char *var, int maxv, char *cmdv[], int maxsz, char *buf);
#endif

void	pci_init      (void);
void	pci_init_board(void);
void	pciinfo	      (int, int);

#if defined(CONFIG_PCI) && defined(CONFIG_440)
#   if defined(CFG_PCI_PRE_INIT)
    int	   pci_pre_init	       (struct pci_controller * );
#   endif
#   if defined(CFG_PCI_TARGET_INIT)
	void	pci_target_init	     (struct pci_controller *);
#   endif
#   if defined(CFG_PCI_MASTER_INIT)
	void	pci_master_init	     (struct pci_controller *);
#   endif
    int	    is_pci_host		(struct pci_controller *);
#endif

int	misc_init_f   (void);
int	misc_init_r   (int);

void	jumptable_init(void);

int	get_ram_size  (volatile long *, long);

void	reset_phy     (void);
void	fdc_hw_init   (void);

void eeprom_init  (void);
#ifndef CONFIG_SPI
int  eeprom_probe (unsigned dev_addr, unsigned offset);
#endif
int  eeprom_read  (unsigned dev_addr, unsigned offset, uchar *buffer, unsigned cnt);
int  eeprom_write (unsigned dev_addr, unsigned offset, uchar *buffer, unsigned cnt);
#ifdef CONFIG_LWMON
extern uchar pic_read  (uchar reg);
extern void  pic_write (uchar reg, uchar val);
#endif

#if defined(CONFIG_SPI) || !defined(CFG_I2C_EEPROM_ADDR)
# define CFG_DEF_EEPROM_ADDR 0
#else
# define CFG_DEF_EEPROM_ADDR CFG_I2C_EEPROM_ADDR
#endif 

#if defined(CONFIG_SPI)
extern void spi_init_f (void);
extern void spi_init_r (void);
extern ssize_t spi_read	 (uchar *, int, uchar *, int);
extern ssize_t spi_write (uchar *, int, uchar *, int);
#endif

#ifdef CONFIG_RPXCLASSIC
void rpxclassic_init (void);
#endif

void rpxlite_init (void);

#ifdef CONFIG_MBX

void	mbx_init (void);
void	board_serial_init (void);
void	board_ether_init (void);
#endif

#if defined(CONFIG_RPXCLASSIC)	|| defined(CONFIG_MBX) || \
    defined(CONFIG_IAD210)	|| defined(CONFIG_XPEDITE1K) || \
    defined(CONFIG_METROBOX)    || defined(CONFIG_KAREF)
void	board_get_enetaddr (uchar *addr);
#endif

#ifdef CONFIG_HERMES

void hermes_start_lxt980 (int speed);
#endif

#ifdef CONFIG_EVB64260
void  evb64260_init(void);
void  debug_led(int, int);
void  display_mem_map(void);
void  perform_soft_reset(void);
#endif

void	load_sernum_ethaddr (void);

int board_early_init_f (void);
int board_late_init (void);
int board_postclk_init (void); 
int board_early_init_r (void);
void board_poweroff (void);

#if defined(CFG_DRAM_TEST)
int testdram(void);
#endif 

#if defined(CONFIG_5xx) || \
    defined(CONFIG_8xx)
uint	get_immr      (uint);
#endif
uint	get_pir	      (void);
#if defined(CONFIG_MPC5xxx)
uint	get_svr       (void);
#endif
uint	get_pvr	      (void);
uint	get_svr	      (void);
uint	rd_ic_cst     (void);
void	wr_ic_cst     (uint);
void	wr_ic_adr     (uint);
uint	rd_dc_cst     (void);
void	wr_dc_cst     (uint);
void	wr_dc_adr     (uint);
int	icache_status (void);
void	icache_enable (void);
void	icache_disable(void);
int	dcache_status (void);
void	dcache_enable (void);
void	dcache_disable(void);
void	relocate_code (ulong, gd_t *, ulong);
ulong	get_endaddr   (void);
void	trap_init     (ulong);
#if defined (CONFIG_4xx)	|| \
    defined (CONFIG_MPC5xxx)	|| \
    defined (CONFIG_74xx_7xx)	|| \
    defined (CONFIG_74x)	|| \
    defined (CONFIG_75x)	|| \
    defined (CONFIG_74xx)	|| \
    defined (CONFIG_MPC8220)	|| \
    defined (CONFIG_MPC85xx)	|| \
    defined (CONFIG_MPC83XX)
unsigned char	in8(unsigned int);
void		out8(unsigned int, unsigned char);
unsigned short	in16(unsigned int);
unsigned short	in16r(unsigned int);
void		out16(unsigned int, unsigned short value);
void		out16r(unsigned int, unsigned short value);
unsigned long	in32(unsigned int);
unsigned long	in32r(unsigned int);
void		out32(unsigned int, unsigned long value);
void		out32r(unsigned int, unsigned long value);
void		ppcDcbf(unsigned long value);
void		ppcDcbi(unsigned long value);
void		ppcSync(void);
void		ppcDcbz(unsigned long value);
#endif

int	checkcpu      (void);
int	checkicache   (void);
int	checkdcache   (void);
void	upmconfig     (unsigned int, unsigned int *, unsigned int);
ulong	get_tbclk     (void);
void	reset_cpu     (ulong addr);

int	serial_init   (void);
void	serial_addr   (unsigned int);
void	serial_setbrg (void);
void	serial_putc   (const char);
void	serial_putc_raw(const char);
void	serial_puts   (const char *);
int	serial_getc   (void);
int	serial_tstc   (void);

void _serial_reinit(const int port, int baud_divisor);

int getcB (unsigned int escape_cnt, char* pCh);
int putcB (const char c, unsigned int escape_cnt);
int putsB (const char *s, unsigned int len, unsigned int escape_cnt);

void	_serial_setbrg (const int);
void	_serial_putc   (const char, const int);
void	_serial_putc_raw(const char, const int);
void	_serial_puts   (const char *, const int);
int	_serial_getc   (const int);
int	_serial_tstc   (const int);

int	get_clocks (void);
int	get_clocks_866 (void);
int	sdram_adjust_866 (void);
int	adjust_sdram_tbs_8xx (void);
#if defined(CONFIG_8260)
int	prt_8260_clks (void);
#elif defined(CONFIG_MPC83XX)
int print_clock_conf(void);
#elif defined(CONFIG_MPC5xxx)
int	prt_mpc5xxx_clks (void);
#endif
#if defined(CONFIG_MPC8220)
int	prt_mpc8220_clks (void);
#endif
#ifdef CONFIG_4xx
ulong	get_OPB_freq (void);
ulong	get_PCI_freq (void);
#endif
#if defined(CONFIG_S3C2400) || defined(CONFIG_S3C2410) || defined(CONFIG_LH7A40X)
ulong	get_FCLK (void);
ulong	get_HCLK (void);
ulong	get_PCLK (void);
ulong	get_UCLK (void);
#endif
#if defined(CONFIG_LH7A40X)
ulong	get_PLLCLK (void);
#endif
#if defined CONFIG_INCA_IP
uint	incaip_get_cpuclk (void);
#endif
#if defined(CONFIG_IMX)
ulong get_systemPLLCLK(void);
ulong get_FCLK(void);
ulong get_HCLK(void);
ulong get_BCLK(void);
ulong get_PERCLK1(void);
ulong get_PERCLK2(void);
ulong get_PERCLK3(void);
#endif
ulong	get_bus_freq  (ulong);

#if defined(CONFIG_MPC85xx)
typedef MPC85xx_SYS_INFO sys_info_t;
void	get_sys_info  ( sys_info_t * );
#endif

#if defined(CONFIG_4xx) || defined(CONFIG_IOP480)
#  if defined(CONFIG_440)
    typedef PPC440_SYS_INFO sys_info_t;
#	if defined(CONFIG_440SPE)
	 unsigned long determine_sysper(void);
	 unsigned long determine_pci_clock_per(void);
#	endif
#  else
    typedef PPC405_SYS_INFO sys_info_t;
#  endif
void	get_sys_info  ( sys_info_t * );
#endif

#if defined(CONFIG_8xx) || defined(CONFIG_8260)
void	cpu_init_f    (volatile immap_t *immr);
#endif
#if defined(CONFIG_4xx) || defined(CONFIG_MPC85xx) || defined(CONFIG_MCF52x2)
void	cpu_init_f    (void);
#endif

int	cpu_init_r    (void);
#if defined(CONFIG_8260)
int	prt_8260_rsr  (void);
#endif

int	interrupt_init	   (void);
void	timer_interrupt	   (struct pt_regs *);
void	external_interrupt (struct pt_regs *);
void	irq_install_handler(int, interrupt_handler_t *, void *);
void	irq_free_handler   (int);
void	reset_timer	   (void);
ulong	get_timer	   (ulong base);
void	set_timer	   (ulong t);
void	enable_interrupts  (void);
int	disable_interrupts (void);

int	dpram_init (void);
uint	dpram_base(void);
uint	dpram_base_align(uint align);
uint	dpram_alloc(uint size);
uint	dpram_alloc_align(uint size,uint align);
void	post_word_store (ulong);
ulong	post_word_load (void);
void	bootcount_store (ulong);
ulong	bootcount_load (void);
#define BOOTCOUNT_MAGIC		0xB001C041

void mii_init (void);

ulong	lcd_setmem (ulong);

ulong	vfd_setmem (ulong);

ulong	video_setmem (ulong);

void	flush_cache   (unsigned long, unsigned long);

unsigned long long get_ticks(void);
void	wait_ticks    (unsigned long);

void	udelay	      (unsigned long);
ulong	usec2ticks    (unsigned long usec);
ulong	ticks2usec    (unsigned long ticks);
int	init_timebase (void);

ulong	simple_strtoul(const char *cp,char **endp,unsigned int base);
#ifdef CFG_64BIT_VSPRINTF
unsigned long long	simple_strtoull(const char *cp,char **endp,unsigned int base);
#endif
long	simple_strtol(const char *cp,char **endp,unsigned int base);
void	panic(const char *fmt, ...);
int	sprintf(char * buf, const char *fmt, ...);
int	vsprintf(char *buf, const char *fmt, va_list args);

ulong crc32 (ulong, const unsigned char *, uint);
ulong crc32_no_comp (ulong, const unsigned char *, uint);

int	console_init_f(void);	
int	console_init_r(void);	
int	console_assign (int file, char *devname);	
int	ctrlc (void);
int	had_ctrlc (void);	
void	clear_ctrlc (void);	
int	disable_ctrlc (int);	

void	serial_printf (const char *fmt, ...);

int	getc(void);
int	tstc(void);

void	putc(const char c);
void	puts(const char *s);
void	printf(const char *fmt, ...);
void	vprintf(const char *fmt, va_list args);

#define eputc(c)		fputc(stderr, c)
#define eputs(s)		fputs(stderr, s)
#define eprintf(fmt,args...)	fprintf(stderr,fmt ,##args)

#define stdin		0
#define stdout		1
#define stderr		2
#define MAX_FILES	3

void	fprintf(int file, const char *fmt, ...);
void	fputs(int file, const char *s);
void	fputc(int file, const char c);
int	ftstc(int file);
int	fgetc(int file);

int	pcmcia_init (void);

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void	show_boot_progress (int status);
#endif

#ifdef CONFIG_INIT_CRITICAL
#error CONFIG_INIT_CRITICAL is depracted!
#error Read section CONFIG_SKIP_LOWLEVEL_INIT in README.
#endif

#endif
