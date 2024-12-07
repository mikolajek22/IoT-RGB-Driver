#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
#include "esp_netif.h"
#include <esp_http_server.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
extern esp_netif_ip_info_t connectionInfo;

/**
 * @brief This handler will be called after opening the web server via typing the ip address of the module.
 **/
extern esp_err_t http_handlers_getStartPage_EventHandler(httpd_req_t *req);

/**
 * @brief Changing between different pages
 **/
extern esp_err_t http_handlers_getPage_EventHandler(httpd_req_t *req);

/**
 * @brief While displaying main page this handler will be called to fetch information about used module and general application informations.
 **/
extern esp_err_t http_handlers_getInfo_EventHandler(httpd_req_t *req);

/**
 * @brief File downloading handler (at this moment only configuration file (settings.json))
 **/
extern esp_err_t http_handlers_getDownload_EventHandler(httpd_req_t *req);


extern esp_err_t http_handlers_postRGB_EventHandler(httpd_req_t *req);

/**
 * @brief Changing settings.json file. It handles upload of configuration file to the server (esp32), changing configuration file by typing information directly on the webserver. Reboot request handler
 **/
extern esp_err_t http_handlers_postConfiguration_EventHandler(httpd_req_t *req);