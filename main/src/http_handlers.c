#include "http_handlers.h"
#include "fs.h"
#include "startup.h"

#define SETTING_FILE_NAME           "settings.json"
/* configuration upload query */
#define KEY_QUERRY_ACTION           "action"
#define VALUE_QUERRY_REBOOT         "reboot"
#define VALUE_QUERRY_SAVE           "save"       
#define VALUE_QUERRY_UPLOAD         "upload"

/* configuration .HTML files upload query */
#define KEY_QUERY_PAGE              "page"
#define VALUE_QUERY_MAIN            "main"
#define HTML_PAGE_NAME_MAIN         "main_page.html"
#define VALUE_QUERY_CONFIG          "config"
#define HTML_PAGE_NAME_CONFIG       "config_page.html"
#define VALUE_QUERY_CONTROL         "control"
#define HTML_PAGE_NAME_CONTROL      "control_page.html"
#define VALUE_QUERY_LOGS            "logs"
#define HTML_PAGE_NAME_LOGS         "logs_page.html"

#define MAX_CFG_FILE_SIZE           4096        // 4  * 1024 (1kB)
#define MAX_HTML_PAGE_SIZE          24576       // 24 * 1024 (24 kB)

static const char* TAG = "http_handler";
esp_netif_ip_info_t connectionInfo;

extern void actualizeValue(uint8_t red, uint8_t green, uint8_t blue);


/* Get .html default page */
esp_err_t http_handlers_getStartPage_EventHandler(httpd_req_t *req) {
    char*       pageText;
    uint8_t     fileID;
    size_t      readBytes;
    uint16_t    totalReadBytes = 0;
    if (fs_findID(&fileID) == ESP_OK) { 
        pageText = malloc(MAX_HTML_PAGE_SIZE);
        if (pageText == NULL) {
            ESP_LOGE(TAG, "Not enough memory, malloc failed!");
        }
        else {
            if (ESP_OK == fs_openFile(fileID, HTML_PAGE_NAME_MAIN, READ_PERMISSION)) {
                do {
                    readBytes = fs_readFile(fileID, HTML_PAGE_NAME_MAIN, pageText + totalReadBytes, totalReadBytes);
                    totalReadBytes += readBytes;
                } while (readBytes == READ_SIZE);

                if (totalReadBytes > 0) {
                    ESP_LOGI(TAG, "File to send: \nFile name: %s \nFile size: %d bytes", HTML_PAGE_NAME_MAIN, totalReadBytes);
                    if (httpd_resp_send(req, pageText, totalReadBytes) == ESP_OK) {
                        ESP_LOGI(TAG, "%s page has been sent", HTML_PAGE_NAME_MAIN);
                    }
                    else {
                        ESP_LOGE(TAG, "%s page sending error!", HTML_PAGE_NAME_MAIN);
                    }
                }
                else {
                    ESP_LOGE(TAG, "readed bytes from file: %s is equal to: %d!", HTML_PAGE_NAME_MAIN, totalReadBytes);
                }
                fs_closeFile(fileID);
            }
            else {
                ESP_LOGE(TAG, "File opening error: %s", HTML_PAGE_NAME_MAIN);
            }
        }
        free(pageText);
    }
    else {
        ESP_LOGE(TAG, "All file handlers are busy!");
    } 
    return ESP_OK;
}

