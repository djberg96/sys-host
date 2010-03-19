require 'mkmf'
require 'rbconfig'

dir_config('host')

have_func('gethostid')
have_func('inet_ntop')

have_library('nsl')
have_library('socket')

# These must come after the have_library() calls above.
have_func('gethostent_r')
have_func('gethostbyname_r')

create_makefile('sys/host', 'sys')
