require 'mkmf'
require 'rbconfig'

dir_config('host')

have_func('gethostid')
have_func('inet_ntop')
have_func('gethostent_r')
have_func('gethostent_r', 'netdb.h')
have_func('gethostbyname_r', 'netdb.h')

create_makefile('sys/host', 'sys')
