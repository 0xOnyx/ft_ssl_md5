#ifndef FT_SSL_MD5_DIGEST_OPT_H
# define FT_SSL_MD5_DIGEST_OPT_H

# include <stddef.h> // for size_t

typedef struct s_digest_ops {
    const char *cmd_name; // md5, sha256, sha224, sha512, sha384
    const char *cmd_label; // MD5, SHA256, SHA224, SHA512, SHA384
    void (*init)(void *ctx);
    void (*update)(void *ctx, const unsigned char *data, size_t len);
    void (*final)(void *ctx, unsigned char *digest);
    size_t digest_size;
    size_t context_size;
} t_digest_ops;

#endif //FT_SSL_MD5_DIGEST_OPT_H
