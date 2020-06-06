/*
PXO file-type reader/writer for C++.
version 0.1
https://github.com/jegor377/pxo

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2020 Igor Santarek.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#define PXO_VERSION_MAJOR 0
#define PXO_VERSION_MINOR 1

#include "json.hpp"
#include <zstd.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>

namespace pxo {
    namespace compression {
        class CompressionException : public std::exception {
        protected:
            std::string path;
            std::string problem;
        public:
            CompressionException(std::string path, std::string problem)
                : std::exception() {
                this->path = path;
            }

            const char* what () const throw () {
                return ("Error with file [" + path + "]! " + problem).c_str();
            }
        };

        class EmptyFileError : public CompressionException {
        public:
            EmptyFileError(std::string path)
                : CompressionException(path, "Empty file.") {}
        };

        class FileReadError : public CompressionException {
        public:
            FileReadError(std::string path)
                : CompressionException(path, "Cannot to read from file.") {}
        };

        class FileWriteError : public CompressionException {
        public:
            FileWriteError(std::string path)
                : CompressionException(path, "Cannot to write to file.") {}
        };

        class FileNotCompressedError : public CompressionException {
        public:
            FileNotCompressedError(std::string path)
                : CompressionException(path, "File was not compressed.") {}
        };

        class DecompressedFileEstimationError : public CompressionException {
        public:
            DecompressedFileEstimationError(std::string path)
                : CompressionException(path, "Decompressed size cannot be estimated.") {}
        };

        class ZSTD_Error : public std::exception {
            std::string error;
        public:
            ZSTD_Error(const char* error)
                : std::exception() {
                this->error = error;
            }

            const char* what () const throw () {
                return ("ZSTD Error name: " + error).c_str();
            }
        };

        void save_file(std::string path, void* buffer, size_t size, int compression_level) {
            FILE* file = fopen(path.c_str(), "wb");
            size_t file_buff_size = ZSTD_compressBound(size);
            void* file_buff = malloc(file_buff_size);

            // compress
            size_t compressed_size = ZSTD_compress(
                file_buff,
                file_buff_size,
                buffer,
                size,
                compression_level
            );
            if(ZSTD_isError(compressed_size)) {
                throw ZSTD_Error(ZSTD_getErrorName(compressed_size));
            }

            // save in file
            const int buff_elem_size = 1; // in bytes
            size_t written_bytes = fwrite(file_buff, buff_elem_size, compressed_size, file);
            if(written_bytes != compressed_size) {
                if(ferror(file)) {
                    throw FileWriteError(path);
                }
            }
            
            fclose(file);
            free(file_buff);
        }

        void load_file(std::string path, void** buffer, size_t* size) {
            FILE* file = fopen(path.c_str(), "rb");
            size_t file_size;
            void* file_buff;

            // read file size
            fseek(file, 0, SEEK_END);
            file_size = ftell(file);
            rewind(file);

            // check if file is empty
            if(file_size == 0) {
                fclose(file);
                throw EmptyFileError(path);
            }

            // reading the file contents
            const int buff_elem_size = 1; // in bytes
            file_buff = malloc(file_size);
            size_t read_bytes = fread(file_buff, buff_elem_size, file_size, file);
            if(read_bytes != file_size) {
                if(ferror(file)) {
                    throw FileReadError(path);
                }
            }
            fclose(file);

            // estimating decompress size
            size_t buffer_size = ZSTD_getFrameContentSize(file_buff, file_size);
            if(buffer_size == ZSTD_CONTENTSIZE_ERROR) {
                throw FileNotCompressedError(path);
            }
            if(buffer_size == ZSTD_CONTENTSIZE_UNKNOWN) {
                throw DecompressedFileEstimationError(path);
            }

            // decompress file contents
            *buffer = malloc(buffer_size);
            *size = ZSTD_decompress(*buffer, buffer_size, file_buff, file_size);
            if(ZSTD_isError(*size)) {
                throw ZSTD_Error(ZSTD_getErrorName(*size));
            }
            
            free(file_buff);
        }
    }

    namespace achrivizing {
        class FileIsNotDirectoryError : std::exception {
        public:
            FileIsNotDirectoryError() : std::exception() {}

            const char* what () const throw () {
                return "Cannot to list the directory because it's file!";
            }
        }

        /*
        File binary structure:
        0-3 [4] bytes size (uint32_t)
        4-15 [12] bytes name (char*)
        16-17 [1] byte is_dir (uint8_t)
        18-size [size] bytes content (void*)
        */
        struct File {
            size_t size;
            std::string name;
            bool is_dir;
            void* content;
        }

        std::vector<File> list_dir(File* directory) {
            if(!directory.is_dir) throw FileIsNotDirectoryError();

            std::vector<File> files;
            uint32_t pos = 0;
            while(pos < directory.size) {
                File file;
                ;
            } 
        }
    }
}