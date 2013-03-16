# MAC Changer: IEEE OUI "parser"
#
# Authors:
#      Alvaro Lopez Ortega <alvaro@alobbs.com>
# 
# Copyright (C) 2002,2003 Alvaro Lopez Ortega
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA
# 

from sys import stdout
from string import split, join, capitalize
import urllib2

SLASHES = '|/-\\'
URL     = 'http://standards.ieee.org/regauth/oui/oui.txt'
OUTPUT  = 'OUI.list'

def download ():
    http = urllib2.urlopen(URL)

    n = i = 0
    buffer = ""
    while 1:
        chunk = http.read(1024)
        length = len(chunk)
        
        if length == 0: break
        buffer = buffer + chunk

        i = i + 1
        n = n + length
        print "%s - Downloading: %d bytes\r" % (SLASHES[i%4], n),
        stdout.flush()

    print
    return buffer


try:
    output = open (OUTPUT, "w")
except IOError:
        print "Cannot open %s for writting" % (OUTPUT)
        raise SystemExit

try:
    f = open ("oui.txt", "r")
    print "Reading file oui.txt"
    content = f.read()    
    f.close()
except IOError:
    print "Downloading file %s" % (URL)
    content = download()

for l in split (content, '\n'):
    if l.find ("(hex)") != -1:
        mac  = split(l[:8], "-")
        name = join (map(lambda x: capitalize(x), split(l[18:], " ")), " ")
        line = '%s %s %s %s' % (mac[0],mac[1],mac[2], name)

        print 'Adding MAC: %s\r' % (line[:9]),
        output.write (line+'\n')

print
output.close()