/* Get .html pages */
esp_err_t  http_handlers_getPage_EventHandler(httpd_req_t *req) {
    char*       buffer;
    char*       pageText;
    uint8_t     fileID;
    size_t      readBytes;
    uint16_t    totalReadBytes = 0;
    size_t      bufferLen = httpd_req_get_url_query_len(req);
    
    if (bufferLen > 0) {
        buffer = malloc(bufferLen+1);
        memset(buffer, 0, bufferLen+1);
        if (httpd_req_get_url_query_str(req, buffer, bufferLen+1) == ESP_OK) {
            char key[64];
            //  example: 192.168.0.10/get?page=main
            if (httpd_query_key_value(buffer,KEY_QUERY_PAGE, key, 64) == ESP_OK) {
            
                /* SENDING MAIN PAGE */
                if (!strcmp(key, VALUE_QUERY_MAIN)) {
                    if (fs_findID(&fileID) == ESP_OK) { 
                        pageText = malloc(MAX_HTML_PAGE_SIZE);
                        if (pageText == NULL) {
                            ESP_LOGE(TAG,"Not enough memory, malloc failed!");
                        }
                        else {
                            if (ESP_OK == fs_openFile(fileID, HTML_PAGE_NAME_MAIN, READ_PERMISSION)) {
                                do {
                                    readBytes = fs_readFile(fileID, HTML_PAGE_NAME_MAIN, pageText + totalReadBytes, totalReadBytes);
                                    totalReadBytes += readBytes;
                                } while (readBytes == READ_SIZE);
                                if (totalReadBytes > 0) {
                                    ESP_LOGI(TAG, "File to send: \nFile name: %s \nFile size: %d bytes", HTML_PAGE_NAME_MAIN, totalReadBytes);
                                    if (httpd_resp_send(req, pageText, totalReadBytes) == ESP_OK) {
                                        ESP_LOGI(TAG, "%s page has been sent", HTML_PAGE_NAME_MAIN);
                                    }
                                    else {
                                        ESP_LOGE(TAG, "%s page sending error!", HTML_PAGE_NAME_MAIN);
                                    }
                                }
                                else {
                                    ESP_LOGE(TAG, "readed bytes from file: %s is equal to: %d!", HTML_PAGE_NAME_MAIN, totalReadBytes);
                                }
                            fs_closeFile(fileID);
                            }
                            else {
                                ESP_LOGE(TAG, "File opening error: %s", HTML_PAGE_NAME_MAIN);
                            }
                        }
                        free(pageText);
                    }
                    else {
                        ESP_LOGE(TAG, "All file handlers are busy!");
                    }
                }

                /* SENDING CONFIGURATION PAGE */
                else if (!strcmp(key, VALUE_QUERY_CONFIG)) {
                    if (fs_findID(&fileID) == ESP_OK) { 
                        pageText = malloc(MAX_HTML_PAGE_SIZE);
                        if (pageText == NULL){
                            ESP_LOGE(TAG,"Not enough memory, malloc failed!");
                        }
                        else {
                            if (ESP_OK == fs_openFile(fileID, HTML_PAGE_NAME_CONFIG, READ_PERMISSION)) {
                                do {
                                    readBytes = fs_readFile(fileID, HTML_PAGE_NAME_CONFIG, pageText + totalReadBytes, totalReadBytes);
                                    totalReadBytes += readBytes;
                                } while (readBytes == READ_SIZE);
                                if (totalReadBytes > 0) {
                                    ESP_LOGI(TAG, "File to send: \nFile name: %s \nFile size: %d bytes", HTML_PAGE_NAME_CONFIG, totalReadBytes);
                                    if (httpd_resp_send(req, pageText, totalReadBytes) == ESP_OK) {
                                        ESP_LOGI(TAG, "%s page has been sent", HTML_PAGE_NAME_CONFIG);
                                    }
                                    else {
                                        ESP_LOGE(TAG, "%s page sending error!", HTML_PAGE_NAME_CONFIG);
                                    }
                                }
                                else {
                                    ESP_LOGE(TAG, "readed bytes from file: %s is equal to: %d!", HTML_PAGE_NAME_CONFIG, totalReadBytes);
                                }
                            fs_closeFile(fileID);
                            }
                            else {
                                ESP_LOGE(TAG, "File opening error: %s", HTML_PAGE_NAME_CONFIG);
                            }
                        }
                        free(pageText);
                    }
                    else {
                        ESP_LOGE(TAG, "All file handlers are busy!");
                    }
                }

                /* SENDING CONTROL PAGE */
                else if (!strcmp(key, VALUE_QUERY_CONTROL)) {
                    if (fs_findID(&fileID) == ESP_OK) { 
                        pageText = malloc(MAX_HTML_PAGE_SIZE);
                        if (pageText == NULL){
                            ESP_LOGE(TAG,"Not enough memory, malloc failed!");
                        }
                        else {
                            if (ESP_OK == fs_openFile(fileID, HTML_PAGE_NAME_CONTROL, READ_PERMISSION)) {
                                do {
                                    readBytes = fs_readFile(fileID, HTML_PAGE_NAME_CONTROL, pageText + totalReadBytes, totalReadBytes);
                                    totalReadBytes += readBytes;
                                } while (readBytes == READ_SIZE);
                                if (totalReadBytes > 0) {
                                    ESP_LOGI(TAG, "File to send: \nFile name: %s \nFile size: %d bytes", HTML_PAGE_NAME_CONTROL, totalReadBytes);
                                    if (httpd_resp_send(req, pageText, totalReadBytes) == ESP_OK) {
                                        ESP_LOGI(TAG, "%s page has been sent", HTML_PAGE_NAME_CONTROL);
                                    }
                                    else {
                                        ESP_LOGE(TAG, "%s page sending error!", HTML_PAGE_NAME_CONTROL);
                                    }
                                }
                                else {
                                    ESP_LOGE(TAG, "readed bytes from file: %s is equal to: %d!", HTML_PAGE_NAME_CONTROL, totalReadBytes);
                                }
                            fs_closeFile(fileID);
                            }
                            else {
                                ESP_LOGE(TAG, "File opening error: %s", HTML_PAGE_NAME_CONTROL);
                            }
                        }
                        free(pageText);
                    }
                    else {
                        ESP_LOGE(TAG, "All file handlers are busy!");
                    }
                }

                /* SENDING LOGGING PAGE */
                else if (!strcmp(key, VALUE_QUERY_LOGS)) {
                    if (fs_findID(&fileID) == ESP_OK) { 
                        pageText = malloc(MAX_HTML_PAGE_SIZE);
                        if (pageText == NULL){
                            ESP_LOGE(TAG,"Not enough memory, malloc failed!");
                        }
                        else {
                            if (ESP_OK == fs_openFile(fileID, HTML_PAGE_NAME_LOGS, READ_PERMISSION)) {
                                do {
                                    readBytes = fs_readFile(fileID, HTML_PAGE_NAME_LOGS, pageText + totalReadBytes, totalReadBytes);
                                    totalReadBytes += readBytes;
                                } while (readBytes == READ_SIZE);
                                if (totalReadBytes > 0) {
                                    ESP_LOGI(TAG, "File to send: \nFile name: %s \nFile size: %d bytes", HTML_PAGE_NAME_LOGS, totalReadBytes);
                                    if (httpd_resp_send(req, pageText, totalReadBytes) == ESP_OK) {
                                        ESP_LOGI(TAG, "%s page has been sent", HTML_PAGE_NAME_LOGS);
                                    }
                                    else {
                                        ESP_LOGE(TAG, "%s page sending error!", HTML_PAGE_NAME_LOGS);
                                    }
                                }
                                else {
                                    ESP_LOGE(TAG, "readed bytes from file: %s is equal to: %d!", HTML_PAGE_NAME_LOGS, totalReadBytes);
                                }
                            fs_closeFile(fileID);
                            }
                            else {
                                ESP_LOGE(TAG, "File opening error: %s", HTML_PAGE_NAME_LOGS);
                            }
                        }
                        free(pageText);
                    }
                    else {
                        ESP_LOGE(TAG, "All file handlers are busy!");
                    }
                }

                /* PAGE REQUEST UNKNOWN */
                else {
                    ESP_LOGE(TAG, "Invalid value of querry parameter.");
                }
            }
            else {
                ESP_LOGE(TAG, "Invalid name of querry parameter.");
            }
        }
        else {
            ESP_LOGE(TAG, "Querry has not been found.");
        }
    }
    return ESP_OK;
}

