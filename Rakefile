require 'rake'
require 'rake/clean'
require 'rake/testtask'
require 'rbconfig'
include Config

desc "Clean the build files for the sys-host source for UNIX systems"
task :clean do
   Dir.chdir('ext') do
      unless RUBY_PLATFORM.match('mswin')
         sh 'make distclean' if File.exists?('host.o')
         FileUtils.rm_rf('host.c') if File.exists?('host.c')
         FileUtils.rm_rf('sys') if File.exists?('sys')
      end
   end
   FileUtils.rm_rf('sys') if File.exists?('sys')
end

desc "Build the sys-host package on UNIX systems (but don't install it)"
task :build => [:clean] do
   Dir.chdir('ext') do
      ruby 'extconf.rb'
      sh 'make'
      build_file = 'host.' + Config::CONFIG['DLEXT']
      Dir.mkdir('sys') unless File.exists?('sys')
      FileUtils.cp(build_file, 'sys')
   end
end

desc "Run the example program"
task :example => [:build] do
   ruby 'host_test.rb'
end

if CONFIG['host_os'].match('mswin')
   desc "Install the sys-host package (non-gem)"
   task :install do
      install_dir = File.join(CONFIG['sitelibdir'], 'sys')
      install_file = File.join(install_dir, 'host.rb')
      Dir.mkdir(install_dir) unless File.exists?(install_dir)
      FileUtils.cp('lib/sys/windows.rb', install_file, :verbose => true)
   end
else
   desc "Install the sys-host package (non-gem)"
   task :install => [:build] do
      Dir.chdir('ext') do
         sh 'make install'
      end
   end
end

desc "Install the sys-host package as a gem"
task :install_gem do
   ruby 'sys-host.gemspec'
   file = Dir["sys-host*.gem"].last
   sh "gem install #{file}"
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

Rake::TestTask.new do |t|
   if CONFIG['host_os'].match('mswin')
      FileUtils.cp('lib/sys/windows.rb', 'lib/sys/host.rb')
   else
      task :test => :build
      t.libs << 'ext'
      t.libs.delete('lib')
   end
end
