#ifndef _PDB_READER_H_
#define _PDB_READER_H_

#include <stdio.h>
#include <inttypes.h>
#include <time.h>

/* data format is as per http://www.haodoo.net/?M=hd&P=mPDB22#P */

static const int PDB_HEADER_SIZE = 78;

typedef struct _pdb_header {
    char meta_str[35];      /*  0..34, book name (old) or author name (new) */
    int8_t  version;        /* 35    , version */
    int32_t _date_creat;    /* 36..39, date created for palm */
    int32_t _date_modif;    /* 40..43, date modified for palm */
    char __pad_zero_1[16];  /* 44..59, padding zero */
    char __ident_book[4];   /* 60..63, BOOK identifier */
    char signature[4];      /* 64..67, 'MTIT' for PDB, 'MTIU' for uPDB */
    char __pad_zero_2[8];   /* 68..75, padding zero */
    int16_t num_records;    /* 76..77, number of records */
} pdb_header;

typedef struct _pdb_book_def {
    char* bookName;
} pdb_book_metadata;

typedef struct _pdb_file {
    pdb_header header;
    time_t date_created;
    time_t date_modified;
    int32_t* offsets;
} pdb_file;

int pdb_read_header(FILE*, pdb_file*);
int pdb_read_offsets(FILE*, pdb_file*);

#endif  /* _PDB_READER_H_ */
