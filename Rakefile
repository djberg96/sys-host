require 'rake'
require 'rake/testtask'
require 'rbconfig'
include Config

desc "Clean the build files for the sys-host source for UNIX systems"
task :clean do
  rm_rf('.test-result') if File.exists?('.test-result')
  Dir['*.gem'].each{ |f| File.delete(f) }
   
  case Config::CONFIG['host_os']
    when /bsd|darwin/i
      dir = 'ext/bsd'
    when /sunos|solaris/i
      dir = 'ext/sunos'
    when /linux/i
      dir = 'ext/linux'
    else
      dir = 'ext/generic'
  end

  unless Config::CONFIG['host_os'] =~ /win32|mswin|dos|cygwin|mingw/i
     Dir.chdir(dir) do
      if Dir['*.o'].length > 0
        sh 'make distclean'
        Dir['sys/host.*'].each{ |f| rm(f) if File.extname(f) != '.c' }
      end
    end
  end
end

desc "Build the sys-host library on UNIX systems (but don't install it)"
task :build => [:clean] do
  case Config::CONFIG['host_os']
    when /bsd|darwin/i
      dir = 'ext/bsd'
    when /sunos|solaris/i
      dir = 'ext/sunos'
    when /linux/i
      dir = 'ext/linux'
    else
      dir = 'ext/generic'
  end

  unless Config::CONFIG['host_os'] =~ /win32|mswin|dos|cygwin|mingw/i
    Dir.chdir(dir) do
      ruby 'extconf.rb'
      sh 'make'
      cp 'host.' + Config::CONFIG['DLEXT'], 'sys'
    end
  end
end

desc "Run the example program"
task :example => [:build] do
  Dir.chdir('examples') do
    ruby 'example_sys_host.rb'
  end
end

desc 'Install the sys-host library'
task :install => [:build] do
  file = nil
  dir  = File.join(Config::CONFIG['sitelibdir'], 'sys')

  Dir.mkdir(dir) unless File.exists?(dir)

  case Config::CONFIG['host_os']
    when /mswin|win32|msdos|cygwin|mingw/i
      file = 'lib/windows/sys/host.rb'
    when /bsd|darwin/i
      Dir.chdir('ext/bsd'){ sh 'make install' }
    when /sunos|solaris/i
      Dir.chdir('ext/sunos'){ sh 'make install' }
    when /linux/i
      Dir.chdir('ext/linux'){ sh 'make install' }
    else
      Dir.chdir('ext/generic'){ sh 'make install' }
  end

  cp(file, dir, :verbose => true) if file
end

desc 'Uninstall the sys-host library'
task :uninstall do
  case Config::CONFIG['host_os']
    when /win32|mswin|dos|cygwin|mingw/i
      dir  = File.join(Config::CONFIG['sitelibdir'], 'sys')
      file = File.join(dir, 'host.rb')
    else
      dir  = File.join(Config::CONFIG['sitearchdir'], 'sys')
      file = File.join(dir, 'host.' + Config::CONFIG['DLEXT'])
  end

  rm(file) 
end

desc "Run the example sys-host program"
task :example => [:build] do
   Dir.mkdir('sys') unless File.exists?('sys')
   if CONFIG['host_os'].match('mswin')
      FileUtils.cp('lib/sys/windows.rb', 'lib/sys/host.rb')
      ruby '-Ilib examples/example_sys_host.rb'
   else
      ruby '-Iext examples/example_sys_host.rb'
   end
end

desc 'Run the test suite'
Rake::TestTask.new do |t|
  task :test => :build
  t.libs << 'test'
  t.test_files = FileList['test/test_sys_host.rb'] 
   
  case Config::CONFIG['host_os']
    when /mswin|msdos|cygwin|mingw/i
      t.libs << 'lib/windows'
    when /linux/i
      t.libs << 'ext/linux'
    when /sunos|solaris/i
      t.libs << 'ext/sunos'
    when /bsd|darwin/i
      t.libs << 'ext/bsd'
    else
      t.libs << 'ext/generic'
  end
end

namespace 'gem' do
  desc 'Create the sys-host gem file'
  task :create do
    spec = eval(IO.read('sys-host.gemspec'))

    case Config::CONFIG['host_os']
      when /bsd|darwin/i
         spec.files << 'ext/bsd/sys/host.c'
         spec.extra_rdoc_files << 'ext/bsd/sys/host.c'
         spec.extensions = ['ext/bsd/extconf.rb']
      when /linux/i
         spec.files << 'ext/bsd/sys/host.c'
         spec.extra_rdoc_files << 'ext/bsd/sys/host.c'
         spec.extensions = ['ext/bsd/extconf.rb']
      when /sunos|solaris/i
         spec.files << 'ext/bsd/sys/host.c'
         spec.extra_rdoc_files << 'ext/bsd/sys/host.c'
         spec.extensions = ['ext/bsd/extconf.rb']
      when /mswin|win32|dos|cygwin|mingw/i
         spec.require_paths = ['lib', 'lib/windows']
         spec.files += ['lib/windows/sys/host.rb']
    end

    Gem::Builder.new(spec).build 
  end

  desc 'Install the sys-host gem'
  task :install => [:create] do
    file = Dir["*.gem"].first
    sh "gem install #{file}"
  end
end
