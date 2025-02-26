#!/usr/bin/python3

import subprocess
import sys
import os 
import sqlite3
import json

# a sample configuration
config = {
  "funccopy": "/mnt/d/huge-pj/dwarf4/funccopy",
  "project": "bison-3.8",
  "dir": "/mnt/d/huge-pj/dwarf4/tmp/bison-3.8/",
  "database": "/mnt/d/huge-pj/dwarf4/tmp/bison.db",
}

if __name__ == "__main__":

  if len(sys.argv) == 2:
    with open(sys.argv[1], 'r') as fobj:
      config = json.load(fobj)

  db = config['database']
  if os.path.exists(db):
    os.system(f"rm {db}")
  
  conn = sqlite3.connect(db)

  # create two tables.
  _ = conn.execute(
"""
CREATE TABLE fs(
  project char(64),
  path varchar(1024),
  file_id int
);
"""
  )
  _ = conn.execute(
"""
CREATE TABLE code(
  file_id int,
  func_id int,
  start int,
  end int,
  src TEXT 
);
"""
  )
  conn.commit()

  # find name of all c source files
  os.chdir(config['dir'])
  find_res = subprocess.run(
    "find -name \\*.c",
    shell=True,
    capture_output=True
  )
  files = find_res.stdout.decode().split('\n')

  # remove empty pathname
  tmp_lst = []
  for file in files:
    if file.strip() != '':
      tmp_lst.append(file)
  del files 
  files = tmp_lst

  # insert into fs 
  pj = config["project"]
  dir = config['dir']
  file_id = 0
  for file in files:
    tpl = {
      "project": pj,
      "path": os.path.join(dir, file),
      "file_id": file_id,
    }
    _ = conn.execute("INSERT INTO fs VALUES(:project, :path, :file_id)", (tpl))
    conn.commit()
    file_id += 1

  for i in range(len(files)):
    file = files[i]
    func_id = 0

    while True:
      fcpy_res = subprocess.run(
        f"{config['funccopy']} {os.path.join(dir, file)} {func_id}",
        shell=True, 
        capture_output=True,
      )

      # execution error.
      if fcpy_res.returncode != 0:
        if fcpy_res.returncode != 2:
          print(file, "Something went wrong. WARN")
        break

      lines = fcpy_res.stdout.decode().split('\n')
      first = lines[0]
      try:
        words = first.split()
        st, ed = int(words[0]), int(words[1])
      except Exception:
        pass
      code = '\n'.join(lines[1:])

      tpl = {
        "file_id": i,
        "func_id": func_id,
        "start": st,
        "end": ed,
        "src": code,
      }

      # execute 
      _ = conn.execute("INSERT INTO code VALUES(:file_id, :func_id, :start, :end, :src)", (tpl))
      conn.commit()

      # advance
      func_id += 1
      del tpl
      del code
      del lines 

  conn.close()
