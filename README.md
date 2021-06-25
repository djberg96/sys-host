# DEPRECATION WARNING
This library is deprecated and has not been updated for many years. Please do
not use this library. It was mainly a learning exercise for C extensions from my
earliest days of Ruby when I was just learning how to write them. I'm fairly
certain anything this library does the socket library in stdlib can already do.

## Description
An interface for getting host information.

## Installation
`gem install sys-host`

## Synopsis
```ruby
require 'sys/host'
include Sys

p Host.hostname
p Host.ip_addr

Host.info{ |h|
  p h
}
```

## Documentation
See the doc/host.txt file for more information.
