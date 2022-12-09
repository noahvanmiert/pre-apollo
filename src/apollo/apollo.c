/*
    Made by Noah Van Miert
    8/12/2022

    Apollo Compiler
*/

#include "apollo.h"

#include "../core.h"
#include "../parser/logging/logging.h"
#include "../parser/parser.h"
#include "../compiler/compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static struct ArgManager args = {
    .filepath = NULL,
    .help_f = 0,
    .run_f = 0,
    .asm_f = 0
};


static void print_help()
{
    printf("Apollo usage: apollo [FLAGS] <FILENAME>\n");
    printf("Flags:\n");
    printf("    -h    Prints this help message.\n");
    printf("    -r    Runs the program after compiling.\n");
    printf("    -asm  Compile to an assembly file.\n");
}


static void parse_flag(const char *flag)
{
    if (strcmp(flag, "-h") == 0) {
        args.help_f = 1;

        print_help();
        exit(0);
    }

    if (strcmp(flag, "-r") == 0) {
        args.run_f = 1;
        return;
    }

    if (strcmp(flag, "-asm") == 0) {
        args.asm_f = 1;
        return;
    }

    apo_error("ERROR: unkown flag '%s'\n", flag);
}


static void parse_filepath(const char *filepath)
{
    if (args.filepath == NULL) {
        args.filepath = filepath;
    } else {
        apo_error("ERROR: multiple files provided");
    }
}


struct ArgManager parse_args(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        // parse a flag
        if (argv[i][0] == '-')
            parse_flag(argv[i]);
        else
            parse_filepath(argv[i]);
    }

    if (args.filepath == NULL)
        apo_error("ERROR: no file provided");

    return args;
}


void run_assembler(struct ArgManager args)
{
    if (!args.asm_f) {
        if (system("nasm -felf64 -o output.o output.asm"))
            apo_error("ERROR: could not assemble output file");

        if (system("ld -o output output.o"))
            apo_error("ERROR: could not link object file");

        system("chmod +x output");

        if (args.run_f) {
            system("./output");
        }
    }
}


void write_assembly(const char *data)
{
    FILE *f = fopen("output.asm", "w");
    fputs(data, f);
    fclose(f);
}


void apollo_start(int argc, char **argv)
{
    struct ArgManager args = parse_args(argc, argv);
    struct Lexer *lexer = create_lexer("Examples/hello_world.apo");
    struct Parser *parser = create_parser(lexer);
    struct Ast *root = parser_parse(create_scope(), parser);
    printf("REACHED\n");

    free(lexer);
    free(parser);

    const char *_nasm = nasm_compile(root);

    free(root);

    write_assembly(_nasm);
    run_assembler(args);
}