#include <common.h>
#include <stdarg.h>
#include <malloc.h>
#include <console.h>
#include <exports.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_AMIGAONEG3SE
int console_changed = 0;
#endif

#ifdef CFG_CONSOLE_IS_IN_ENV

#ifdef CFG_CONSOLE_OVERWRITE_ROUTINE
extern int overwrite_console (void);
#define OVERWRITE_CONSOLE overwrite_console ()
#else
#define OVERWRITE_CONSOLE 0
#endif 

#endif 

static int console_setfile (int file, device_t * dev)
{
	int error = 0;

	if (dev == NULL)
		return -1;

	switch (file) {
	case stdin:
	case stdout:
	case stderr:
		
		if (dev->start) {
			error = dev->start ();
			
			if (error < 0)
				break;
		}

		stdio_devices[file] = dev;

		switch (file) {
		case stdin:
			gd->jt[XF_getc] = dev->getc;
			gd->jt[XF_tstc] = dev->tstc;
			break;
		case stdout:
			gd->jt[XF_putc] = dev->putc;
			gd->jt[XF_puts] = dev->puts;
			gd->jt[XF_printf] = printf;
			break;
		}
		break;

	default:		
		error = -1;
	}
	return error;
}

void serial_printf (const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	serial_puts (printbuffer);
}

int fgetc (int file)
{
	if (file < MAX_FILES)
		return stdio_devices[file]->getc ();

	return -1;
}

int ftstc (int file)
{
	if (file < MAX_FILES)
		return stdio_devices[file]->tstc ();

	return -1;
}

void fputc (int file, const char c)
{
	if (file < MAX_FILES)
		stdio_devices[file]->putc (c);
}

void fputs (int file, const char *s)
{
	if (file < MAX_FILES)
		stdio_devices[file]->puts (s);
}

void fprintf (int file, const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	fputs (file, printbuffer);
}

int getc (void)
{
	if (gd->flags & GD_FLG_DEVINIT) {
		
		return fgetc (stdin);
	}

	return serial_getc ();
}

int getcB (unsigned int escape_cnt, char* pCh)
{
	
	return _serial_getcB (AP_CP_UART, escape_cnt, pCh);
}

int putcB (const char c, unsigned int escape_cnt)
{
	
	return _serial_putcB (c, AP_CP_UART, escape_cnt);
}

int putsB (const char *s, unsigned int len, unsigned int escape_cnt)
{
	return _serial_putsB (s, AP_CP_UART, len, escape_cnt);
}

int tstc (void)
{
	if (gd->flags & GD_FLG_DEVINIT) {
		
		return ftstc (stdin);
	}

	return serial_tstc ();
}

void putc (const char c)
{
#ifdef CONFIG_SILENT_CONSOLE
	if (gd->flags & GD_FLG_SILENT)
		return;
#endif

	if (gd->flags & GD_FLG_DEVINIT) {
		
		fputc (stdout, c);
	} else {
		
		serial_putc (c);
	}
}

void puts (const char *s)
{
#ifdef CONFIG_SILENT_CONSOLE
	if (gd->flags & GD_FLG_SILENT)
		return;
#endif

	if (gd->flags & GD_FLG_DEVINIT) {
		
		fputs (stdout, s);
	} else {
		
		serial_puts (s);
	}
}

void printf (const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	puts (printbuffer);
}

void vprintf (const char *fmt, va_list args)
{
	uint i;
	char printbuffer[CFG_PBSIZE];

	i = vsprintf (printbuffer, fmt, args);

	puts (printbuffer);
}

