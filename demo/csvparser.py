#!/usr/bin/env python
# -*- coding: utf-8 -*-

import csv
import sys
import codecs

outstr = ""

addressbook = csv.reader(sys.stdin)

def rowformat(aRow):
  if ((len(aRow) >= 5) & (aRow[4] != "")):
    return "%s %s様;%s;%s\n%s" % (aRow[0], aRow[1], aRow[2], aRow[3], aRow[4])
  else:
    return "%s %s様;%s;%s" % (aRow[0], aRow[1], aRow[2], aRow[3])

nextrow = addressbook.next()

if (nextrow):
  outstr = rowformat(nextrow)

for row in addressbook:
  outstr += "|" + rowformat(row)

print outstr
