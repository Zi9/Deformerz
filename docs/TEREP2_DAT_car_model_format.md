# Terep2 DAT Car Model File Format

This document is the result of reverse engineering the Terep2 car data files in an attempt to fully understand them and to aid in building LibTerep car support. This file may not be 100% correct and matching the original implementation, however it is a best effort document based on reverse engineering of the files.

## Background

Terep2 expects 1-5 car DAT files to be in the current folder and reads them sequentially. File name format corresponds to `CAR{ID}.DAT`with `ID` being a number from 1 to 5. Terep2 internally loads the data file by issuing an INT 21,3F call to read from a file with CX (number of bytes to read) set to 10000. Imposing a limit of 10000 bytes max file size for the DAT files (not verified if the engine can handle a file this large yet). The car data memory region appears to be 30000 bytes in size giving another maximum limit based on how many cars and what size you're trying to use.

Terep2 also processes the data on load in various ways which still need to be fully reverse engineered to understand. Some of it my be related to conversions to floating point numbers as the file holds only integer data on disk.

## File Structure

The car data file can be split up into 4 logical chunks:

- Header
- Array of point definitions
- Array of physics link definitions
- Array of variable length definitions (polygons and other properties)

The file is little-endian.

### The Header

In Terep2, the standard car data files always start with a 132 byte header. This header contains offsets to chunk beginnings from the start of the file and some other miscellaneous values. The header also contains a lot of blank space which appears to be used by the game as pre-allocated space for runtime data, as the file gets loaded into ram and directly used for physics simulation and rendering. Later tests confirm that this header size can be increased by offsetting the chunk starts.

| Offset | Datatype | Description                                                  |
| ------ | -------- | ------------------------------------------------------------ |
| 0      | `uint16` | Offset to the start of the point definitions chunk           |
| 2      | `uint16` | Offset to the start of the physics link definitions chunk    |
| 4      | `uint16` | Offset to the start of the variable length definitions chunk |
| 6      | `uint16` | An unknown value that is always 130 on all DATs I've seen    |
| 8      | `uint16` | Engine Sound / Performance Mode                              |

### The Point Definitions Chunk

This chunk contains an array of 3D space points in XZY format. Some points are special and may also have a nonzero size. Each data point definition is a fixed size of 28 bytes.

#### Point Definition

| Offset | Datatype | Description                                                     |
| ------ | -------- | --------------------------------------------------------------- |
| 0      | `int16`  | Unknown value                                                   |
| 2      | `int16`  | X position value of the point                                   |
| 4      | `int16`  | Unknown value                                                   |
| 6      | `int16`  | Z position value of the point                                   |
| 8      | `int16`  | Unknown value                                                   |
| 10     | `int16`  | Y position value of the point                                   |
| 24     | `int16`  | Size value of the point (only for camera and wheel type points) |
| 26     | `int16`  | Point type designator (see table below)                         |

The point contains 10 null bytes which appear to be padding or also used for runtime data storage.

#### Point Types

| ID  | ID in HEX | Type        |
| --- | --------- | ----------- |
| -1  | 0xFFFF    | Camera      |
| 0   | 0x0000    | Normal      |
| 1   | 0x0100    | Rear Wheel  |
| 2   | 0x0200    | Front Wheel |

### The Physics Link Definitions Chunk

This chunk contains an array of physics link between 2 points definitions along with some physics data. Each definition is a fixed size of 14 bytes.

#### Physics Link Definition

| Offset | Datatype | Description                                               |
| ------ | -------- | --------------------------------------------------------- |
| 0      | `uint16` | Index of the first point of the link (points array item)  |
| 2      | `uint16` | Index of the second point of the link (points array item) |
| 4      | `uint16` | Length of the link                                        |
| 6      | `uint16` | Also length of the link, not sure why duplicated          |
| 8      | `uint16` | Physics link link type (see table below)                  |
| 10     | `uint16` | Minimum length of the link                                |
| 12     | `uint16` | Maximum length of the link                                |

#### Link Types

| ID  | ID in HEX | Type                                                   |
| --- | --------- | ------------------------------------------------------ |
| 0   | 0x0000    | Some sort of extra link type                           |
| 1   | 0x0100    | Normal link between 2 points                           |
| 6   | 0x0600    | Link where one of the points connects to a rear wheel  |
| 10  | 0x0A00    | Link where one of the points connects to a front wheel |

### The Variable Length Definitions Chunk

This chunk contains an array of elements where each item is prefixed by an ID. Elements have a fixed size that differs from one to another. Most of the elements are polygon definitions. The chunk goes on until the end of the file. Usually the file ends with a null entry. This chunk does not start with a count of items unlike the previous two chunks.