static int ctrlc_disabled = 0;	
static int ctrlc_was_pressed = 0;
int ctrlc (void)
{
	if (!ctrlc_disabled && gd->have_console) {
		if (tstc ()) {
			switch (getc ()) {
			case 0x03:		
				ctrlc_was_pressed = 1;
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}

int disable_ctrlc (int disable)
{
	int prev = ctrlc_disabled;	

	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc (void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc (void)
{
	ctrlc_was_pressed = 0;
}

#ifdef CONFIG_MODEM_SUPPORT_DEBUG
char	screen[1024];
char *cursor = screen;
int once = 0;
inline void dbg(const char *fmt, ...)
{
	va_list	args;
	uint	i;
	char	printbuffer[CFG_PBSIZE];

	if (!once) {
		memset(screen, 0, sizeof(screen));
		once++;
	}

	va_start(args, fmt);

	i = vsprintf(printbuffer, fmt, args);
	va_end(args);

	if ((screen + sizeof(screen) - 1 - cursor) < strlen(printbuffer)+1) {
		memset(screen, 0, sizeof(screen));
		cursor = screen;
	}
	sprintf(cursor, printbuffer);
	cursor += strlen(printbuffer);

}
#else
inline void dbg(const char *fmt, ...)
{
}
#endif

int console_assign (int file, char *devname)
{
	int flag, i;

	switch (file) {
	case stdin:
		flag = DEV_FLAGS_INPUT;
		break;
	case stdout:
	case stderr:
		flag = DEV_FLAGS_OUTPUT;
		break;
	default:
		return -1;
	}

	for (i = 1; i <= ListNumItems (devlist); i++) {
		device_t *dev = ListGetPtrToItem (devlist, i);

		if (strcmp (devname, dev->name) == 0) {
			if (dev->flags & flag)
				return console_setfile (file, dev);

			return -1;
		}
	}

	return -1;
}

int console_init_f (void)
{
	gd->have_console = 1;

#ifdef CONFIG_SILENT_CONSOLE
	if (getenv("silent") != NULL)
		gd->flags |= GD_FLG_SILENT;
#endif

	return (0);
}

#if defined(CFG_CONSOLE_IS_IN_ENV) || defined(CONFIG_SPLASH_SCREEN) || defined(CONFIG_SILENT_CONSOLE)

device_t *search_device (int flags, char *name)
{
	int i, items;
	device_t *dev = NULL;

	items = ListNumItems (devlist);
	if (name == NULL)
		return dev;

	for (i = 1; i <= items; i++) {
		dev = ListGetPtrToItem (devlist, i);
		if ((dev->flags & flags) && (strcmp (name, dev->name) == 0)) {
			break;
		}
	}
	return dev;
}
#endif 

#ifdef CFG_CONSOLE_IS_IN_ENV

int console_init_r (void)
{
	char *stdinname, *stdoutname, *stderrname;
	device_t *inputdev = NULL, *outputdev = NULL, *errdev = NULL;
#ifdef CFG_CONSOLE_ENV_OVERWRITE
	int i;
#endif 

	gd->jt[XF_getc] = serial_getc;
	gd->jt[XF_tstc] = serial_tstc;
	gd->jt[XF_putc] = serial_putc;
	gd->jt[XF_puts] = serial_puts;
	gd->jt[XF_printf] = serial_printf;

	stdinname  = getenv ("stdin");
	stdoutname = getenv ("stdout");
	stderrname = getenv ("stderr");

	if (OVERWRITE_CONSOLE == 0) { 	
		inputdev  = search_device (DEV_FLAGS_INPUT,  stdinname);
		outputdev = search_device (DEV_FLAGS_OUTPUT, stdoutname);
		errdev    = search_device (DEV_FLAGS_OUTPUT, stderrname);
	}
	
	if (inputdev == NULL) {
		inputdev  = search_device (DEV_FLAGS_INPUT,  "serial");
	}
	if (outputdev == NULL) {
		outputdev = search_device (DEV_FLAGS_OUTPUT, "serial");
	}
	if (errdev == NULL) {
		errdev    = search_device (DEV_FLAGS_OUTPUT, "serial");
	}
	
	if (outputdev != NULL) {
		console_setfile (stdout, outputdev);
	}
	if (errdev != NULL) {
		console_setfile (stderr, errdev);
	}
	if (inputdev != NULL) {
		console_setfile (stdin, inputdev);
	}

	gd->flags |= GD_FLG_DEVINIT;	

#ifndef CFG_CONSOLE_INFO_QUIET
	
	puts ("In:    ");
	if (stdio_devices[stdin] == NULL) {
		puts ("No input devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdin]->name);
	}

	puts ("Out:   ");
	if (stdio_devices[stdout] == NULL) {
		puts ("No output devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdout]->name);
	}

	puts ("Err:   ");
	if (stdio_devices[stderr] == NULL) {
		puts ("No error devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stderr]->name);
	}
#endif 

#ifdef CFG_CONSOLE_ENV_OVERWRITE
	
	for (i = 0; i < 3; i++) {
		setenv (stdio_names[i], stdio_devices[i]->name);
	}
#endif 

#if 0
	
	if ((stdio_devices[stdin] == NULL) && (stdio_devices[stdout] == NULL))
		return (0);
#endif
	return (0);
}

#else 

int console_init_r (void)
{
	device_t *inputdev = NULL, *outputdev = NULL;
	int i, items = ListNumItems (devlist);

#ifdef CONFIG_SPLASH_SCREEN

	if (getenv("splashimage") != NULL)
		outputdev = search_device (DEV_FLAGS_OUTPUT, "nulldev");
#endif

#ifdef CONFIG_SILENT_CONSOLE
	
	if (gd->flags & GD_FLG_SILENT)
		outputdev = search_device (DEV_FLAGS_OUTPUT, "nulldev");
#endif

	for (i = 1;
	     (i <= items) && ((inputdev == NULL) || (outputdev == NULL));
	     i++
	    ) {
		device_t *dev = ListGetPtrToItem (devlist, i);

		if ((dev->flags & DEV_FLAGS_INPUT) && (inputdev == NULL)) {
			inputdev = dev;
		}
		if ((dev->flags & DEV_FLAGS_OUTPUT) && (outputdev == NULL)) {
			outputdev = dev;
		}
	}

	if (outputdev != NULL) {
		console_setfile (stdout, outputdev);
		console_setfile (stderr, outputdev);
	}

	if (inputdev != NULL) {
		console_setfile (stdin, inputdev);
	}

	gd->flags |= GD_FLG_DEVINIT;	

#ifndef CFG_CONSOLE_INFO_QUIET
	
	puts ("In:    ");
	if (stdio_devices[stdin] == NULL) {
		puts ("No input devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdin]->name);
	}

	puts ("Out:   ");
	if (stdio_devices[stdout] == NULL) {
		puts ("No output devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdout]->name);
	}

	puts ("Err:   ");
	if (stdio_devices[stderr] == NULL) {
		puts ("No error devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stderr]->name);
	}
#endif 

	for (i = 0; i < 3; i++) {
		setenv (stdio_names[i], stdio_devices[i]->name);
	}

#if 0
	
	if ((stdio_devices[stdin] == NULL) && (stdio_devices[stdout] == NULL))
		return (0);
#endif

	return (0);
}

#endif
