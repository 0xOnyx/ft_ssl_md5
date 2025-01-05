#ifndef FT_SSL_MD5_SHA256_H
# define FT_SSL_MD5_SHA256_H

# include <stdint.h>
# include <stddef.h>

# define SHA256_DIGEST_SIZE 32


typedef struct s_sha256_ctx {
    uint32_t state[8];
    uint64_t bitlen;
    unsigned char buffer[64];
} t_sha256_ctx;

void sha256_init(t_sha256_ctx *ctx);
void sha256_update(t_sha256_ctx *ctx, const unsigned char *data, size_t len);
void sha256_final(t_sha256_ctx *ctx, unsigned char digest[SHA256_DIGEST_SIZE]);

#endif //FT_SSL_MD5_SHA256_H
