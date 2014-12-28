#!/usr/bin/ruby
require 'json'

Dir.chdir File.dirname($0)

BASE_CONF_FILE = "configuration.json"
IOS_CONF_FILE = "iOS/PebbleTimeSchedule/Resources/Settings/configuration.json"
PEBBLE_CONF_FILE_ORG = "Watch/_appinfo.json"
PEBBLE_CONF_FILE = "Watch/appinfo.json"

base_conf = JSON.parse(File.read(BASE_CONF_FILE))
ios_conf = {
  "parse_application_id" => base_conf["parse"]["application_id"],
  "parse_client_key"     => base_conf["parse"]["client_key"]
}
File.write(IOS_CONF_FILE, JSON.pretty_unparse(ios_conf))

pebble_conf = JSON.parse(File.read(PEBBLE_CONF_FILE_ORG))
pebble_conf["uuid"] = base_conf["pebble"]["uuid"]
pebble_conf["appKeys"]["parseJavascriptKey"] = base_conf["parse"]["javascript_key"]
File.write(PEBBLE_CONF_FILE, JSON.pretty_unparse(pebble_conf))

