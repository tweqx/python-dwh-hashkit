from distutils.core import setup, Extension
import shutil, os

def main():
    dwh_hashkit_files = [
        "dwh_hashkit/skein/skein.c",
        "dwh_hashkit/skein/skein_block.c",
        "dwh_hashkit/skein/SHA3api_ref.c",
        "dwh_hashkit/fnv512/fnv512.c",
        "dwh_hashkit/fnv512/uint512.c",
        "dwh_hashkit/lsh/sse2/lsh512_sse2.c",
        "dwh_hashkit/lsh/lsh.c",
        "dwh_hashkit/lsh/no_arch/lsh512.c",
        "dwh_hashkit/lsh/avx2/lsh512_avx2.c",
        "dwh_hashkit/lsh/xop/lsh512_xop.c",
        "dwh_hashkit/lsh/ssse3/lsh512_ssse3.c",
        "dwh_hashkit/blake512/blake512.c",
        "dwh_hashkit/lp_hash.c",
        "dwh_hashkit/keccak3/keccak3.c",
        "dwh_hashkit/whirlpool/Whirlpool_constants_T.c",
        "dwh_hashkit/whirlpool/Whirlpool_constants_0.c",
        "dwh_hashkit/whirlpool/Whirlpool_constants.c",
        "dwh_hashkit/whirlpool/Whirlpool.c",
        "dwh_hashkit/streebog/gost3411-2012-core.c",
        "dwh_hashkit/md6/md6_mode.c",
        "dwh_hashkit/md6/md6_compress.c",
        "dwh_hashkit/jh/jh.c",
        "dwh_hashkit/grostl/grostl.c",
        "dwh_hashkit/cubehash/cubehash.c",
        "dwh_hashkit/hashbox.c",
    ]

    setup(
        name="dwh_hashkit_c",
        version="1.0.0",
        description="Python module to check if an input hashes to the deep web hash",
        author="tweqx",
        ext_modules=[
            Extension(
                "dwh_hashkit_c",
                dwh_hashkit_files + ["module_main.c"],
                extra_link_args=[
                    "-lcrypto",
                    "-lssl",
                ],
            )
        ],
    )

    # Copy output .so to parent directory
    lib_folder = next(f for f in os.listdir("./build") if f.startswith('lib.'))
    so_file = next(f for f in os.listdir(f"./build/{lib_folder}") if f.endswith('.so'))

    shutil.copy(f"./build/{lib_folder}/{so_file}", f"../{so_file}")

if __name__ == "__main__":
    main()
