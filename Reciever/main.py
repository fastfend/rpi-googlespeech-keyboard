#!/usr/bin/env python
import sys

list = sys.argv
list.pop(0)

NULL_CHAR = chr(0)
dict = {'a': 4,'b': 5,'c': 6,'d': 7,'e': 8,'f': 9,'g': 10,'h': 11,'i': 12,'j': 13,'k': 14,'l': 15,'m': 16,'n': 17,'o': 18,'p': 19,'q': 20,'r': 21,'s': 22,'t': 23,'u': 24,'v': 25,'w': 26,'x': 27,'y': 28,'z': 29,'0': 30,'1': 31,'2': 32,'3': 32,'4': 34,'5': 35,'6': 36,'7': 37,'8': 38,'9': 39 }

def write_report(report):
    with open('/dev/hidg0', 'rb+') as fd:
        fd.write(report.encode())

s = ' '.join(list)
for c in s:
    if c == ' ':
        write_report(NULL_CHAR*2+chr(44)+NULL_CHAR*5)
    elif c == 'Ą':
        write_report(chr(96)+NULL_CHAR+chr(4)+NULL_CHAR*5)
    elif c == 'Ć':
        write_report(chr(96)+NULL_CHAR+chr(6)+NULL_CHAR*5)
    elif c == 'Ę':
        write_report(chr(96)+NULL_CHAR+chr(8)+NULL_CHAR*5)
    elif c == 'Ł':
        write_report(chr(96)+NULL_CHAR+chr(15)+NULL_CHAR*5)
    elif c == 'Ń':
        write_report(chr(96)+NULL_CHAR+chr(17)+NULL_CHAR*5)
    elif c == 'Ó':
        write_report(chr(96)+NULL_CHAR+chr(18)+NULL_CHAR*5)
    elif c == 'Ś':
        write_report(chr(96)+NULL_CHAR+chr(22)+NULL_CHAR*5)
    elif c == 'Ż':
        write_report(chr(96)+NULL_CHAR+chr(29)+NULL_CHAR*5)
    elif c == 'Ź':
        write_report(chr(96)+NULL_CHAR+chr(27)+NULL_CHAR*5)
    elif c == 'ą':
        write_report(chr(64)+NULL_CHAR+chr(4)+NULL_CHAR*5)
    elif c == 'ć':
        write_report(chr(64)+NULL_CHAR+chr(6)+NULL_CHAR*5)
    elif c == 'ę':
        write_report(chr(64)+NULL_CHAR+chr(8)+NULL_CHAR*5)
    elif c == 'ł':
        write_report(chr(64)+NULL_CHAR+chr(15)+NULL_CHAR*5)
    elif c == 'ń':
        write_report(chr(64)+NULL_CHAR+chr(17)+NULL_CHAR*5)
    elif c == 'ó':
        write_report(chr(64)+NULL_CHAR+chr(18)+NULL_CHAR*5)
    elif c == 'ś':
        write_report(chr(64)+NULL_CHAR+chr(22)+NULL_CHAR*5)
    elif c == 'ż':
        write_report(chr(64)+NULL_CHAR+chr(29)+NULL_CHAR*5)
    elif c == 'ź':
        write_report(chr(64)+NULL_CHAR+chr(27)+NULL_CHAR*5)
    elif c.islower():
        write_report(NULL_CHAR*2+chr(dict[c])+NULL_CHAR*5)
    elif c.isupper():
        write_report(chr(32)+NULL_CHAR+chr(dict[c.lower()])+NULL_CHAR*5)
    else:
        write_report(NULL_CHAR*2+chr(dict[c])+NULL_CHAR*5)
    write_report(NULL_CHAR*8)
