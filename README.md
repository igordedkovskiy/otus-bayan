# homework-07: otus-bayan
    find identical files

## Help
    -h    help
    -i    folders to discover for files
    -o    folders to ommit
    -d    depth of discovering
    -s    minimal size of a file to consider in bytes
    -r    masks of file names to consider. Mask is a regular expression
    -b    size of a block to read from file
    -H    hash algorithm: std::hash, md5, crc32
    
## Example
    bayan -i tests -H md5 -d 5 -b 20 -r (.*pic18f.*)
    App will search for files in the folder "tests" recursively with depth of recursion 5.
    Hash algorithm is "md5", size of block of a file is 20 bytes.
    (.*pic18f.*) - name of file to consider must contain a string "pic18f".
    
