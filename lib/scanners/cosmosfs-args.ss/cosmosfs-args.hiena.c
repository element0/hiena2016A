/* This was hand-coded as if "cosmosfs-args.hiena"
   were precompiled by the Hiena compiler.
   */

#include "../../src/scanner_compiled.h"

/* scanner header */
int compiler_version = 1;

/* name table */
enum rule_id {
    COSMOSFS_ARGS,
    CMDNAME,
    SOURCES,
    FUSEARGS,
    FUSEARG,
    FUSEOPT, 
    MOUNTPOINT,
    FILEPATH,
    FILENAME
};
int   name_str_dim = 9;
char *name_str[]={
    "cosmosfs-args",
    "cmdname",
    "sources",
    "fuseargs",
    "fusearg",
    "fuseopt",
    "mountpoint",
    "filepath",
    "filename",
};
int name_table[]={
    /* ruleid, guid, fsm, fsmlen, nametab, nametablen */
    COSMOSFS_ARGS, 10000, 0,12,0,0,
    CMDNAME,       10001,12, 5,0,0,
    SOURCES,       10002, 0 ,0,0,0,
    FUSEARGS,      10003, 0 ,0,0,0,
    FUSEARG,       10004, 0 ,0,0,0,
    FUSEOPT,       10005, 0 ,0,0,0,
    MOUNTPOINT,    10006, 0 ,0,0,0,
    FILEPATH,      10007, 0 ,0,0,0,
    FILENAME,      10008, 0 ,0,0,0,
};
/* FSM table */
int fsm_table[]={
    /*          STAT,   TRANSID,     JUMP */// JUMP is a relative offset
    /*          ----    -------      ---- */// makes it easier on hand-coding
    /* RULE: cosmosfs-args */
    INCOMPLETE_STATE, 	'a',		4,
    INCOMPLETE_STATE,   'b',		3,
    INCOMPLETE_STATE,   '.',		2,
    NOMATCH,		0,		0,
    INCOMPLETE_STATE,	'b',		3,
    INCOMPLETE_STATE,   -CMDNAME,	2,
    NOMATCH,		0,		0,
    INCOMPLETE_STATE,   'c',		2,
    NOMATCH,		0,		0,
    INCOMPLETE_STATE,	'd',		2,
    NOMATCH,		0,		0,
    COMPLETE_STATE,	0,		0,

    /* RULE: CMDNAME */
    INCOMPLETE_STATE, 'x',2,
    INCOMPLETE_STATE, 'y',2,
    /* xy */
    INCOMPLETE_STATE, 'y',2,
    /* yx */
    INCOMPLETE_STATE, 'x',1,
    /* (xy|yx) */
    COMPLETE_STATE,0,0,
};
