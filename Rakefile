require 'rake'
require 'rake/clean'
require 'rake/testtask'
require 'rbconfig'
include Config

CLEAN.include(
  '**/*.gem',               # Gem files
  '**/*.rbc',               # Rubinius
  '**/*.o',                 # C object file
  '**/*.log',               # Ruby extension build log
  '**/Makefile',            # C Makefile
  '**/conftest.dSYM',       # OS X build directory
  "**/*.#{CONFIG['DLEXT']}" # C shared object
)

desc "Build the sys-host library on UNIX systems"
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

  unless File::ALT_SEPARATOR
    Dir.chdir(dir) do
      ruby 'extconf.rb'
      sh 'make'
      cp 'host.' + Config::CONFIG['DLEXT'], 'sys'
    end
  end
end

desc "Run the example sys-host program"
task :example => [:build] do
   Dir.mkdir('sys') unless File.exists?('sys')
   if File::ALT_SEPARATOR
      ruby '-Ilib/windows examples/example_sys_host.rb'
   else
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
      ruby "-I#{dir} examples/example_sys_host.rb"
   end
end

desc 'Run the test suite'
Rake::TestTask.new do |t|
  task :test => :build
  t.libs << 'test'
  t.test_files = FileList['test/test_sys_host.rb'] 
   
  case Config::CONFIG['host_os']
    when /mswin|msdos|cygwin|mingw|windows/i
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
  task :create => [:clean] do
    spec = eval(IO.read('sys-host.gemspec'))

    case Config::CONFIG['host_os']
      when /bsd|darwin/i
         spec.files << 'ext/bsd/sys/host.c'
         spec.extra_rdoc_files << 'ext/bsd/sys/host.c'
         spec.extensions = ['ext/bsd/extconf.rb']
      when /linux/i
         spec.files << 'ext/linux/sys/host.c'
         spec.extra_rdoc_files << 'ext/linux/sys/host.c'
         spec.extensions = ['ext/linux/extconf.rb']
      when /sunos|solaris/i
         spec.files << 'ext/sunos/sys/host.c'
         spec.extra_rdoc_files << 'ext/sunos/sys/host.c'
         spec.extensions = ['ext/sunos/extconf.rb']
      when /mswin|win32|dos|cygwin|mingw|windows/i
         spec.platform = Gem::Platform::CURRENT
         spec.require_paths = ['lib', 'lib/windows']
         spec.files += ['lib/windows/sys/host.rb']
      else
         spec.files << 'ext/generic/sys/host.c'
         spec.extra_rdoc_files << 'ext/generic/sys/host.c'
         spec.extensions = ['ext/generic/extconf.rb']
    end

    Gem::Builder.new(spec).build 
  end

  desc 'Install the sys-host gem'
  task :install => [:create] do
    file = Dir["*.gem"].first
    sh "gem install #{file}"
  end
end

task :default => :test
