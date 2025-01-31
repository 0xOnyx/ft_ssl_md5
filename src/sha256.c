#include "sha256.h"

#define ROTR(word, bits)  (((word) >> (bits)) | ((word) << (32 - (bits))))
#define CH(x, y, z)           (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJ(x, y, z)          (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)                (ROTR(x, 2)  ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x)                (ROTR(x, 6)  ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x)               (ROTR(x, 7)  ^ ROTR(x, 18) ^ ((x) >> 3))
#define SIG1(x)               (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))


static const uint32_t g_sha256_k[64] =
        {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
                0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
                0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
                0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
                0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
                0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
                0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
                0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
                0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

static void sha256_transform(t_sha256_ctx *ctx, const unsigned char *block){
    uint32_t m[64];
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1, t2;
    int i;

    for (i = 0; i < 16; i++){ //big endian
        m[i] = (uint32_t)block[i * 4] << 24 |
               (uint32_t)block[i * 4 + 1] << 16 |
               (uint32_t)block[i * 4 + 2] << 8 |
               (uint32_t)block[i * 4 + 3];
    }

    for (i = 16; i < 64; i++){
        uint32_t s0 = SIG0(m[i - 15]);
        uint32_t s1 = SIG1(m[i - 2]);
        m[i] = m[i - 16] + s0 + m[i - 7] + s1;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0; i < 64; i++){
        t1 = h + EP1(e) + CH(e, f, g) + g_sha256_k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h  = g;
        g  = f;
        f  = e;
        e  = d + t1;
        d  = c;
        c  = b;
        b  = a;
        a  = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(t_sha256_ctx *ctx){
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(t_sha256_ctx *ctx, const unsigned char *data, size_t len){
    size_t i;
    size_t index;
    size_t to_fill;

    index = (ctx->bitlen / 8) % 64;
    to_fill = 64 - index;
    ctx->bitlen += (uint64_t)len * 8;

    i = 0;
    if (len >= to_fill){
        size_t j;

        for (j = 0; j < to_fill; j++){
            ctx->buffer[index + j] = data[j];
        }

        sha256_transform(ctx, ctx->buffer);

        for (i = to_fill;  i + 63 < len; i += 64){
            sha256_transform(ctx, &data[i]);
        }

        index = 0;
    }

    for (; i < len ; i++){
        ctx->buffer[index++] = data[i];
    }
}

void sha256_final(t_sha256_ctx *ctx, unsigned char digest[SHA256_DIGEST_SIZE]){
    static unsigned char padding[64] = {0x80};
    unsigned char length_le[8];
    size_t index;
    size_t pad_len;
    int i;

    index = (ctx->bitlen / 8) % 64;

    uint64_t bitlen = ctx->bitlen;
    for (i = 7; i >= 0; i--){
        length_le[i] = bitlen & 0xff;
        bitlen >>= 8;
    }

    pad_len = (index < 56) ? (56 - index) : (64 + 56 - index);

    sha256_update(ctx, padding, pad_len);

    sha256_update(ctx, length_le, 8);

    for (i = 0; i < 8; i++){ //big endian
        digest[i * 4 + 0] = (unsigned char)((ctx->state[i] >> 24) & 0xff);
        digest[i * 4 + 1] = (unsigned char)((ctx->state[i] >> 16) & 0xff);
        digest[i * 4 + 2] = (unsigned char)((ctx->state[i] >>  8) & 0xff);
        digest[i * 4 + 3] = (unsigned char)((ctx->state[i]      ) & 0xff);
    }
}