/* Response to fetch info */
esp_err_t http_handlers_getInfo_EventHandler(httpd_req_t *req) {
    uint8_t mac[6];
    char macStr[20];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, &mac);
    snprintf(macStr, sizeof(macStr), "%x%x%x%x%x%x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    
    char ipAddrStr[16];
    snprintf(ipAddrStr, sizeof(ipAddrStr), IPSTR, IP2STR(&connectionInfo.ip));

    char netmaskStr[16];
    snprintf(netmaskStr, sizeof(netmaskStr), IPSTR, IP2STR(&connectionInfo.netmask));

    char gwStr[16];
    snprintf(gwStr, sizeof(gwStr), IPSTR, IP2STR(&connectionInfo.gw));

    char fwVStr[6];
    snprintf(fwVStr, sizeof(fwVStr), "%d.%d.%d", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH);

    char isCfgStr[6];
    snprintf(isCfgStr, sizeof(isCfgStr), "%s", defaultCfg.cfgFile ? "true" : "false");

    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "wifiMode", cJSON_CreateString(defaultCfg.mode));
    cJSON_AddItemToObject(root, "wifiName", cJSON_CreateString(defaultCfg.wifiName));
    cJSON_AddItemToObject(root, "wifiPass", cJSON_CreateString(defaultCfg.wifiPassword));
    cJSON_AddItemToObject(root, "macAddr", cJSON_CreateString(macStr)); //todo
    cJSON_AddItemToObject(root, "ipAddr", cJSON_CreateString(ipAddrStr));
    cJSON_AddItemToObject(root, "subnetMask", cJSON_CreateString(netmaskStr));
    cJSON_AddItemToObject(root, "gw", cJSON_CreateString(gwStr));
    cJSON_AddItemToObject(root, "fwV", cJSON_CreateString(fwVStr));
    cJSON_AddItemToObject(root, "cfgFile", cJSON_CreateString(isCfgStr));
    cJSON_AddItemToObject(root, "cfgAuthor", cJSON_CreateString(defaultCfg.author));
    cJSON_AddItemToObject(root, "cfgTime", cJSON_CreateString(defaultCfg.date));
 
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, cJSON_Print(root), strlen(cJSON_Print(root)));
    return ESP_OK;
}
/* send congiuration file .json to client */
esp_err_t http_handlers_getDownload_EventHandler(httpd_req_t *req) {

    char*       buffer;
    uint8_t     fileID;
    size_t      readBytes;
    uint16_t    totalReadBytes = 0;
    if (ESP_OK == fs_findID(&fileID)) {
        if (ESP_OK == fs_openFile(fileID, SETTING_FILE_NAME, READ_PERMISSION)) {
            buffer = calloc(4096, sizeof(uint8_t));
            do {
                readBytes = fs_readFile(fileID, SETTING_FILE_NAME, buffer + totalReadBytes, totalReadBytes);
                totalReadBytes += readBytes;
            } while (readBytes == READ_SIZE);
            cJSON *root = cJSON_Parse(buffer);
            if (ESP_OK == fs_closeFile(fileID)) {
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=\"settings.json\"");
                httpd_resp_send(req, cJSON_Print(root), strlen(cJSON_Print(root)));
                ESP_LOGI(TAG, "Configuration file has been sent.");
            }
            else {
                ESP_LOGE(TAG, "Error while closing file.");
            }
            free(buffer);
        }
    }
    return ESP_OK;
}

