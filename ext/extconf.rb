require 'mkmf'
require 'fileutils'

dir_config('host')

have_func('gethostid')
have_func('inet_ntop')
Dir.mkdir('sys') unless File.exists?('sys')

case RUBY_PLATFORM
   when /bsd|darwin|powerpc|mach/i
      have_func('getipnodebyname')
      FileUtils.cp('bsd/bsd.c', 'sys/host.c')
   when /sunos|solaris/i
      have_library('nsl')
      have_library('socket')
      have_func('gethostbyname_r', 'netdb.h')
      have_func('gethostent_r', 'netdb.h')
      FileUtils.cp('sunos/sunos.c', 'sys/host.c')
   when /linux/i
      have_func('gethostbyname_r', 'netdb.h')
      have_func('gethostent_r', 'netdb.h')
      FileUtils.cp('linux/linux.c', 'sys/host.c')
   when /win32|windows|dos|mingw|cygwin/i
      STDERR.puts 'Run the "rake install" task to install on MS Windows'
   else
      FileUtils.cp('generic/generic.c', 'sys/host.c')
end

# Already grabbed this from netdb.h on Solaris
unless RUBY_PLATFORM =~ /sunos|solaris/i
   have_func('gethostent_r')
end

create_makefile('sys/host', 'sys')
