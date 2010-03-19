####################################################################
# example_sys_host.rb
#
# Generic test script for general futzing. Use the 'rake example'
# task to run this.
#
# Modify as you see fit.
####################################################################
require 'sys/host'
require 'pp'
include Sys

puts "VERSION: " + Host::VERSION
puts "Hostname: " + Host.hostname
puts "IP Addresses : " + Host.ip_addr.join(',')
puts "Host ID: " + Host.host_id.to_s

puts "Info: "
pp Host.info
