#include <Arduino.h>
#include "settings.h"

const char H_HOME_PAGE[] PROGMEM = {
  //  this is the page frame for all webpages
  //  all pages should be generated by replacing the {pagename}-fields width
  //  the webpages for the particular pages
  "<!DOCTYPE html>\r\n"
  "<html lang=\"de\">\r\n"
  "<head>\r\n"
  "<meta charset=\"utf-8\">\r\n"
  "<TITLE>{title}</TITLE>\r\n"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.5\">\r\n"
  "<style>\r\n"
  "body {\r\n"
    "  font-family: Verdana, sans-serif;\r\n"
    "  font-size: 87.5%;\r\n"
    "  background-color: #e8e8e8;\r\n"
    "  color: black;\r\n"
    "  padding: 0;\r\n"
    "  margin:0;\r\n"
  "}\r\n"

  "div1 {\r\n"
    "  width: 100%;\r\n"
    "  color: black;\r\n"
    "  padding: 10px;\r\n"
    "  position: absolute;\r\n"
    "  top: 60px;\r\n"
    "  left: 150px;\r\n"
  "}\r\n"

  ".vertical-menu a {\r\n"
    "  background-color: #b9d8e9;\r\n"
    "  color: black;\r\n"
    "  display: block;\r\n"
    "  padding: 12px;\r\n"
    "  text-decoration: none;\r\n"
  "}\r\n"

  ".vertical-menu {\r\n"
    "  width: 100px;\r\n"
    "  background-color: #e8e8e8;\r\n" //menuefarbe ???
  "}\r\n"

  ".vertical-menu a:hover {\r\n"
    "  background-color: #4CAFf0;\r\n"
  "}\r\n"

  ".vertical-menu a.active {\r\n"
    "  background-color: #4CAFf0;\r\n"
    "  color: white;\r\n"
  "}\r\n"

  "</style>\r\n"
  "</head>\r\n"
  "<body>\r\n"

  "<h1>{hostname}</h1>\r\n"
  "<div1>{content}</div1>\r\n"
  "<div class=\"vertical-menu\">\r\n"
  "<a href=\"info\">Info</a><br>"
  "{appmenu}"
  "{mainpage}"
  "{confpage}"
  "</div>\r\n"
  "</body>\r\n"
  "</html>\r\n"};

const char H_APPMENU[] PROGMEM = {
  "<a href=\"status\">Status</a><br>"
  "{appmenu}"};

const char H_STATUSMENU[] PROGMEM = {
  "<a href=\"on\">Ein</a><br>"
  "<a href=\"off\">Aus</a><br>"
   };

const char H_INFOMENU[] PROGMEM = {
  "<a href=\"config\">Config</a><br>"
  "<a href=\"showlog\">show log</a>"
  "<a href=\"showdir\">show Dir</a>"
  };

const char H_CONFMENUMEAS[] PROGMEM = {
  "<a href=\"meascyc\">MeasCyc</a>"
  "<a href=\"transcyc\">TransCyc</a>"
  "<a href=\"pagereload\">Pagereload</a>"
  "<a href=\"server\">Server</a>"
  "<a href=\"service\">Service</a>"
  "<a href=\"hostname\">Name</a>"
  "<a href=\"led\">LED</a>"
  "<a href=\"default\">set defaults</a>"
  "<a href=\"reset\">Reset</a></p>"
  "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"
  };


const char H_CONFMENU[] PROGMEM = {
  "<a href=\"transcyc\">TransCyc</a>"
  "<a href=\"pagereload\">Pagereload</a>"
  "<a href=\"server\">Server</a>"
  "<a href=\"service\">Service</a>"
  "<a href=\"hostname\">Name</a>"
  "<a href=\"led\">LED</a>"
  "<a href=\"default\">set defaults</a>"
  "<a href=\"reset\">Reset</a></p>"
  "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"
  };


const char H_RADIO_WIFI_START[] PROGMEM = {
  "<form>\r\n"
  "<p>mit welchem Netzwerk soll eine Verbindung hergestellt werden?</p>\r\n"};

const char H_RADIO_WIFI_LINE[] PROGMEM = {
  "<input type=\"radio\" name=\"SSID\" value=\"{SSID}\">  {CRYPT}{SSID}   ->   (Signal: {RSSI})\r\n"};

const char H_RADIO_WIFI_END[] PROGMEM = {
    "<br><br>\r\n"
    "<input type=\"password\" name=\"pass\"> </label>\r\n"
    "<input type=\"submit\" value=\"senden\">\r\n"
    "</form>\r\n"
  };

const char H_RADIO_LED_START[] PROGMEM = {
    "<form>\r\n"
    "<p> soll die LED ein- oder ausgeschaltet sein?</p>\r\n"
    "<input type=\"radio\" name=\"Led\" value=\"On\" {ontext}>  On\r\n"
    "<input type=\"radio\" name=\"Led\" value=\"Off\" {offtext}>  Off\r\n"};

const char H_RADIO_LED_END[] PROGMEM = {
    "<br><br>\r\n"
    "<input type=\"submit\" value=\"senden\">\r\n"
    "</form>\r\n"
  };

  const char H_NAME[] PROGMEM = {
    "<form>\r\n"
    "<p> {nametext}</p>\r\n"
    "<input type=\"text\" name=\"{urltext}\"> </label>\r\n"
    "<input type=\"submit\" value=\"senden\">\r\n"
    "</form>\r\n"
  };
