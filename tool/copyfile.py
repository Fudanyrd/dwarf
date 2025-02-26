#!/usr/bin/python3

import subprocess

FUNCCOPY = "/home/liuyu/Desktop/dwarf/funccopy"

def process_file(file: str) -> int:
  """ Run funccopy on a file repeatedly until all functions are copied. """
  func_count = 0

  while True:
    res = subprocess.run(
      f"{FUNCCOPY} {file} {func_count}",
      shell=True,
    )

    if res.returncode == 0:
      func_count += 1
    else:
      if res.returncode != 2:
        print(file, ": Something went wrong")
      else:
        print(file, ": OK")
      break

  return func_count

if __name__ == "__main__":
  import sys
  for i in range(1, len(sys.argv)):
    process_file(sys.argv[i])
