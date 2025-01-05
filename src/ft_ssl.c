#include "ft_ssl.h"
#include "digest_ops.h"
#include "md5.h"
#include "sha256.h"
#include "utils.h"


static void print_usage(void)
{
    const char *msg =
            "usage: ft_ssl command [flags] [file/string]\n\n"
            "Commands:\n"
            "md5\n"
            "sha256\n\n"
            "Flags:\n"
            "-p -q -r -s\n";

    ft_putstr_fd(msg, 1);
}

/*
 * Set up the array of available hash functions
 * Each entry include:
 *  - name (e.g. md5, sha256)
 *  - label (e.g. MD5, SHA256)
 *  - function pointer to init / update / final
 *  - digest size
 *  - context struct size
 *
 *  the last entry is "sentinel" with cmd_name = NULL
 */
static t_digest_ops g_hash_ops[] = {
        {
            /* MD5 */
            .cmd_name       = "md5",
            .cmd_label      = "MD5",
            .init           = (void (*)(void *))md5_init,
            .update         = (void (*)(void *, const unsigned char *, size_t))md5_update,
            .final          = (void (*)(void *, unsigned char *))md5_final,
            .digest_size    = MD5_DIGEST_SIZE,
            .context_size   = sizeof(t_md5_ctx)
        },
//        {
//            /* SHA256 */
//            .cmd_name       = "sha256",
//            .cmd_label    = "SHA256",
//            .init           = (void (*)(void *))sha256_init,
//            .update         = (void (*)(void *, const unsigned char *, size_t))sha256_update,
//            .final          = (void (*)(void *, unsigned char *))sha256_final,
//            .digest_size    = SHA256_DIGEST_SIZE,
//            .context_size   = sizeof(t_sha256_ctx)
//        },
        {
            /* Sentinel (no command name) */
            .cmd_name =  NULL,
            .cmd_label = NULL,
            .init = NULL,
            .update = NULL,
            .final = NULL,
            .digest_size = 0,
            .context_size= 0
        }
};

int main(int argc, char **argv){
    if (argc < 2){
         print_usage();
         return 1;
    }

    for (int i = 0; g_hash_ops[i].cmd_name != NULL; i++){
        if (ft_strcmp(argv[1], g_hash_ops[i].cmd_name) == 0){
            return hash_command((const void *)&g_hash_ops[i], argc, argv);
        }
    }

    ft_putstr_fd("ft_ssl: Error: '", 2);
    ft_putstr_fd(argv[1], 2);
    ft_putstr_fd("' is an invalid command.\n\n", 2);
    print_usage();

    return 1;
}
