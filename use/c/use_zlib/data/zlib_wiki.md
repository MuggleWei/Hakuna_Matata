from: https://en.wikipedia.org/wiki/Zlib

## zlib

zlib (/ˈziːlɪb/ or "zeta-lib", /ˈziːtəˌlɪb/)[1][2] is a software library used for data compression. zlib was written by Jean-loup Gailly and Mark Adler and is an abstraction of the DEFLATE compression algorithm used in their gzip file compression program. zlib is also a crucial component of many software platforms, including Linux, macOS, and iOS. It has also been used in gaming consoles such as the PlayStation 4, PlayStation 3, Wii U, Wii, Xbox One and Xbox 360.

The first public version of zlib, 0.9, was released on 1 May 1995 and was originally intended for use with the libpng image library. It is free software, distributed under the zlib License.

## Capabilities

### Encapsulation
zlib compressed data are typically written with a gzip or a zlib wrapper. The wrapper encapsulates the raw DEFLATE data by adding a header and trailer. This provides stream identification and error detection that are not provided by the raw DEFLATE data.

The gzip header, used in the ubiquitous gzip file format, is larger than the zlib header, as it stores a file name and other file system information.

### Algorithm
As of September 2018, zlib only supports one algorithm, called DEFLATE, which uses a combination of a variation of LZ77 (Lempel–Ziv 1977) and Huffman coding.[3] This algorithm provides good compression on a wide variety of data with minimal use of system resources. This is also the algorithm used in the Zip archive format. The header makes allowance for other algorithms, but none are currently implemented.

### Resource use
zlib provides facilities for control of processor and memory use. A compression level value may be supplied that trades speed for compression. There are also facilities for conserving memory, useful in restricted memory environments, such as some embedded systems.

### Strategy
The compression can be optimized for specific types of data. If one is using the library to always compress specific types of data, then using a specific strategy may improve compression and performance. For example, if the data contain long lengths of repeated bytes, the run-length encoding (RLE) strategy may give good results at a higher speed. For general data, the default strategy is preferred.

### Error handling
Errors in compressed data may be detected and skipped. Further, if "full-flush" points are written to the compressed stream, then corrupt data can be skipped, and the decompression will resynchronize at the next flush point — although no error recovery of the corrupt data is provided. Full-flush points are useful for large data streams on unreliable channels, where some data loss is unimportant, such as in some multimedia applications. However, creating many flush points can affect the speed as well as the amount (ratio) of compression.

### Data length
There is no limit to the length of data that can be compressed or decompressed. Repeated calls to the library allow an unlimited number of blocks of data to be handled. Some ancillary code (counters) may suffer from overflow for long data streams, but this does not affect the actual compression or decompression.

When compressing a long (or infinite) data stream, it is advisable to write regular full-flush points.
