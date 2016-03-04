// -------------------------
// Hiena scannerserver "dir"
// -------------------------

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


/* CONTEMPLATION
   =============
   is a directory useful as a Hiena source?

   a POSIX directory literally looks like this:
   	name ino
	name ino
	name ino
	name ino

   it's a list of references.

   it's stream equivalent would be html.
   	<a href=URL>name</a>
   	<a href=URL>name</a>
   	<a href=URL>name</a>
   	<a href=URL>name</a>

   knowing how Hiena parses one, will illuminate how
   to parse the other.

   Okay, so lets build our html scenario.
       Setup scannerlib in host filesystem
           setup host servers
	       HOST_FILE
	       HOST_URL
           setup "html" scanner
	       which treats elements as a nested hierarchy
	       which treats anchor elements as directory entries
       Create a new parse packet,
	   add source addr FILEPATH
	   add source srvr HOST_FILE
       Create a new access path,
	   add domain {the parse packet}
	   add scanners SCANNERLIB
       Create a new request 'ls .'
       Operate {the request} on {the access path}
           Operate {the scanners} on {the parse packet}
	       Create object map in parse packet.
	       ---> here we use the rules in "html" scanner
	       ---> anchor href's get mapped as directory entries
	       ---> its up to the "html" scanner to define entries as file or dir
	       Create the access path's directory with entries from the map.
	         for each entry:
	           Create new parse packet
		       add source addr href URL
		       add source srvr HOST_URL
		   Create new access path
		       add domain {parse packet}
		       add scanners {...TBD}
	   Operate {the request} on {access path and parse packet}
	       ---> list directory entries from access path
       Create a new request 'ls name'
       Operate {the request} on {the access path}
           Skip scanner mapping phase
	   Operate {the request} on {access path and parse packet}
	       Selct access path for 'name' directory entry
	       Modify request to eliminate 'name' from request
	       Operate {the request} on 'name' {access path}

   Ok, so the lesson here,

   	1)	Require the scanners to define source addr of directory entries.


   */

/*
   provides a set of functions
   which turn a directory into a file stream for reading
   and which turn a file stream into a directory for writing.

   reading
   =======
   as simple as transcoding a directory stream
   a serialized output of directory entries will suffice.
   each directory entry is addressable.

   the man page for readdir on LINUX reveals a dirent
   	ino
	offset
	reclen
	type
	name

   which leaves a bunch of valuable meta-data out of the picture.





   writing
   =======
   a fragment presented to a scanner for writing
   must have a specified action

   A typical directory is un-ordered so some functions just map
   to a simple version.

      insert <new> before <addr>	// converted to append <new>
      remove <addr>
      append <new> at end of file
      overwrite entire file with <new>
       
      replace <addr> with <new>		// converted to remove <addr>; append <new>
      move <addr> to before <addr2>
      replace <addr> with <addr2>


      

   */

