# PXO File Format Specification (CANDIDATE)
PXO file format specification repository.

1. [References](#references)
2. [Introduction](#introduction)
3. [Header](#header)
4. Frames
5. Config Chunks
6. File Format Changes

## References
PXO files use Intel (little-endian) byte order.

* BYTE: An 8-bit unsigned integer value
* WORD: A 16-bit unsigned integer value
* DWORD: A 32-bit unsigned integer value
* BYTE[n]: "n" BYTEs.
* PIXEL: Alias for BYTE[4], where each BYTE represent a channel from RGBA format.

## Introduction
The format is based on ASE file format specification, but with changes (signature and version instead of magic number, diffrent data types, etc...). The color depth is always static - 32 bit, and images are always compressed with ZStandard.

## Header

    BYTE[3]      Signature (Always 'PXO' in ascii codes)
    BYTE[3]      Version
    WORD         Frames
    WORD         Width in pixels
    WORD         Height in pixels