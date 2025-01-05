# ft_ssl_md5
recode part of the OpenSSL program, specifically the MD5 and SHA-256 hashing algorithms


# Stucture

```
.
├── Makefile
├── ft_ssl.c           (entry point + dispatch)
├── ft_ssl.h           (common declarations)
├── command_hash.c     (hash_command() + chunk-based reading + flags logic)
├── digest_ops.h       (defines t_digest_ops struct)
├── md5.c / md5.h      (MD5 init, update, final)
├── sha256.c / sha256.h (SHA256 init, update, final)
└── ...
```

# Usage

```
$> make
$> ./ft_ssl md5 [-pqr] [-s string] [files ...]
$> echo "pickle rick" | ./ft_ssl md5 -p
$> ./ft_ssl md5 -s "pickle rick"
$> ./ft_ssl md5 -r file
$> ./ft_ssl md5 -q file
$> ./ft_ssl md5 file
```


# md5 functioning

Preprocessing:
```mathematica
Original message --> Padding --> Append Length --> Blocks of 512 bits
```

Initialisation:
```mathematica
A, B, C, D = Initial Constants
```

Processing:
```mathematica
Apply 64 steps with F, G, H, I functions and update A, B, C, D
```

Output:
```mathematica
Final hash = A || B || C || D
```


# Resources
https://en.wikipedia.org/wiki/Merkle%E2%80%93Damg%C3%A5rd_construction

## MD5
https://en.wikipedia.org/wiki/MD5
https://tools.ietf.org/html/rfc1321


## SHA-256
https://en.wikipedia.org/wiki/SHA-2
https://tools.ietf.org/html/rfc4634
```