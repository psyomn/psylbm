#!/usr/bin/env python

# old text API
#
# other considerations: you could maybe use `wrk` for this sort of
# thing, but considering the project, it might be somewhat overkill
# over a simple python script

import socket
import sys
import argparse

default_range = range(1, 10)
LOGIN_TOKENS = []
SYNC_REQUEST_NUM = 10

def banner(label):
    print("---------------- {}".format(label))

def insert_users():
    banner("insert-users")
    for i in default_range:
        msg = 'reg|user{}|pass{}'.format(i, i)
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(bytes(msg, "utf-8"), ("127.0.0.1", 8080))

def login():
    banner("login")
    for i in default_range:
        msg = 'auth|user{}|pass{}'.format(i, i)
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(bytes(msg, "utf-8"), ("127.0.0.1", 8080))
        (token, _) = sock.recvfrom(1024)
        print("login: ", token)
        LOGIN_TOKENS.append(str(token).split('|')[1])

def insert_bookmarks():
    banner("insert-bookmarks")
    for token in LOGIN_TOKENS:
        for i in default_range:
            msg = 'ins|bookmark-{}|title-{}|1|2|3|{}'.format(i, i, token)
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.sendto(bytes(msg, "utf-8"), ("127.0.0.1", 8080))
            (resp, _) = sock.recvfrom(1024)

def syncdata():
    banner("sync-data")
    for _ in range(1, SYNC_REQUEST_NUM):
        for token in LOGIN_TOKENS:
            msg = 'sync|{}'.format(token)
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.sendto(bytes(msg, "utf-8"), ("127.0.0.1", 8080))
            (resp, _) = sock.recvfrom(1024)
            print('.')

parser = argparse.ArgumentParser()
parser.add_argument("--brute", help="run a bunch of sync queries against project")
parser.add_argument("--create-users", help="create a bunch of users")
parser.add_argument("--insert-bookmarks", help="create default bookmarks")

args = parser.parse_args()

if args.brute:
    login()
    syncdata()
elif args.create_users:
    insert_users()
elif args.insert_bookmarks:
    login()
    insert_bookmarks()
