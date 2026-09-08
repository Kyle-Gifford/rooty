# Custom Archive Utility (archive.c information)

A recursive file archiving utility written in **C** for Linux that can package files and directory trees into a custom archive format and later reconstruct them.

The program implements its own lightweight archive format using length-prefixed file and directory names, file sizes, raw file contents, and directory-end markers. It uses standard Unix filesystem APIs to recursively traverse directories and rebuild their structure during extraction.

> This project is intended as a systems-programming exercise and educational archive format, not as a replacement for production tools such as `tar` or `zip`.

## Features

* Create archives containing regular files
* Recursively archive entire directory trees
* Extract archived files and directories
* Preserve nested directory structure
* Store raw file contents inside the archive
* Custom length-prefixed archive format
* Recursive directory traversal
* Recursive directory reconstruction
* Automatically create missing parent directories
* Skip unsupported/non-regular filesystem entries
* Implemented entirely in C using Unix/POSIX filesystem APIs
* No third-party libraries required

## Technologies

* **C**
* **Linux / Unix**
* POSIX filesystem APIs
* Standard C file I/O
* Recursive directory traversal

Key system and library interfaces used include:

```c
stat()
opendir()
readdir()
closedir()
chdir()
mkdir()
getcwd()
fopen()
fread()
fwrite()
fgetc()
fputc()
```

## How It Works

The program operates in one of two modes depending on the number of command-line arguments.

### Archive Creation

When multiple input paths are supplied, the final argument is treated as the output archive:

```bash
./archive FILE... OUTFILE
```

For example:

```bash
./archive documents archive.dat
```

or:

```bash
./archive file.txt archive.dat
```

The program recursively examines each input path using `stat()`.

If the path represents a regular file, its name, size, and contents are written to the archive.

If the path represents a directory, the directory is recorded in the archive and its contents are recursively processed.

### Archive Extraction

When only one path is supplied, it is treated as an existing archive:

```bash
./archive INFILE
```

For example:

```bash
./archive archive.dat
```

The program reads the encoded entries from the archive and recreates the corresponding files and directory structure.

## Building

Compile the program with GCC:

```bash
gcc -std=gnu99 -Wall -Wextra -o archive archive.c
```

A simpler build also works:

```bash
gcc -o archive archive.c
```

No external dependencies are required beyond a standard C compiler and Unix/POSIX environment.

## Usage

The program's command-line interface is:

```text
Usage: ./archive FILE... OUTFILE
       ./archive INFILE
```

### Create an Archive

Suppose the current directory contains:

```text
project/
├── notes.txt
├── src/
│   ├── main.c
│   └── utils.c
└── data/
    └── input.txt
```

Create an archive of the entire directory:

```bash
./archive project project.archive
```

The program recursively walks through `project` and stores its directory structure and file contents inside `project.archive`.

### Extract an Archive

To extract it:

```bash
./archive project.archive
```

The program reconstructs the archived directory hierarchy and writes each file back to disk.

## Custom Archive Format

The archive uses a custom variable-length format rather than a fixed structure.

Entries begin with an ASCII decimal length followed by a colon:

```text
LENGTH:
```

The meaning of the following data depends on whether the entry represents a file or directory.

## Regular File Encoding

A regular file is encoded conceptually as:

```text
<filename-length>:<filename><file-size>:<raw-file-data>
```

For example, a file named:

```text
hello.txt
```

has a filename length of `9`, so the beginning of its archive entry would resemble:

```text
9:hello.txt
```

The file size follows immediately afterward.

If `hello.txt` contained 12 bytes, the representation would conceptually be:

```text
9:hello.txt12:<12 bytes of file data>
```

The file contents themselves are written directly using `fread()` and `fwrite()`.

This allows the decoder to determine exactly how many bytes belong to the file without relying on a delimiter inside the file contents.

## Directory Encoding

Directories are identified by storing their name with a trailing `/`.

Conceptually:

```text
<directory-name-length>:<directory-name>/
```

For example:

```text
project/
```

is stored as a directory entry because the decoded path ends in `/`.

The archive then contains all entries belonging to that directory.

After every item in the directory has been written, the directory is terminated with:

