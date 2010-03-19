require 'mkmf'
require 'rbconfig'

dir_config('host')

have_func('gethostid')
have_func('inet_ntop')
have_func('gethostent_r')

case Config::CONFIG['host_os']
   when /bsd|darwin|powerpc|mach/i
      have_func('getipnodebyname')
   when /sunos|solaris/i
      have_library('nsl')
      have_library('socket')
      have_func('gethostbyname_r', 'netdb.h')
   when /linux/i
      have_func('gethostbyname_r', 'netdb.h')
      have_func('gethostent_r', 'netdb.h')
   else
end

create_makefile('sys/host', 'sys')
