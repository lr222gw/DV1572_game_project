DIRECTORY LEGEND:
  bin      --  (binaries)      built executables & DLLs
  dat      --  (data)          data/assets
  inc      --  (include)       library include headers
  res      --  (resources)     DLLs, etc (to be copied to /bin/ during building)
  src      --  (source)        source code
  src/misc --                  misc source code (for really small libraries)
  tmp      --  (temporary)     object files, etc
  lib      --  (library)       library files
  doc      --  (documents)     documentation text files, UML diagrams, etc

MAKEFILE COMMANDS:
    make all        (makes binary and copies DLLs)
    make remake     (full cleans and makes)
    make clean      (removes binaries)
    make cleaner    (removes binaries and object files)
