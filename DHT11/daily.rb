# # -*- coding: utf-8 -*-
# require 'rubygems'
# require 'bundler'

# Bundler.require(:default)

output = `sudo ~/Github/Raspberry-sensor/DHT11/temp`
match_data = output.match /RH:(?<RH>\d+),TEMP:(?<TEMP>\d+)/

humidity = match_data['RH']
temperature = match_data['TEMP']

time = Time.now.strftime('%Y-%m-%d %H:%M:%S')

humidity_data = {timestamp: time, value: humidity}.to_s
temperature_data = {timestamp: time, value: temperature}.to_s

json_temperature = %Q[{\\"timestamp\\": \\"#{time}\\", \\"value\\": #{temperature}}]
json_humidity = %Q[{\\"timestamp\\": \\"#{time}\\", \\"value\\": #{humidity}}]

`curl -X POST -d "#{json_temperature}" -H "Content-Type: application/json" -H "U-ApiKey:9a820441dbb60753618fe667c625232f" http://api.yeelink.net/v1.0/device/342655/sensor/380108/datapoints`
`curl -X POST -d "#{json_humidity}" -H "Content-Type: application/json" -H "U-ApiKey:9a820441dbb60753618fe667c625232f" http://api.yeelink.net/v1.0/device/342655/sensor/380676/datapoints`


