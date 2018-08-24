#include "reader.h"

#include <stdlib.h>
#include <byteswap.h>

static time_t __pdb_interpret_date(int32_t ts) {
    const int EPOCH_1904_DIFF = 2082844800L;

    // the trick from https://en.wikipedia.org/wiki/PDB_(Palm_OS)
    if (ts & (1 << 31)) {
        // top bit set, it is an unsigned 1904 ts
        return (time_t) (((uint64_t) ts) + EPOCH_1904_DIFF);
    }
    // top bit unset, it is a (signed) standard epoch
    return (time_t) ts;
}

int pdb_read_header(FILE* file, pdb_file* dest) {
    int rtn;
    rtn = fseek(file, 0, SEEK_SET);
    if (rtn) return -1;
    rtn = fread(dest, sizeof(*dest), 1, file);
    if (rtn <= 0) return -1;

    /* TODO: check signatures */

    /* convert endianness of numbers */
    pdb_header* hdr = &dest->header;
    hdr->_date_creat = bswap_32(hdr->_date_creat);
    hdr->_date_modif = bswap_32(hdr->_date_modif);
    hdr->num_records = bswap_16(hdr->num_records);

    /* interpret dates */
    dest->date_created = __pdb_interpret_date(hdr->_date_creat);
    dest->date_modified = __pdb_interpret_date(hdr->_date_modif);

    return 0;
}

int pdb_read_offsets(FILE* file, pdb_file* dest) {
    int rtn;
    rtn = fseek(file, PDB_HEADER_SIZE, SEEK_SET);
    if (rtn) return -1;

    struct {
        uint32_t offs;
        char __pad_zero[4];
    } buf;

    int nmemb = dest->header.num_records;
    int nread = 0;

    dest->offsets = (int32_t*) malloc(nmemb * sizeof(int32_t));
    for (int i = 0; i < nmemb; i++) {
        rtn = fread(&buf, sizeof(buf), 1, file);
        if (rtn <= 0) break;
        nread += rtn;

        dest->offsets[i] = bswap_32(buf.offs);
    }
    return nread;
}