```text
0:
```

This acts as an end-of-directory marker.

Conceptually, a directory archive might look like:

```text
8:project/
9:hello.txt12:<file contents>
4:src/
6:main.c100:<file contents>
0:
0:
```

The format therefore naturally represents nested directory trees.

## Archive Format Summary

```text
Regular file:

<name length>:<name><data length>:<data>


Directory:

<name length>:<name>/
    <child entry>
    <child entry>
    ...
0:
```

The `0:` marker signals that the decoder has reached the end of the current directory.

## Recursive Packing

Archive creation is handled by the `pack()` function:

```c
void pack(char * const fn, FILE *outfp)
```

The function calls `stat()` to determine what type of filesystem object it received.

### Regular Files

For regular files, `pack()`:

1. Determines the filename length.
2. Writes the filename length and filename.
3. Gets the file size from `stat()`.
4. Writes the file size.
5. Opens the file.
6. Reads its contents in chunks.
7. Writes those bytes directly into the archive.

File contents are processed using a 200-byte working buffer:

```c
while (1)
{
    num_bytes = fread(buffer, 1, sizeof(buffer), reg_file_fp);

    if (num_bytes == 0)
        break;

    fwrite(buffer, 1, num_bytes, outfp);
}
```

This avoids needing to load an entire file into memory before archiving it.

### Directories

For directories, `pack()`:

1. Writes a directory entry ending in `/`.
2. Opens the directory using `opendir()`.
3. Changes into the directory using `chdir()`.
4. Iterates over entries using `readdir()`.
5. Ignores `.` and `..`.
6. Recursively calls `pack()` for each entry.
7. Writes `0:` when the directory is complete.
8. Changes back to the parent directory.

The recursion allows arbitrary nested directory structures to be represented.

Conceptually:

```text
pack(directory)
│
├── pack(file)
│
├── pack(subdirectory)
│   │
│   ├── pack(file)
│   └── pack(file)
│
└── pack(file)
```

## Recursive Extraction

Archive extraction is implemented by:

```c
int unpack(FILE *fp)
```

The decoder begins by reading decimal digits until it reaches the delimiter separating the length field from the entry name.

The name length is constructed one digit at a time:

```c
name_length = ((10 * name_length) + (chr - '0'));
```

The decoder then reads exactly that number of characters to recover the entry name.

### Extracting Directories

If the decoded name ends in `/`, the entry is treated as a directory.

The program:

1. Creates the directory structure.
2. Changes into that directory.
3. Recursively calls `unpack()`.
4. Continues until a `0:` directory terminator is found.
5. Changes back into the parent directory.

This mirrors the recursive structure used during archive creation.

### Extracting Files

If the decoded name does not end in `/`, the entry is treated as a regular file.

The program:

1. Opens the output file with `fopen()`.
2. Decodes the file's data length.
3. Reads exactly that many bytes from the archive.
4. Writes each byte to the newly created file.
5. Closes the output file.

Because the file size is explicitly encoded, file data does not require a special terminating character.

## Recursive Directory Creation

Extraction uses the custom helper:

```c
int mkpath(const char *pathname, mode_t mode)
```

`mkpath()` behaves similarly to `mkdir()`, but it also creates missing parent directory components.

For a path such as:

```text
project/src/parser/
```

the function can create the required hierarchy one component at a time.

It uses `stat()` to determine whether each component already exists and `mkdir()` when a directory needs to be created.

Extracted directories are requested with permissions:

```text
0700
```

subject to normal filesystem behavior and the process's environment.

## Filesystem Traversal

The project demonstrates low-level directory traversal using:

```c
DIR *dir;
struct dirent *dent;

dir = opendir(fn);

while ((dent = readdir(dir)) != NULL)
{
    ...
}
```

The special entries:

```text
.
..
```

are explicitly ignored to prevent infinite recursion.

Each remaining directory entry is passed back into `pack()`.

## Directory Navigation

The implementation uses `chdir()` while recursively traversing and extracting directory trees.

During packing:

```text
current directory
      │
      ▼
chdir(directory)
      │
      ├── process child
      ├── process child
      └── process child
      │
      ▼
chdir("..")
```

