#ifndef FT_SSL_MD5_MD5_H
# define FT_SSL_MD5_MD5_H

# include <stddef.h>
# include <stdint.h>

# define MD5_DIGEST_SIZE 16


typedef struct s_md5_ctx{
    uint32_t state[4];
    uint64_t bitlen;
    unsigned char buffer[64];
} t_md5_ctx;

void md5_init(t_md5_ctx *ctx);
void md5_update(t_md5_ctx *ctx, const unsigned char *data, size_t len);
void md5_final(t_md5_ctx *ctx, unsigned char digest[MD5_DIGEST_SIZE]);

#endif //FT_SSL_MD5_MD5_H
