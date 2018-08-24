#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "cmdline.h"
#include "reader.h"

int main(int argc, const char* argv[]) {
    struct arguments cmdline_args = {};

    if (parse_args(argc, (char**) argv, &cmdline_args)) {
        perror("argp");
        return 1;
    }

    const char* filename = cmdline_args.args[0];

    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error opening file \"%s\": %s\n", filename, strerror(errno));
        return 1;
    }

    pdb_file pdb_container;

    if (pdb_read_header(f, &pdb_container)) {
        perror("pdb_read_header");
        return 1;
    }
    pdb_header* hdr = &pdb_container.header;

    printf("--- metadata\n");

    char s1[128], s2[128];
    struct tm ts1 = *localtime(&pdb_container.date_created),
              ts2 = *localtime(&pdb_container.date_modified);

    strftime(s1, 128, "%F %T", &ts1);
    strftime(s2, 128, "%F %T", &ts2);

    printf("meta str: [%.35s]\n", hdr->meta_str);
    printf("version: [%" PRIi8 "]\n", hdr->version);
    printf("date created: %s\n", s1);
    printf("date modified: %s\n", s2);
    printf("identity: [%.4s]\n", hdr->__ident_book);
    printf("signature: [%.4s]\n", hdr->signature);
    printf("# of records: [%" PRIi16 "]\n", hdr->num_records);
    puts("");

    int nread = pdb_read_offsets(f, &pdb_container);
    printf("Read offsets of %d/%d entries\n", nread, hdr->num_records);

    if (fseek(f, 0, SEEK_END) < 0) {
        perror("fseek");
        return 1;
    }
    long pos_end = ftell(f);
    if (pos_end < 0) {
        perror("ftell");
        return 1;
    }

    int lens[nread];
    char* blocks[nread];

    for (int i = 0; i < nread; i++) {
        int offs = pdb_container.offsets[i];
        if (i == nread - 1) {
            lens[i] = pos_end - offs;
        } else {
            lens[i] = pdb_container.offsets[i + 1] - offs;
        }

        if (fseek(f, offs, SEEK_SET)) {
            perror("fseek");
            break;
        }
        blocks[i] = (char*) malloc(lens[i]);
        fread(blocks[i], lens[i], 1, f);

        // printf("#%3d: pos %6d, len %6d: %s\n", i, offs, data_len, "");
    }

    printf("--- first block (TOC)\n");

    char* block = blocks[0];
    int state = 0;
    int pos = 8;
    while (pos < lens[0]) {
        if (block[pos] == 0x1b) {  // ESC
            printf("[ESC]\n");
            state++;
            pos++;
        } else {
            if (block[pos] < 0) {  // big5 word
                printf("%c%c", block[pos], block[pos + 1]);
                pos += 2;
            } else {
                printf("%c", block[pos]);
                pos++;
            }
        }
    }
    printf("[EOB]\n");

    for (int i = 1; i < nread; i++) {
        char fname[128];
        sprintf(fname, "book/%d.txt", i);
        FILE* outfile = fopen(fname, "w");
        if (!outfile) {
            fprintf(stderr, "Cannot create file \"%s\": %s\n", fname, strerror(errno));
            return 1;
        }
        fprintf(outfile, "%*s\n", lens[i],blocks[i]);
        fclose(outfile);
    }
}
