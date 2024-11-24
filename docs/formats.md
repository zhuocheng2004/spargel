# File Formats

## PPM

ASCII plain text

```
P3 # this is a comment
3 2 # width and height
255 # max value for each color
# first row/line
255   0   0 # pixel 1 of row 1
  0 255   0 # pixel 2
  0   0 255
# second row
255 255   0
255 255 255
  0   0   0
```

## OpenEXR

- Data in an OpenEXR file are densely packed.
- Data in an OpenEXR file are little-endian (i.e. the least significant byte is closest to the start of the file)

General structures:

- magic number
- version field
- header (single-part files), or an array of headers (multi-part files)
- line offset table (single-part files), or an array of chunk offset tables (multi-part files)
- scan lines, or tiles, or chunks

### Magic Number

The magic number, of type `i32`, is always `20000630` (which is equal to `0x1312F76`).
Thus the first four bytes of an OpenEXR file are `0x76`, `0x2F`, `0x31`, and `0x01`.

### Version Field

The version field is of type `i32`.
The first byte, (i.e. bits 0 to 7, the 8 least significant bits), indicates the file format version number.
The current OpenEXR format version number is `2`.

Bits 8 to 31 are boolean flags.

- Bit 9 indicates that this file is a single-part file in tile format.
- Bit 10 indicates that this file contains long names. (255 bytes instead of 31 bytes)
- Bit 11 indicates that this file contains non-image data, like deep data.
- Bit 12 indicates that this file is a multi-part file.
- The other flags are zero.

The default file format is single-part scan-line.

### Headers

A header is a sequence of attributes ended by a null byte.
Single-part files contain a single header.
Multi-part files contain a sequence of headers ended by an empty header, i.e. a null byte.
In particular, the last two bytes of the headers of a multi-part file is two null bytes.

### Attributes

An attribute consists of an attribute name, an attribute type, an attribute size, and an attribute value.
Both attribute names and attribute types are null-terminated strings.
The attribute size is of type `i32`.
The layout of the attribute value depends of type attribute type, which can be application specific.
Some attribute names and attribute types are predefined by the OpenEXR format.

### Offset Tables

An offset table is a sequence of offsets, with one offset per chunk.
Each offset is of type `u64`, and indicates the distance, in bytes, between the start of the file and the start of the chunk.

The number of entries in the offset table is calculated from the attributes.
TODO: write down the formulas.

- scan-line: each chunk is a scan-line; scan-lines are ordered in increasing `y` order
- tile: each chunk is a tile; tiles are ordered in increasing `y` order
- multi-part: each part has a offset table

### Chunks

The layout of each chunk is as follows:
- part number (if multi-part file)
- chunk data

The part number is of `u64`.

Chunks have four types:
- scan-line
- tile
- deep scan-line
- deep tile

#### Scan-line

For scan-line and deep scan-line, several scan-lines are stored together as a scan-line block.
The number of lines per block depends on the compression method.

Each scan-line block:
- `y` coordinate
- pixel data size
- pixel data

Each scan line is contiguous, and within a scan line the data for each channel are contiguous.
Channels are stored in alphabetical order, according to channel names.
Within a channel, pixels are stored left to right.

Pixel data is compressed using the compression attribute.

#### Tile

- tile coordinates, four `i32`: tile_x, tile_y, level_x, level_y
- pixel data size
- pixel data

Pixel data in a tile is stored in scan-line.