#### Variable Length Definition Types

These are always 1 byte unlike all other definition types.

| ID  | ID in HEX | Type                                                                 |
| --- | --------- | -------------------------------------------------------------------- |
| 0   | 0x00      | Null entry, seems to affect the ability of rendering                 |
| 1   | 0x01      | Camera related properties. Usually only 1 of these appears in a file |
| 3   | 0x03      | Unknown polygon, seems to be clipping related, always 12 bytes long  |
| 4   | 0x04      | Colored polygon                                                      |
| 8   | 0x08      | Textured polygon                                                     |
| 10  | 0x0A      | Wheel related properties. Usually 4 of these appear in a file        |

Following tables list the structure of each entry. Each items definition starts after the ID byte.

##### Camera Property Item (ID 1 - 0x01)

This item is always 4 bytes.

| Offset | Datatype | Description                                                           |
| ------ | -------- | --------------------------------------------------------------------- |
| 0      | `uint16` | Double of the index of the Camera point in the point definition array |
| 2      | `uint8`  | Unknown value, usually 15                                             |
| 3      | `uint8`  | Unknown value, usually 15                                             |

##### Unknown Culling Polygon Item (ID 3 - 0x03)

This item is always a 3 point polygon which seems to affect the culling of wheels during rendering. Always 12 bytes long.

| Offset | Datatype | Description                                                       |
| ------ | -------- | ----------------------------------------------------------------- |
| 0      | `uint16` | Double of the index of the point. There is 3 of them sequentially |
| 6      | `uint8`  | Unknown value                                                     |
| 8      | `uint8`  | Unknown value                                                     |
| 12     | `uint8`  | Unknown value                                                     |

##### Colored Polygon Item (ID 4 - 0x04)

This item is variable length based on the amount of points defined in the polygon. The color items can be set independently, if they differ then the game dithers between the two colors for the polygon.

| Offset    | Datatype | Description                                                           |
| --------- | -------- | --------------------------------------------------------------------- |
| 0         | `uint8`  | Count of points in the polygon                                        |
| 1         | `uint16` | Double of the index of the point. These repeat by the amount of count |
| 2*count+1 | `uint8`  | Palette index value                                                   |
| 2*count+2 | `uint8`  | Palette index value                                                   |

Polygons seem to contain one more point than what would usually be defined. For rendered polygons this last point is usually a duplicate of the first point. Shadow polygons seem to have the index 0.

The color also has some special colors which may get rendered transparently or not visible at all. Color 240 (0xF0) appears to be invisible. Colors originate from the "Master palette" which gets loaded from `COL.PCX`.

##### Textured Polygon Item (ID 8 - 0x08)

This item is variable length based on the amount of points defined in the polygon. The index and UV data repeats for count (IDX, UVx, UVy).

| Offset | Datatype | Description                                                                                             |
| ------ | -------- | ------------------------------------------------------------------------------------------------------- |
| 0      | `uint8`  | Count of points in the polygon                                                                          |
| 1      | `uint16` | Double of the index of the point                                                                        |
| 3      | `uint16` | UV X coordinate (0 - MAX_UINT16 (65535)). To map it to usual 0-1 coordinates, divide the value by 65535 |
| 5      | `uint16` | UV Y coordinate (0 - MAX_UINT16 (65535)). To map it to usual 0-1 coordinates, divide the value by 65535 |

Sidenote, the game bundles textures with a size of 320x200 but discards the 64 pixels on the right side as the texture is constrained to 256 pixels wide.

##### Wheel Properties Item (ID 10 - 0x0A)

This item is always 187 bytes in size.

| Offset | Datatype         | Description                                                                      |
| ------ | ---------------- | -------------------------------------------------------------------------------- |
| 0      | `uint16`         | Double of the index of the wheel point                                           |
| 2      | `uint16`         | Unknown value, seems to affect the angle of the wheel billboard sprite rendering |
| 4      | `uint16`         | Unknown value, seems to be 0 most of the time                                    |
| 6      | `WheelSprite[9]` | Array of wheel sprite definitions, see table below                               |

It is also possible that the two uint16 values are actually an (u)int32, this is yet to be confirmed.

###### Wheel Sprite Structure

| Offset | Datatype              | Description                                          |
| ------ | --------------------- | ---------------------------------------------------- |
| 0      | `uint16`              | Height of the wheel sprite                           |
| 2      | `uint16`              | Width of the wheel sprite                            |
| 4      | `(uint16, uint16)[4]` | Array of UV tuples, UVs are multiplied by MAX_UINT16 |
