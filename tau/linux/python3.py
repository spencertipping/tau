#!/usr/bin/env python3
# τ-python UTF9 protocol driver
#
# Supports a subset of UTF9 data structures: int, float, bool, utf8, bytes,
# tuple, set, map. Input can be stream-assembled, output is serialized from
# python structures.


from collections import deque
from struct      import pack, unpack


class utf9_reader:
  def __init__(self):
    self.buf    = b''
    self.parsed = deque()  # TODO: remove this

  def has_next(self): return len(self.parsed)
  def next(self):     return self.parsed.popleft()

  def __lshift__(self, x):
    self.buf += x


# TODO: UTF9 writer
