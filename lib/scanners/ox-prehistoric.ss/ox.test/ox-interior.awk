#!/usr/bin/awk -f

/^[ ]+/	 { print "[indent]"; }
BEGIN    { print "BEGIN"; }
