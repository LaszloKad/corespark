/*
 * Copyright 1993, 1995 Christopher Seiwald.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

# include "jam.h"
# include "lists.h"
# include "execcmd.h"
# include <errno.h>
# include <time.h>

#if defined(sun) || defined(__sun)
#include <unistd.h> /* need to include unistd.h on sun for the vfork prototype*/
#include <wait.h>
#endif

# ifdef USE_EXECUNIX
# include <sys/times.h>

# ifdef NO_VFORK
# define vfork() fork()
# endif

/*
 * execunix.c - execute a shell script on UNIX/WinNT/OS2/AmigaOS
 *
 * If $(JAMSHELL) is defined, uses that to formulate execvp()/spawnvp().
 * The default is:
 *
 *	/bin/sh -c %		[ on UNIX/AmigaOS ]
 *	cmd.exe /c %		[ on OS2/WinNT ]
 *
 * Each word must be an individual element in a jam variable value.
 *
 * In $(JAMSHELL), % expands to the command string and ! expands to 
 * the slot number (starting at 1) for multiprocess (-j) invocations.
 * If $(JAMSHELL) doesn't include a %, it is tacked on as the last
 * argument.
 *
 * Don't just set JAMSHELL to /bin/sh or cmd.exe - it won't work!
 *
 * External routines:
 *	execcmd() - launch an async command execution
 * 	execwait() - wait and drive at most one execution completion
 *
 * Internal routines:
 *	onintr() - bump intr to note command interruption
 *
 * 04/08/94 (seiwald) - Coherent/386 support added.
 * 05/04/94 (seiwald) - async multiprocess interface
 * 01/22/95 (seiwald) - $(JAMSHELL) support
 * 06/02/97 (gsar)    - full async multiprocess support for Win32
 */

static int intr = 0;
static int cmdsrunning = 0;
static void (*istat)( int );

static struct
{
	int	pid; /* on win32, a real process handle */
	void	(*func)( void *closure, int status, timing_info* );
	void 	*closure;
} cmdtab[ MAXJOBS ] = {{0}};

/*
 * onintr() - bump intr to note command interruption
 */

void
onintr( int disp )
{
	intr++;
	printf( "...interrupted\n" );
}

/*
 * execcmd() - launch an async command execution
 */

void
execcmd( 
	char *string,
	void (*func)( void *closure, int status, timing_info* ),
	void *closure,
	LIST *shell )
{
	int pid;
	int slot;
	char *argv[ MAXARGC + 1 ];	/* +1 for NULL */

	/* Find a slot in the running commands table for this one. */

	for( slot = 0; slot < MAXJOBS; slot++ )
	    if( !cmdtab[ slot ].pid )
		break;

	if( slot == MAXJOBS )
	{
	    printf( "no slots for child!\n" );
	    exit( EXITBAD );
	}


	/* Forumulate argv */
	/* If shell was defined, be prepared for % and ! subs. */
	/* Otherwise, use stock /bin/sh (on unix) or cmd.exe (on NT). */

	if( shell )
	{
	    int i;
	    char jobno[4];
	    int gotpercent = 0;

	    sprintf( jobno, "%d", slot + 1 );

	    for( i = 0; shell && i < MAXARGC; i++, shell = list_next( shell ) )
	    {
		switch( shell->string[0] )
		{
		case '%':	argv[i] = string; gotpercent++; break;
		case '!':	argv[i] = jobno; break;
		default:	argv[i] = shell->string;
		}
		if( DEBUG_EXECCMD )
		    printf( "argv[%d] = '%s'\n", i, argv[i] );
	    }

	    if( !gotpercent )
		argv[i++] = string;

	    argv[i] = 0;
	}
	else
	{
	    argv[0] = "/bin/sh";
	    argv[1] = "-c";
	    argv[2] = string;
	    argv[3] = 0;
	}

	/* Catch interrupts whenever commands are running. */

	if( !cmdsrunning++ )
	    istat = signal( SIGINT, onintr );

	/* Start the command */

	if ((pid = vfork()) == 0) 
   	{
		execvp( argv[0], argv );
		_exit(127);
	}

	if( pid == -1 )
	{
	    perror( "vfork" );
	    exit( EXITBAD );
	}

	/* Save the operation for execwait() to find. */

	cmdtab[ slot ].pid = pid;
	cmdtab[ slot ].func = func;
	cmdtab[ slot ].closure = closure;

	/* Wait until we're under the limit of concurrent commands. */
	/* Don't trust globs.jobs alone. */

	while( cmdsrunning >= MAXJOBS || cmdsrunning >= globs.jobs )
	    if( !execwait() )
		break;
}

