
#include "FileReader.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "TxtReader.h"
#include "spdlog/spdlog.h"

/**
 * @details Retrieves a reader instance for a specific file based on its
 * extension.
 *
 * - `txt` files are handled by `TxTReader`.
 * - `yaml` files are unimplemented
 */
// TODO yaml
std::unique_ptr<FileReader> FileReader::getReaderForFile(const std::string &fileName) {
    size_t extension_start = fileName.find_last_of('.');

    // retrieve file extension
    // no file extension retrieved assumes 'txt' format
    std::string extension;
    if (extension_start == std::string::npos) {
        spdlog::error("no file extension detected in file `{}`, assume 'txt'", fileName);
        extension = "txt";
    } else {
        extension = fileName.substr(extension_start + 1);
    }

    // return appropriate reader instance based on detected file format
    // `txt` yields TxtReader
    if (extension == "txt") {
        return std::make_unique<TxtReader>();
    }

    spdlog::error("no reader available for file extension `{}`, assume 'txt'", extension);
    return std::make_unique<TxtReader>();
}
