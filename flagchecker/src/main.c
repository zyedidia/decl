#include <assert.h>
#include <argp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <elf.h>

#include "args.h"

bool
flag_verify(void *code, size_t size, uintptr_t addr);

static char doc[] = "flagcheck: DeCl flag verifier";

static char args_doc[] = "INPUT...";

static struct argp_option options[] = {
    { "help",           'h',               0,      0, "show this message", -1 },
    { "n",              'n',               "NUM",  0, "run the verifier n times (for benchmarking)" },
    { 0 },
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    struct Args *args = state->input;

    switch (key) {
    case 'h':
        argp_state_help(state, state->out_stream, ARGP_HELP_STD_HELP);
        break;
    case 'n':
        args->n = atoi(arg);
        break;
    case ARGP_KEY_ARG:
        if (args->ninputs < INPUTMAX)
            args->inputs[args->ninputs++] = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

static inline
long long unsigned time_ns()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts)) {
        exit(1);
    }
    return ((long long unsigned)ts.tv_sec) * 1000000000LLU +
        (long long unsigned)ts.tv_nsec;
}

static bool
verify(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "failed to open %s: %s\n", filename, strerror(errno));
        return false;
    }

    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), file) != sizeof(ehdr)) {
        fprintf(stderr, "failed to read ELF header: %s\n", strerror(errno));
        goto err;
    }

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0 || ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "not a valid 64-bit ELF file\n");
        goto err;
    }

    if (ehdr.e_machine != EM_X86_64) {
        fprintf(stderr, "ELF architecture is not x64\n");
        goto err;
    }

    size_t total = 0;

    long long unsigned t1 = time_ns();
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        if (fseek(file, ehdr.e_phoff + i * sizeof(Elf64_Phdr), SEEK_SET) != 0) {
            fprintf(stderr, "seek failed: %s\n", strerror(errno));
            goto err;
        }

        Elf64_Phdr phdr;
        if (fread(&phdr, 1, sizeof(phdr), file) != sizeof(phdr)) {
            fprintf(stderr, "read failed: %s\n", strerror(errno));
            goto err;
        }

        if (phdr.p_type == PT_LOAD && (phdr.p_flags & PF_X)) {
            void *segment = malloc(phdr.p_filesz);
            if (!segment) {
                fprintf(stderr, "error: out of memory\n");
                goto err;
            }

            if (fseek(file, phdr.p_offset, SEEK_SET) != 0) {
                fprintf(stderr, "seek failed: %s\n", strerror(errno));
                free(segment);
                goto err;
            }

            if (fread(segment, 1, phdr.p_filesz, file) != phdr.p_filesz) {
                fprintf(stderr, "read failed: %s\n", strerror(errno));
                free(segment);
                goto err;
            }

            if (!flag_verify(segment, phdr.p_filesz, phdr.p_vaddr)) {
                fprintf(stderr, "verification failed\n");
                return false;
            }

            total += phdr.p_filesz;

            free(segment);
        }
    }
    long long unsigned elapsed = time_ns() - t1;

    fclose(file);
    printf("verification passed (%.1f MiB/s)\n", ((float) total / ((float) elapsed / 1000 / 1000 / 1000)) / 1024 / 1024);
    return true;
err:
    fclose(file);
    return false;
}

struct Args args;

int
main(int argc, char **argv)
{
    argp_parse(&argp, argc, argv, ARGP_NO_HELP, 0, &args);

    if (args.n == 0)
        args.n = 1;

    if (args.ninputs <= 0) {
        fprintf(stderr, "no input\n");
        return 0;
    }

    bool failed = false;
    for (size_t i = 0; i < args.ninputs; i++) {
        if (!verify(args.inputs[i]))
            failed = true;
    }
    if (failed)
        return 1;

    return 0;
}
