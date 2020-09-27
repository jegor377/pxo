# PXO File Format Specification (CANDIDATE)
PXO file format specification repository.

1. [References](#references)
2. [Introduction](#introduction)
3. [Header](#header)
4. [Tags](#tags)
5. [Frames](#frames)
6. [Data Chunks](#data-chunks)
7. [File Format Changes](#file-format-changes)

## References
PXO files use Intel (little-endian) byte order.

* BYTE: An 8-bit unsigned integer value
* WORD: A 16-bit unsigned integer value
* DWORD: A 32-bit unsigned integer value
* BYTE[n]: "n" BYTEs.
* STRING:
    - `WORD`: length (number of bytes)
    - `BYTE[length]`: characters (in UTF-8). The `'\0'` character is not included.
* PIXEL: Alias for BYTE[4], where each BYTE represent a channel from RGBA format.

## Introduction
The format is based on ASE file format specification, but with changes (signature and version instead of magic number, diffrent data types, etc...). The color depth is always static - 32 bit, and images are always compressed with [ZStandard](https://github.com/facebook/zstd).

## Header
Header without constant size.

    BYTE[3]      Signature (Always 'PXO' in ascii codes)
    BYTE[3]      Version (see Versioning section for more info)
    WORD         Frames count
    WORD         Layers count
    WORD         Tags count
    WORD         Width in pixels
    WORD         Height in pixels
    BYTE         Export format (see Export Formats section for more info)
    STRING       Save path
    STRING       Export path

### Versioning
Every new PXO File Format Specification has to be released along with new Pixelorama version. When it happens, the new PXO File Format Specification version derives from Pixelorama versioning system. For example, when Pixelorama 0.8.2 is released, then if new PXO file format is released, then it will have version value of "082" in ASCII.

### Export Formats
Export format stores value of an enum-like structure with respectable values:

    PNG = 0x1
    GIF = 0x2

## Tags
After header comes tags section which holds tags' informations.

    DWORD        Bytes in this section
    Tag          Tags list (see below)

### Tag

    STRING       Name
    PIXEL        Color of tag

## Frames
Each frame has this little header:

    DWORD        Bytes in this section
    WORD         Tag ID

## Data Chunks

### Cel Chunk
This chunk determine where to put a cel in the specified layer/frame.

    WORD         Layer index
    BYTE         Opacity level
    BYTE[]       "Raw Cel" data compressed with ZStandard method

ZStandard library that will allow you to decode and encode the data: https://github.com/facebook/zstd

## File Format Changes