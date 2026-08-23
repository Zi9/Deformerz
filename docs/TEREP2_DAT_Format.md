# Terep2 Car DAT File Format

This document is the result of reverse engineering the Terep2 car data files in an attempt to fully understand them and to aid in building LibTerep car support. This file may not be 100% correct and matching the original implementation, however it is a best effort document based on reverse engineering of the files.

## Background

Terep2 expects 1-5 car dat files to be in the current folder and reads them sequentially. File name format corresponds to `CAR{ID}.DAT`with `ID` being a number from 1 to 5. Terep2 internally loads the data file by issuing an INT 21,3F call to read from a file with CX (number of bytes to read) set to 10000. Imposing a limit of 10000 bytes max file size for the dat files (not verified yet).

Terep2 also processes the data on load in various ways which still need to be fully reverse engineered to understand. Some of it my be related to conversions to floating point numbers as the file holds only integer data on disk.

## File Structure

The car data file can be split up into 4 logical chunks:

- Header
- Array of point definitions
- Array of physics segment definitions
- Array of variable length definitions (polygons and other properties)

### The Header

In Terep2, the car data files always start with a 132 byte header. This header contains offsets to chunk beginnings from the start of the file and some other miscellaneous values which are still unknown currently. The header also contains a lot of blank space which appears to be used by the game as pre-allocated space for runtime data, as the file gets loaded into ram and directly used for physics simulation and rendering.

| Offset | Datatype | Description                                                               |
| ------ | -------- | ------------------------------------------------------------------------- |
| 0      | uint16   | Offset to the start of the point definitions chunk                        |
| 2      | uint16   | Offset to the start of the physics segment definitions chunk              |
| 4      | uint16   | Offset to the start of the variable length definitions chunk              |
| 6      | uint16   | An unknown value that is always 130 on all DATs I've seen                 |
| 8      | uint16   | An unknown value appears to either be 0 or 2 on all of the DATs I've seen |

### The Point Definitions Chunk

This chunk contains an array of 3D space points in XZY format. Some points are special and may also have a nonzero size. Each data point definition is 28 bytes.

#### Point Definition

| Offset | Datatype | Description                                                     |
| ------ | -------- | --------------------------------------------------------------- |
| 0      | int32    | X position value of the point                                   |
| 4      | int32    | Z position value of the point                                   |
| 8      | int32    | Y position value of the point                                   |
| 22     | int32    | Size value of the point (only for camera and wheel type points) |
| 26     | int16    | Point type designator (see table below)                         |

#### Point Types

| ID  | Type        |
| --- | ----------- |
| -1  | Camera      |
| 0   | Normal      |
| 1   | Rear Wheel  |
| 2   | Front Wheel |

### The Physics Segment Definitions Chunk

This chunk contains an array of physics segment (link between 2 points) definitions along with some physics data that is currently unknown. Each definition is 14 bytes;

#### Physics Segment Definition

| Offset | Datatype | Description                                               |
| ------ | -------- | --------------------------------------------------------- |
| 0      | uint16   | Index of the first point of the link (points array item)  |
| 2      | uint16   | Index of the second point of the link (points array item) |
| 4      | uint16   | Unknown physics related value                             |
| 6      | uint16   | Unknown physics related value                             |
| 8      | uint16   | Physics segment link type (see table below)               |
| 10     | uint16   | Unknown physics related value                             |
| 12     | uint16   | Unknown physics related value                             |

#### Segment Types

| ID  | Type                                                   |
| --- | ------------------------------------------------------ |
| 1   | Normal link between 2 points                           |
| 4   | Link where one of the points connects to a rear wheel  |
| 6   | Link where one of the points connects to a rear wheel  |
| 10  | Link where one of the points connects to a front wheel |
| 12  | Link where one of the points connects to a front wheel |
