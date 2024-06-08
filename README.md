# Archivator

Archivator is a command-line utility for archiving and unarchiving files. It supports multiple archiving methods and password protection for archives.

## Installation

Clone the repository and compile the source code using your preferred C++ compiler.

```bash
git clone https://github.com/birmay95/Archivator
cd archivator
make
```

## Usage

```bash
./archiver [OPTIONS]
```

Options

- `-f, --file FILE`: Specify the file to be archived/unarchived.
- `-p, --path PATH`: Specify the path where the archive/unarchive should be placed. Default is current directory.
- `-a, --archive`: Archive the specified file(s).
- `-u, --unarchive`: Unarchive the specified file(s).
- `-h, --hofman`: Use Hofman method for archivation.
- `-l, --lzw`: Use LZW method for archivation.
- `-L, --lz77`: Use LZ77 method for archivation.
- `-n, --name NAME`: Specify the name of the archive.
- `-P, --password`: Specify the password for the archive.
- `-H, --help`: Display help and exit.

## Examples

Archive a file:

```bash
./archiver -a -f myfile.txt -n myarchive
```

Unarchive a file:

```bash
./archiver -u -f myarchive.archive
```

## The project team

Contacts for communication with the developer.

- [Mihail Borovensky](https://t.me/mishail_b) — Developer