Extraction uses the same general recursive pattern.

This simplifies path construction because nested entries can be processed relative to their current parent directory.

## Supported Filesystem Objects

The archiver explicitly processes:

* Regular files
* Directories

Other filesystem object types are skipped.

For example:

```text
Skipping non-regular file `...'.
```

This means objects such as symbolic links, depending on how they are reported by `stat()`, special devices, sockets, or other special filesystem entries are not explicitly encoded by the custom archive format.

## Example

Create some example data:

```bash
mkdir -p example/docs
printf 'HELLO WORLD\n' > example/hello.txt
printf 'ARCHIVE TEST\n' > example/docs/test.txt
```

The resulting structure is:

```text
example/
├── hello.txt
└── docs/
    └── test.txt
```

Create an archive:

```bash
./archive example example.archive
```

The archiver recursively visits:

```text
example/
example/hello.txt
example/docs/
example/docs/test.txt
```

You can then move or remove the original directory and extract the archive:

```bash
mv example example_original
./archive example.archive
```

The extracted structure should again contain:

```text
example/
├── hello.txt
└── docs/
    └── test.txt
```

Compare a recovered file:

```bash
cmp example/hello.txt example_original/hello.txt
```

If the files are identical, `cmp` produces no output.

## Program Structure

The implementation is organized around four primary functions:

### `mkpath()`

```c
int mkpath(const char *pathname, mode_t mode)
```

Creates missing directory components while extracting an archive.

### `getcwd_a()`

```c
char *getcwd_a(void)
```

Allocates enough memory to hold the current working directory, dynamically increasing the buffer size when necessary.

This helper is defined in the source even though the current archive workflow does not directly use it.

### `pack()`

```c
void pack(char * const fn, FILE *outfp)
```

Recursively encodes files and directories into the archive.

### `unpack()`

```c
int unpack(FILE *fp)
```

Decodes archive entries and recursively reconstructs the archived directory structure.

A return value of `1` is used internally to signal the `0:` end-of-directory marker.

## High-Level Workflow

### Packing

```text
                    archive(FILE/DIRECTORY)
                              │
                              ▼
                           stat()
                              │
                ┌─────────────┴─────────────┐
                │                           │
                ▼                           ▼
          Regular File                  Directory
                │                           │
                ▼                           ▼
         Encode filename              Encode name + "/"
                │                           │
                ▼                           ▼
          Encode filesize                opendir()
                │                           │
                ▼                           ▼
        Write raw contents              readdir()
                                            │
                                            ▼
                                      Recursive pack()
                                            │
                                            ▼
                                       Write "0:"
```

### Unpacking

```text
                       Read name length
                              │
                              ▼
                         Read name
                              │
                ┌─────────────┴─────────────┐
                │                           │
                ▼                           ▼
           Ends with "/"               Regular file
                │                           │
                ▼                           ▼
             mkpath()                 Read file size
                │                           │
                ▼                           ▼
             chdir()                  Create file
                │                           │
                ▼                           ▼
       Recursive unpack()            Copy N bytes
                │
                ▼
         Encounter "0:"
                │
                ▼
            chdir("..")
```

## Error Handling

The program includes checks for several filesystem operations.

Examples include failure to open output files:

```text
Could not open output file
```

failure to open archived data:

```text
Unable to open data file [...]
```

and directory creation failures through:

```c
err(errno, "mkpath()");
```

The code also checks whether existing path components are directories before continuing directory reconstruction.

## Technical Concepts Demonstrated

This project demonstrates several important C and Unix systems-programming concepts:

* Recursive algorithms
* Filesystem traversal
* Directory streams
* File metadata inspection
* Custom serialization formats
* Variable-length encoding
* Raw file I/O
* Dynamic memory allocation
* Pointer and string manipulation
* File and directory creation
* Current-working-directory management
* Command-line argument processing
* Unix filesystem APIs

## Project Structure

A minimal repository can be organized as:

```text
.
├── archive.c
└── README.md
```

After compilation:

```text
.
├── archive
├── archive.c
└── README.md
```
---

**Built with C and Unix/POSIX filesystem APIs to explore recursive filesystem traversal, serialization, and archive reconstruction.**
