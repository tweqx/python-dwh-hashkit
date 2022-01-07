# dwh_hashkit

dwh_hashkit is the Python port of the 'dwh_hashkit' C library.

To install it, clone the repository in your project under the name `dwh_hashkit`. Then, you can import it with `from dwh_hashkit import dwh_hashkit`, for instance.

## Library
This module exports two functions, `check` and `hashes`. They take a bytestring object as argument and respectively returns the algorithm name matching the Deep Web Hash (or None) and a dictionnary giving the computed hash for each algorithm.

The `Hashbox` class has three methods: `update`, `final` and `final_hashes`.

For more information, use `help(dwh_hashkit)` to see the module documentation.

## Example Code:
```py
hashbox = Hashbox()

hashbox.update(b'dwh_hash')
hashbox.update(b'kit')

match = hashbox.final()
if match:
  print(f'Found the DWH using algorithm {match}')
else:
  print(":(")
```

## Supported hashing algorithms
 * SHA-512
 * BLAKE2b
 * Streebog
 * SHA-3
 * FNV-0/FNV-1/FNV-1a
 * Grøstl
 * MD6
 * JH
 * BLAKE-512
 * LSH
 * Skein
 * Keccak3
 * CubeHash
 * Whirlpool-0/Whirlpool-T/Whirlpool

## License
[GPLv3](https://www.gnu.org/licenses/gpl-3.0.html)
