#!/usr/bin/ruby
require 'json'

BASE_CONF_FILE = "configuration.json"
IOS_CONF_FILE = "iOS/PebbleTimeSchedule/Resources/Settings/configuration.json"

base_conf = JSON.parse(File.read(BASE_CONF_FILE))

ios_conf = {
  "parse_application_id" => base_conf["parse"]["ios_application_id"],
  "parse_client_key"     => base_conf["parse"]["ios_client_key"]
}

File.write(IOS_CONF_FILE, JSON.generate(ios_conf))
