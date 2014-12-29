#!/usr/bin/ruby
require 'json'

Dir.chdir File.dirname($0)

BASE_CONF_FILE = "configuration.json"

#
# iOS app
#
IOS_CONF_FILE = "iOS/PebbleTimeSchedule/Resources/Settings/configuration.json"
base_conf = JSON.parse(File.read(BASE_CONF_FILE))
ios_conf = {
  "parse_application_id" => base_conf["parse"]["application_id"],
  "parse_client_key"     => base_conf["parse"]["client_key"]
}
File.write(IOS_CONF_FILE, JSON.pretty_unparse(ios_conf))

#
# Watch app
#
PEBBLE_CONF_FILE_ORG = "Watch/_appinfo.json"
PEBBLE_CONF_FILE = "Watch/appinfo.json"

pebble_conf = JSON.parse(File.read(PEBBLE_CONF_FILE_ORG))
pebble_conf["uuid"] = base_conf["pebble"]["uuid"]
#pebble_conf["appKeys"]["parseApplicationId"] = base_conf["parse"]["application_id"]
#pebble_conf["appKeys"]["parseRestKey"] = base_conf["parse"]["rest_key"]
#pebble_conf["appKeys"]["parseJavascriptKey"] = base_conf["parse"]["javascript_key"]
File.write(PEBBLE_CONF_FILE, JSON.pretty_unparse(pebble_conf))

PEBBLE_CONF_JS = "Watch/src/js/libs/configurations.json"
js_conf = {
  "parse_application_id" => base_conf["parse"]["application_id"],
  "parse_rest_key"       => base_conf["parse"]["rest_key"]
}
File.write(PEBBLE_CONF_JS, "var configurations = " + JSON.pretty_unparse(js_conf) + ";")

#
# Parse Cloud Code
#
PARSE_CC_CONF_FILE_ORG = "Parse/config/_global.json"
PARSE_CC_CONF_FILE = "Parse/config/global.json"

cc_conf = JSON.parse(File.read(PARSE_CC_CONF_FILE_ORG))
cc_conf["applications"]["Pebble TimeSchedule"]["applicationId"] = base_conf["parse"]["application_id"]
cc_conf["applications"]["Pebble TimeSchedule"]["masterKey"] = base_conf["parse"]["master_key"]
File.write(PARSE_CC_CONF_FILE, JSON.pretty_unparse(cc_conf))

