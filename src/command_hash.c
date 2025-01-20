#include "ft_ssl.h"
#include "digest_ops.h"
#include "utils.h"
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>

static void read_and_hash_fd(int fd,
                             void *ctx,
                             const t_digest_ops *ops,
                             int echo_to_stdout){
    unsigned char buffer[4096];
    unsigned char temp_buffer[4096];
    ssize_t bytes_read;

    if (echo_to_stdout)
        ft_putstr_fd(" (\"", 1);
    while((bytes_read = read(fd, buffer, sizeof(buffer))) > 0){
        if (echo_to_stdout){
            ssize_t temp_index = 0;
            for (ssize_t i = 0; i < bytes_read; i++) {
                if (buffer[i] != '\n') {
                    temp_buffer[temp_index++] = buffer[i];
                }
            }
            if (temp_index > 0) {
                write(1, temp_buffer, temp_index);
            }
        }
        ops->update(ctx, buffer, bytes_read);
    }
    if (bytes_read < 0){
        ft_putstr_fd("ft_ssl: read failed\n", 2);
    }
    if (echo_to_stdout)
        ft_putstr_fd("\")= ", 1);
}


static void print_hex_digest(const unsigned char *digest, size_t digest_len){
    static const char *hex = "0123456789abcdef";

    char buff[128];
    size_t i;

    if (digest_len * 2 > sizeof(buff)){
        digest_len = sizeof(buff) / 2;
    }

    for (i = 0; i < digest_len; i++){
        buff[i * 2] = hex[(digest[i] >> 4) & 0xf];
        buff[i * 2 + 1] = hex[digest[i] & 0xf];
    }

    write(1, buff, digest_len * 2);
}

static void hash_stdin(int echo_to_stdout,
                       int label_stdin,
                       const t_hash_flags *flags,
                       const t_digest_ops *ops){
    void *ctx = malloc(ops->context_size);
    if (!ctx){
        ft_putstr_fd("ft_ssl: malloc failed\n", 2);
        return;
    }

    ops->init(ctx);
    read_and_hash_fd(0, ctx, ops, echo_to_stdout);

    unsigned char digest[64];
    ops->final(ctx, digest);

    free(ctx);
    if (flags->q){
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd("\n", 1);
    }
    else if (flags->r){
        print_hex_digest(digest, ops->digest_size);
        if (label_stdin)
            ft_putstr_fd(" (stdin)\n", 1);
        ft_putstr_fd("\n", 1);
    }
    else {
        if (label_stdin)
            ft_putstr_fd("(stdin)= ", 1);
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd("\n", 1);
    }
}

static void hash_string(const char *str,
                        const t_hash_flags *flags,
                        const t_digest_ops *ops){
    void *ctx = malloc(ops->context_size);

    if (!ctx){
        ft_putstr_fd("ft_ssl: malloc failed\n", 2);
        return;
    }

    ops->init(ctx);
    ops->update(ctx, (const unsigned char *)str, ft_strlen(str));

    unsigned char digest[64];
    ops->final(ctx, digest);
    free(ctx);

    if (flags->q)
    {
        print_hex_digest(digest, ops->digest_size);
        write(1, "\n", 1);
    }
    else if (flags->r)
    {
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd(" \"", 1);
        ft_putstr_fd(str, 1);
        ft_putstr_fd("\"\n", 1);
    }
    else
    {
        ft_putstr_fd((char*)ops->cmd_label, 1);
        ft_putstr_fd(" (\"", 1);
        ft_putstr_fd(str, 1);
        ft_putstr_fd("\") = ", 1);
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd("\n", 1);
    }
}

static int hash_file(const char *filename, const t_hash_flags *f, const t_digest_ops *ops){
    int fd = open(filename, O_RDONLY);

    if (fd < 0){
        ft_putstr_fd("ft_ssl: ", 2);
        ft_putstr_fd((char *)ops->cmd_name, 2);
        ft_putstr_fd(": ", 2);
        ft_putstr_fd(filename, 2);
        ft_putstr_fd(": No such file or directory\n", 2);
        return 1;
    }

    void *ctx = malloc(ops->context_size);
    if (!ctx){
        close(fd);
        ft_putstr_fd("ft_ssl: malloc failed\n", 2);
        return 1;
    }

    ops->init(ctx);
    read_and_hash_fd(fd, ctx, ops, 0);
    close(fd);

    unsigned char digest[64];

    ops->final(ctx, digest);
    free(ctx);

    if (f->q)
    {
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd("\n", 1);
    }
    else if (f->r)
    {
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd(" ", 1);
        ft_putstr_fd(filename, 1);
        ft_putstr_fd("\n", 1);
    }
    else
    {
        ft_putstr_fd((char*)ops->cmd_label, 1);
        ft_putstr_fd(" (", 1);
        ft_putstr_fd(filename, 1);
        ft_putstr_fd(") = ", 1);
        print_hex_digest(digest, ops->digest_size);
        ft_putstr_fd("\n", 1);
    }
    return 0;
}


int hash_command(const void *ops_void, int argc, char **argv){
    const t_digest_ops *ops = (const t_digest_ops *)ops_void;

    t_hash_flags flags = {0};

    int i = 2;
    int file_count = 0;
    int used_stdin_for_p = 0;

    for (; i < argc; i++){
        if (argv[i][0]  == '-' && file_count == 0){
            int j = 1;
            while (argv[i][j]) {
                if (argv[i][j] == 'p') {
                    flags.p = 1;
                    hash_stdin(1, 0, &flags, ops);
                    used_stdin_for_p = 1;
                } else if (argv[i][j] == 'q') { flags.q = 1; }
                else if (argv[i][j] == 'r') { flags.r = 1; }
                else if (argv[i][j] == 's') {
                    flags.s = 1;
                    if (i + 1 < argc) {
                        hash_string(argv[i + 1], &flags, ops);
                        i++;
                    } else {
                        /* Error: no string after -s */
                        ft_putstr_fd("ft_ssl: ", 2);
                        ft_putstr_fd((char *) ops->cmd_name, 2);
                        ft_putstr_fd(": -s: No such file or directory\n", 2);
                        return (1);
                    }
                    break;
                }
                else {
                    /* Error: unknown flag */
                    ft_putstr_fd("ft_ssl: ", 2);
                    ft_putstr_fd((char *) ops->cmd_name, 2);
                    ft_putstr_fd(": -", 2);
                    ft_putstr_fd(argv[i] + j, 2);
                    ft_putstr_fd(": unknown option\n", 2);
                    return (1);
                }
                j++;
            }
        }
        else {
            hash_file(argv[i], &flags, ops);
            file_count++;
        }
    }

    if (file_count == 0 && !used_stdin_for_p && !flags.s){
        hash_stdin(0, 1, &flags, ops);
    }

    return (0);
}