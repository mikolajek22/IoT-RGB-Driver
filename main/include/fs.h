#include "esp_littlefs.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#define WRITE_PERMISSION        "w"
#define READ_PERMISSION         "r"
#define READ_WRITE_PERMISSION   "r+"
#define APPEND_PERMISSION       "a"

#define READ_SIZE           4096

/**
 * @brief used to open file from LFS. It is obligatory to do any operation on the file, e.g. wirte/read. In case of the file does not exist it will be created.
 * 
 * @param fID IN, ID of the file handler
 * @param fName IN, string name of the file to be opened.
 * @param permission IN, access to the file wirte, read or read/write ("r", "w", "r+")
 * 
 * @returns ESP_OK if file has been opened, else ESP_FAIL
 **/
esp_err_t fs_openFile(uint8_t fID, char* fName, char* permission);

/**
 * @brief Closing file function. It is necessery to close file in order to save changes.
 * 
 * @param fID IN, ID of the file handler
 * 
 * @returns ESP_OK if file has been closed, else ESP_FAIL.
 **/
esp_err_t fs_closeFile(uint8_t fID);

/**
 * @brief This function checks if there is free to use file handler.
 * 
 * @param fID OUT, ptr to ID of the free to use file handler
 * 
 * @returns ESP_OK if ID has been found, else ESP_FAIL.
 **/
esp_err_t fs_findID(uint8_t *fID);

/**
 * @brief This function mounts file system on the device.
 * 
 * @returns ESP_OK if system has been mounted succesfully, else ESP_FAIL.
 **/
esp_err_t fs_mount(void);

/**
 * @brief reading from file 255 bytes or less.
 * 
 * @param fID OUT, ID of the file handler
 * @param fName IN, name of the file to be read
 * @param buffer  OUT, ptr to array where data will be stored
 * @param offset IN, offset from the beggining of the file.
 * 
 * @returns amount of readen bytes.
 **/
size_t fs_readFile(uint8_t fID, char* fName, char* buffer, size_t offest);

/**
 * @brief writting into the file
 * @param fID IN, ID of the file handler
 * @param fName IN, name of the file to be read
 * @param buffer IN, size of the data to be written into the file.
 * @param writeSize, IN, size of data to be written into the file
 * @return size of the written data (bytes)
 */
size_t fs_writeFile(uint8_t fID, char* fName, char* buffer, uint16_t writeSize);

/**
 * @brief file rewinding
 * @param ID IN, ID of the file handler
 * @return ESP_OK if file has been rewinded, ESP_FAIL if file ID has not been found.
 */
esp_err_t fs_rewindFile(uint8_t fID, bool overwrite);

/**
 * @brief file delating
 * @param ID IN, ID of the file handler
 * @return ESP_OK if file has been delated, ESP_FAIL if failed to delate.
 */
esp_err_t fs_delateFile(uint8_t fID);

/**
 * @brief file size
 * @param ID IN, ID of the file handler
 * @return size of the file.
 */
size_t fs_fileSize(uint8_t fID);