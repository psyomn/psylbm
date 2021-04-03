#!/usr/bin/env python

# old text API
#
# other considerations: you could maybe use `wrk` for this sort of
# thing, but considering the project, it might be somewhat overkill
# over a simple python script

import socket
import sys
import argparse
import time

default_range = range(1, 1000)
LOGIN_TOKENS = []
SYNC_REQUEST_NUM = 10000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def banner(label):
    print("---------------- {}".format(label))

def insert_users():
    banner("insert-users")
    for i in default_range:
        msg = 'reg|user{}|pass{}'.format(i, i)
        sock.sendto(bytes(msg, "ascii"), ("127.0.0.1", 8080))

def login():
    banner("login")
    for i in default_range:
        msg = 'auth|user{}|pass{}'.format(i, i)
        sock.sendto(bytes(msg, "ascii"), ("127.0.0.1", 8080))
        (token, _) = sock.recvfrom(1024)
        clean_token = (token.split(b'|')[1]).decode('ascii')
        print("login: ", clean_token)
        LOGIN_TOKENS.append(clean_token)

def insert_bookmarks():
    banner("insert-bookmarks")
    for token in LOGIN_TOKENS:
        for i in default_range:
            # eg: ins|bookmark-1|book-title|1|2|3|28b8a728b932fb27095582335761f06f656087af5be9e6bd58f1525ca76ec0e9
            msg = "ins|bookmark-name{}|title-{}|1|2|3|{}".format(i, i, token)
            sock.sendto(bytes(msg, "ascii"), ("127.0.0.1", 8080))
            (resp, _) = sock.recvfrom(1024)
            print('.', end='')
    print()

def syncdata():
    banner("sync-data")
    for _ in range(1, SYNC_REQUEST_NUM):
        for token in LOGIN_TOKENS:
            msg = 'sync|{}'.format(token)
            sock.sendto(bytes(msg, "ascii"), ("127.0.0.1", 8080))
            (resp, _) = sock.recvfrom(1024)
            print("sync-response:", resp)

parser = argparse.ArgumentParser()
parser.add_argument("--brute",
                    action="store_true",
                    help="run a bunch of sync queries against project")

parser.add_argument("--create-users",
                    action="store_true",
                    help="create a bunch of users")

parser.add_argument("--insert-bookmarks",
                    action="store_true",
                    help="create default bookmarks")

args = parser.parse_args()

if args.brute:
    login()
    syncdata()
elif args.create_users:
    insert_users()
elif args.insert_bookmarks:
    login()
    insert_bookmarks()