/*
 * execwait() - wait and drive at most one execution completion
 */

int
execwait()
{
	int i;
	int status, w;
	int rstat;
    timing_info time;
    struct tms old_time, new_time;
    
	/* Handle naive make1() which doesn't know if cmds are running. */

	if( !cmdsrunning )
	    return 0;

    times(&old_time);
    
	/* Pick up process pid and status */
	while( ( w = wait( &status ) ) == -1 && errno == EINTR )
		;

	if( w == -1 )
	{
	    printf( "child process(es) lost!\n" );
	    perror("wait");
	    exit( EXITBAD );
	}

    times(&new_time);

    time.system = (double)(new_time.tms_cstime - old_time.tms_cstime) / CLOCKS_PER_SEC;
    time.user = (double)(new_time.tms_cutime - old_time.tms_cutime) / CLOCKS_PER_SEC;
    
	/* Find the process in the cmdtab. */

	for( i = 0; i < MAXJOBS; i++ )
	    if( w == cmdtab[ i ].pid )
		break;

	if( i == MAXJOBS )
	{
	    printf( "waif child found!\n" );
	    exit( EXITBAD );
	}

    
	/* Drive the completion */

	if( !--cmdsrunning )
	    signal( SIGINT, istat );

	if( intr )
	    rstat = EXEC_CMD_INTR;
	else if( w == -1 || status != 0 )
	    rstat = EXEC_CMD_FAIL;
	else
	    rstat = EXEC_CMD_OK;

	cmdtab[ i ].pid = 0;

	(*cmdtab[ i ].func)( cmdtab[ i ].closure, rstat, &time );

	return 1;
}

# if defined( OS_NT ) && !defined( __BORLANDC__ )

# define WIN32_LEAN_AND_MEAN

# include <windows.h>		/* do the ugly deed */

static int
my_wait( int *status )
{
	int i, num_active = 0;
	DWORD exitcode, waitcode;
	static HANDLE *active_handles = 0;

	if (!active_handles)
	    active_handles = (HANDLE *)malloc(globs.jobs * sizeof(HANDLE) );

	/* first see if any non-waited-for processes are dead,
	 * and return if so.
	 */
	for ( i = 0; i < globs.jobs; i++ ) {
	    if ( cmdtab[i].pid ) {
		if ( GetExitCodeProcess((HANDLE)cmdtab[i].pid, &exitcode) ) {
		    if ( exitcode == STILL_ACTIVE )
			active_handles[num_active++] = (HANDLE)cmdtab[i].pid;
		    else {
			CloseHandle((HANDLE)cmdtab[i].pid);
			*status = (int)((exitcode & 0xff) << 8);
			return cmdtab[i].pid;
		    }
		}
		else
		    goto FAILED;
	    }
	}

	/* if a child exists, wait for it to die */
	if ( !num_active ) {
	    errno = ECHILD;
	    return -1;
	}
	waitcode = WaitForMultipleObjects( num_active,
					   active_handles,
					   FALSE,
					   INFINITE );
	if ( waitcode != WAIT_FAILED ) {
	    if ( waitcode >= WAIT_ABANDONED_0
		&& waitcode < WAIT_ABANDONED_0 + num_active )
		i = waitcode - WAIT_ABANDONED_0;
	    else
		i = waitcode - WAIT_OBJECT_0;
	    if ( GetExitCodeProcess(active_handles[i], &exitcode) ) {
		CloseHandle(active_handles[i]);
		*status = (int)((exitcode & 0xff) << 8);
		return (int)active_handles[i];
	    }
	}

FAILED:
	errno = GetLastError();
	return -1;
    
}

# endif /* NT && !__BORLANDC__ */

# endif /* USE_EXECUNIX */