/* TODO: handler of RGB control */
esp_err_t http_handlers_postRGB_EventHandler(httpd_req_t *req) {
    const char* postBuffer[256];
    size_t      contentLen = req->content_len;
    uint8_t     red = 0;
    uint8_t     green = 0;
    uint8_t     blue = 0;

    if (httpd_req_recv(req, postBuffer, contentLen) > 0) {
        cJSON *root = cJSON_Parse(postBuffer);
        cJSON *rgbValues = cJSON_GetObjectItem(root, "rgbValues");
        red = cJSON_GetObjectItem(rgbValues, "red")->valueint;
        green = cJSON_GetObjectItem(rgbValues, "green")->valueint;
        blue = cJSON_GetObjectItem(rgbValues, "blue")->valueint;
        // if (red!=NULL && green!=NULL && blue!=NULL){
            httpd_resp_send(req, "Values actualized", sizeof("Values actualized"));
            actualizeValue(red, green, blue);
        // }
    }
    
    return ESP_OK;
}

esp_err_t http_handlers_postRGBSequence_EventHandler(httpd_req_t *req) {
    // TODO:
    httpd_resp_send(req, "Values actualized", sizeof("Values actualized"));
    return ESP_OK;
}

esp_err_t http_handlers_postRGBOriginal_EventHandler(httpd_req_t *req) {
    // TODO:
    httpd_resp_send(req, "Values actualized", sizeof("Values actualized"));
    return ESP_OK;
}

