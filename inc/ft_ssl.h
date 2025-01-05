#ifndef FT_SSL_MD5_FT_SSL_H
#   define FT_SSL_MD5_FT_SSL_H


typedef struct s_hash_flags {
    int p;
    int q;
    int r;
    int s;
} t_hash_flags;

/* Forward declarations */
int hash_command(const void *ops_void, int argc, char **argv);

#endif