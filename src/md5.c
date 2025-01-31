#include "md5.h"

#define F(x, y, z) ((x & y) | ((~x) & z))
#define G(x, y, z) ((x & z) | (y & (~z)))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (x | (~z)))


#define LEFT_ROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

/*
** Per-round shift amounts.
*/
static const uint32_t g_md5_r[64] =
        {
                7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
                5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
                4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
                6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
        };


static const uint32_t g_md5_k[64] =
        {
                0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

                0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

                0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

                0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
        };

void md5_init(t_md5_ctx *ctx){
    ctx->bitlen = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
}

static void md5_transform(t_md5_ctx *ctx, const unsigned char *block){
    uint32_t w[16];
    uint32_t a, b, c, d;
    uint32_t f, g;
    uint32_t temp;
    int i;

    for (i = 0; i < 16; i++){ //little-endian
        w[i] = (uint32_t)block[i * 4] |
               (uint32_t)block[i * 4 + 1] << 8 |
               (uint32_t)block[i * 4 + 2] << 16 |
               (uint32_t)block[i * 4 + 3] << 24;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];

    for (i = 0; i < 64; i++){
        if (i < 16){
            f = F(b, c, d);
            g = i;
        }
        else if (i < 32){
            f = G(b, c, d);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48){
            f = H(b, c, d);
            g = (3 * i + 5) % 16;
        }
        else{
            f = I(b, c, d);
            g = (7 * i) % 16;
        }
        temp = d;
        d = c;
        c = b;
        b = b + LEFT_ROTATE(
                (a + f + g_md5_k[i] + w[g]), g_md5_r[i]);
        a = temp;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
}


void md5_update(t_md5_ctx *ctx, const unsigned char *data, size_t len){
    size_t i;
    size_t index;
    size_t to_fill;

    index = (ctx->bitlen / 8) % 64;
    ctx->bitlen += (uint64_t)len * 8;

    to_fill = 64 - index;

    i = 0;
    if (len >= to_fill){

        size_t j;

        for (j = 0; j < to_fill; j++){
            ctx->buffer[index + j] = data[j];
        }

        md5_transform(ctx, ctx->buffer);

        for (i = to_fill; i + 63 < len; i += 64){
            md5_transform(ctx, data + i);
        }
        index = 0;
    }

    for (; i < len; i++){
        ctx->buffer[index++] = data[i];
    }
}

void md5_final(t_md5_ctx *ctx, unsigned char digest[MD5_DIGEST_SIZE]){
    static unsigned char    padding[64] = {0x80};
    unsigned char           lenght_le[8];
    size_t                  index;
    size_t                  pad_len;
    int                     i;


    for (i = 0; i < 4; i++){
        lenght_le[i] = (ctx->bitlen >> (i * 8)) & 0xff;
    }

    index = (ctx->bitlen / 8) % 64;

    pad_len = (index < 56) ? (56 - index) : (64 + 56 - index);

    md5_update(ctx, padding, pad_len);

    md5_update(ctx, lenght_le, 8);


    for (i = 0; i < 4; i++){ //little endian
        digest[i]      = (unsigned char)((ctx->state[0] >> (8 * i)) & 0xff);
        digest[i + 4]  = (unsigned char)((ctx->state[1] >> (8 * i)) & 0xff);
        digest[i + 8]  = (unsigned char)((ctx->state[2] >> (8 * i)) & 0xff);
        digest[i + 12] = (unsigned char)((ctx->state[3] >> (8 * i)) & 0xff);
    }
}