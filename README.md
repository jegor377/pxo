# PXO File Format (.pxo) Specification (INCOMPLETE AND DROPPED FOR NOW)
PXO file format specification repository.

1. [References](#references)
2. [Introduction](#introduction)
3. [Header](#header)
4. [Sections](#sections)
5. [Data Chunks](#data-chunks)
6. [Notes](#notes)
7. [File Format Changes](#file-format-changes)

## References
PXO files use Intel (little-endian) byte order.

* `BYTE`: An 8-bit unsigned integer value
* `WORD`: A 16-bit unsigned integer value
* `DWORD`: A 32-bit unsigned integer value
* `BYTE[n]`: "n" BYTEs.
* `STRING`:
    - `WORD`: length (number of bytes)
    - `BYTE[length]`: characters (in UTF-8). The `'\0'` character is not included.
* `PIXEL`: Alias for BYTE[4], where each BYTE represent a channel from RGBA format.
* `COLOR`: Alias for BYTE[3], where each BYTE represent a channel from RGB format.

### Nomenclature:
- **Importer** is a program or a code that reads and processes the PXO file.
- **Exporter** is a program or a code that writes project data to the PXO file.
- **Reserved for future** should be set to 0.
- **Incremental** means that a field contains values that are not a binary mask but incremental integers.

## Introduction
The format is based on ASE file format specification, but with changes (signature and version instead of magic number, diffrent data types, etc...). The color depth is always static - 32 bit, and images are always compressed with [ZStandard](https://github.com/facebook/zstd).

### Sections and data chunks
The PXO File Format is divided to sections and data chunks. All data chunks comes after their respectable section. Sections types should be described in format 0x##00, where ## stands for section type number, and data chunks types should be descripbed in format 0x##$$ where ## stands for section type number that they should be put after and $$ means data chunk type number. For example 0x0201 is section type number 2 and data chunk type numer 1 (which is Cel Chunk that should come after Frame section). Data Chunks that have certain sections type number in their type number should be added after these Sections.

## Header
Header is without constant size. It describes basic informations about the project file and tells whether it is a PXO file.

    BYTE[3]      Signature (Always 'PXO' in ascii codes)
    DWORD        Version
    WORD         Frames count
    WORD         Layers count
    WORD         Tags count
    WORD         Width in pixels
    WORD         Height in pixels
    BYTE         Export format: (Incremental)
                   0  - PNG
                   1  - GIF
    STRING       Save path
    STRING       Export path

### Versioning
Every PXO File Format Specification version is defined as integer value that starts from 0 up to 2^32-1. First PXO File Format Specification has version number 0.

## Sections
PXO file is divided to section. Sections can occur more than once. Each section has Data Chunks that should come after them but they're not forced to do it.

### Control
This section comes after Header and holds information about other stuff than cels. It has a header:

    BYTE         Section type (always 0x0100)
    DWORD        Bytes in the section
    BYTE         Flags:
                   1  - Has vertical symmetry point
                   2  - Has horizontal symmetry point
    WORD         Vertical symmetry point
    WORD         Horizontal symmetry point
    DWORD        Number of chunks that follow after this section

### Frame
This section comes after Control and hold information about frames (cels, etc..). Each frame has this little header:

    BYTE         Section type (always 0x0200)
    DWORD        Bytes in the section
    WORD         Frame index
    WORD         Frame duration (in miliseconds)
    DWORD        Number of chunks that follow after this section

## Data Chunks
Each section has some data blocks comming after it. These data blocks are Data Chunks. Each Data Chunk starts with header like this:

    DWORD        Chunk size
    WORD         Chunk type
    BYTE[]       Chunk data

### Additional informations
If Data Chunk can't be identified because the chunk type isn't yet implemented in the Importer or for some other reason (other than error), then it should be skipped.

### Layer Chunk (0x0101)

    WORD         Layer index
    STRING       Layer name
    WORD         Reserved for future (Future layer type)
    WORD         Reserved for future (Future layer child level)
    BYTE         Flags:
                   1  - Visible
                   2  - Locked
                   4  - New cels linked
    BYTE         Reserved for future (Future extension of flags)
    BYTE         Opacity level
    WORD         Reserved for future (Future blend mode)

### Tag Chunk (0x0102)

    STRING       Name
    COLOR        Color of tag
    WORD         From frame
    WORD         To Frame
    BYTE         Reserved for future (Future loop direction)

### Guide Chunk (0x0103)

    BYTE         Type:
                   1  - Vertical guide
                   2  - Horizontal guide
    WORD         Position

### Brush Chunk (0x0104)

    WORD         Width
    WORD         Height
    DWORD        Brush image data bytes count
    BYTE[]       Brush image data compressed with ZStandard method (see Note 1)

### Palette Chunk (0x0105)

    DWORD        Palette color count
    PIXEL[]      Colors

### Cel Chunk (0x0201)
This chunk determine where to put a cel in the specified layer/frame. If cel is of type Linked Cel then cel does not contain any image data. Only link to other cel in different frame.

    WORD         Layer index
    WORD         Frame index
    BYTE[4]      Reserved for future (Future cel position)
    BYTE         Cel type: (Incremental)
    + For cel type = 0 (Compressed Image)
    DWORD        Image data bytes count
    BYTE[]       Image data compressed with ZStandard method (see Note 1)
    + For cel type = 1 (Linked Cel)
    WORD         Frame position to link with

## Notes

### Note 1
ZStandard library that will allow you to decode and encode the data: https://github.com/facebook/zstd

## File Format Changes
