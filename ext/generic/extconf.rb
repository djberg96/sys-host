require 'mkmf'
require 'rbconfig'

dir_config('host')

have_func('gethostid')
have_func('inet_ntop')
have_func('gethostent_r')

create_makefile('sys/host', 'sys')