esp_err_t http_handlers_postConfiguration_EventHandler(httpd_req_t *req) {
    size_t contentLen = req->content_len;
    size_t querryLen = httpd_req_get_url_query_len(req);
    char* querryContent;
    querryContent = malloc(contentLen);
    // eg. xxx.xxx.xxx.xxx/configuration?action=reboot
    
    if (ESP_OK == httpd_req_get_url_query_str(req, querryContent, querryLen + 1)){
        char key[64];
        if (ESP_OK == httpd_query_key_value(querryContent, KEY_QUERRY_ACTION, key, 64)) {

            /* Handle system reboot request */
            if(0 == strcmp(key, VALUE_QUERRY_REBOOT)) {
                httpd_resp_send(req, "Sys rebooted", sizeof("Sys rebooted"));
                esp_restart();
                return ESP_OK;
            }

            /* Parse received data to the configuration file */
            else if ((0 == strcmp(key, VALUE_QUERRY_SAVE))){
                char *buffer;
                buffer = calloc(MAX_CFG_FILE_SIZE, sizeof(uint8_t));
                
                if (httpd_req_recv(req, buffer, contentLen) > 0){
                    char *bufferFile;
                    bufferFile = calloc(MAX_CFG_FILE_SIZE, sizeof(uint8_t));
                    size_t readBytes;
                    size_t totalReadBytes = 0;
                    uint8_t fileID;
                    if (ESP_OK == fs_findID(&fileID)) {
                        if (ESP_OK == fs_openFile(fileID, SETTING_FILE_NAME, READ_WRITE_PERMISSION)) {
                            do {
                                readBytes = fs_readFile(fileID, SETTING_FILE_NAME, bufferFile + totalReadBytes, totalReadBytes);
                                totalReadBytes += readBytes;
                            } while (readBytes == READ_SIZE);

                            espConfigurationFile_t cfgRcv;
                            espConfigurationFile_t cfgFile;
                            cfgRcv.root = cJSON_Parse(buffer);
                            cfgFile.root = cJSON_Parse(bufferFile);

                            cfgFile.settings = cJSON_GetObjectItem(cfgFile.root, "settings");
                            cfgFile.network = cJSON_GetObjectItem(cfgFile.settings, "network");
                            printf("1...");
                            if (NULL != cfgRcv.root){
                                printf("2...");
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "ipAddress")) {
                                    printf("3...");
                                    cJSON_GetObjectItem(cfgFile.network, "ipAddress")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "ipAddress")->valuestring;
                                }
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "netmask")) {
                                    cJSON_GetObjectItem(cfgFile.network, "netmask")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "netmask")->valuestring;
                                }
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "defaultGateway")) {
                                    cJSON_GetObjectItem(cfgFile.network, "defaultGateway")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "defaultGateway")->valuestring;
                                }
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "staticIP")) {
                                    cJSON_GetObjectItem(cfgFile.network, "staticIP")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "staticIP")->valuestring;
                                }
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "wifi_mode")) {
                                    cJSON_GetObjectItem(cfgFile.network, "STA/AP")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "wifi_mode")->valuestring;
                                }
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "wifiName")) {
                                    cJSON_GetObjectItem(cfgFile.network, "wifiName")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "wifiName")->valuestring;
                                }
                                if (NULL != cJSON_GetObjectItem(cfgRcv.root, "wifiPassword")) {
                                    cJSON_GetObjectItem(cfgFile.network, "wifiPassword")->valuestring = cJSON_GetObjectItem(cfgRcv.root, "wifiPassword")->valuestring;
                                }
                            }
                            if (ESP_OK == fs_rewindFile(fileID)) {
                                printf("4...");
                                ESP_LOGW(TAG,"saving file :%s",cJSON_Print(cfgFile.root));
                                if (0 < fs_writeFile(fileID, SETTING_FILE_NAME, cJSON_Print(cfgFile.root), strlen(cJSON_Print(cfgFile.root)))) {
                                    printf("5...");
                                    httpd_resp_send(req, "Configuration saved", strlen("Configuration saved"));
                                }
                                else {
                                    ESP_LOGE(TAG, "Error while writting into configuration file: %s", SETTING_FILE_NAME);
                                }
                            }
                            else {
                                ESP_LOGE(TAG, "Rewinding file: %s error. Data has not been saved.", SETTING_FILE_NAME);
                            }
                            fs_closeFile(fileID);

                            free(bufferFile);
                        }
                    }
                    
                }
                free(buffer);
            }

            /* Configuration file upload & save in the LFS */
            else if ((0 == strcmp(key, VALUE_QUERRY_UPLOAD))){
                char* buffer;
                buffer = malloc(MAX_CFG_FILE_SIZE);
                if (httpd_req_recv(req, buffer, contentLen) > 0){
                    httpd_resp_send(req, "File received", sizeof("File received"));

                    // JSON data must by found at multiplepart payload first to be assigned as a cJSON object.
                    char* bufferJson = strstr(buffer, "\r\n\r\n");  //find firs sequence before json data (confirmed in Wire Shark)
                    bufferJson += 4;    // Start after "\r\n\r\n"
                    char* bufferJsonEnd = strstr(buffer, "\r\n--"); //find the end of the json payload.
                    *bufferJsonEnd = '\0';  // cut out everything what is after json payload

                    cJSON *root;
                    root = cJSON_Parse(bufferJson);
                    
                    uint8_t     fileID;
                    if (ESP_OK == fs_findID(&fileID)) {
                        if (ESP_OK == fs_openFile(fileID, SETTING_FILE_NAME, WRITE_PERMISSION)) {
                            size_t writtenBytes = fs_writeFile(fileID, SETTING_FILE_NAME, cJSON_Print(root), strlen(cJSON_Print(root)));
                            if(0 < writtenBytes) {
                                ESP_LOGI(TAG, "%s File has been send replaced!", SETTING_FILE_NAME);
                            }
                            else {
                                ESP_LOGE(TAG, "Writting error: %d", writtenBytes);
                            }
                        }
                        else {
                            ESP_LOGE(TAG, "failed to open file: %s", SETTING_FILE_NAME);
                        }
                        fs_closeFile(fileID);
                    }
                    else {
                        ESP_LOGE(TAG, "No free file handler.");
                    }
                }
                free(buffer);
            }

            /* Unknown request? */
            else {
                ESP_LOGE(TAG, "%s is an unknown value", key);
            }
        }
        else {
            ESP_LOGE(TAG, "%s is not a querry key", KEY_QUERRY_ACTION);
        }
    }
    else {
        ESP_LOGE(TAG, "Querry has not been found.");
    }
    free(querryContent);
    return ESP_OK;
}