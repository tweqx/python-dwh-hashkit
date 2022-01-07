"""Easily check if data hashes to the Deep Web Hash, using many hashing algorithms efficiently written.

This module is the Python port of the C library 'dwh_hashkit'. The 18 512-bit hashing algorithms are:
 SHA-512, BLAKE2b, Streebog, SHA-3, FNV-0, FNV-1, FNV-1a, Grøstl, MD6, JH, BLAKE-512, Skein, Keccak-3, Cubehash, Whirlpool, Whirlpool-T and Whirlpool-0.

  Typical usage example:

  hashbox = Hashbox()

  hashbox.update(b'dwh_hash')
  hashbox.update(b'kit')

  match = hashbox.final()
  if match:
    print(f'Found the DWH using algorithm {match}')
  else:
    print(":(")

"""

import dwh_hashkit_c

_algorithms_names = ['sha512', 'blake2b', 'streebog', 'sha3', 'fnv0', 'fnv1', 'fnv1a', 'grostl', 'md6', 'jh', 'blake512', 'lsh', 'skein', 'keccak3', 'cubehash', 'whirlpool', 'whirlpool0', 'whirlpoolT']

class Hashbox:
  """Allows to check if an input hashes to the Deep Web Hash using 18 hashing algorithms

  One can hash an input separated into chunks, by creating a Hashbox object and using the 'update' and 'final' methods.
  All hashes can be retrieved by calling 'final_hashes'.

  The lifecycle of a Hashbox object should be: possibly several calls to 'update' with the data to hash followed by a single call to 'final' or 'final_hashes'.
  Once the hashbox object has been finalized, subsequent calls to method will raise an Error.
  """

  def __init__(self):
    """Inits Hashbox"""
    self._hashbox = dwh_hashkit_c.new()

  def update(self, data):
    """Appends 'data' to the hashbox

    Args:
      data:
        A byte-like object to append to the hashbox

    Raises:
      RuntimeError: The hashbox object is already finalized
    """
    dwh_hashkit_c.update(self._hashbox, data)

  def final(self):
    """Produces output hashes and compares them against the Deep Web Hash.

    Returns:
      None when the Deep Web Hash is not found, or the matching algorithm name otherwise.

    Raises:
      RuntimeError: The hashbox object is already finalized
    """
    match = dwh_hashkit_c.final(self._hashbox)

    if match != 0: # != HASHBOX_NO_MATCH
      return _algorithms_names[match - 1]

  def final_hashes(self):
    """Produces output hashes and returns them in a dictionnary

    Returns:
      A dict with algorithm names as keys and 512-bit hexdigests as values.
      The algorithm names are: sha512, blake2b, streebog, sha3, fnv0, fnv1, fnv1a, grostl, md6
       jh, blake512, lsh, skein, cubehash, whirlpool, whirlpool0 and whirlpoolT

    Raises:
      RuntimeError: The hashbox object is already finalized
    """
    hashes = dwh_hashkit_c.final_hashes(self._hashbox)

    return { name: hash.hex() for name, hash in hashes.items() }

def check(data):
  """Checks if data hashes to the Deep Web Hash.

  Returns:
    None when the Deep Web Hash is not found, or the matching algorithm name otherwise.
  """
  hashbox = Hashbox()
  hashbox.update(data)
  return hashbox.final()

def hashes(data):
  """Returns all hashes for data.

  Returns:
    A dict with algorithm names as keys and 512-bit hexdigests as values.
    The algorithm names are: sha512, blake2b, streebog, sha3, fnv0, fnv1, fnv1a, grostl, md6
     jh, blake512, lsh, skein, cubehash, whirlpool, whirlpool0 and whirlpoolT
  """
  hashbox = Hashbox()
  hashbox.update(data)
  return hashbox.final_hashes()
