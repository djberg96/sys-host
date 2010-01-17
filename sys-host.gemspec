require 'rubygems'
require 'rbconfig'
include Config

spec = Gem::Specification.new do |gem|
   gem.name        = 'sys-host'
   gem.version     = '0.6.2'
   gem.author      = 'Daniel J. Berger'
   gem.license     = 'Artistic 2.0'
   gem.email       = 'djberg96@gmail.com'
   gem.homepage    = 'http://www.rubyforge.org/projects/sysutils'
   gem.platform    = Gem::Platform::RUBY
   gem.summary     = 'Provides hostname and ip address info for a given host'
   gem.test_file   = 'test/test_sys_host.rb'
   gem.has_rdoc    = true
   gem.files       = Dir['**/*'].reject{ |f| f.include?('CVS') }

   gem.extra_rdoc_files = ['CHANGES', 'README', 'MANIFEST']
   gem.rubyforge_project = 'sysutils'
   gem.required_ruby_version = '>= 1.8.2'

   gem.add_development_dependency('test-unit', '>= 2.0.3')

   gem.description = <<-EOF
      The sys-host library provides information about the current machine that
      your program is running on. Specifically, it returns hostname, IP
      address information, aliases and so on.
   EOF
   
   if CONFIG['host_os'] =~ /mswin|win32|dos/i
      File.rename('lib/sys/windows.rb', 'lib/sys/host.rb')
      gem.files += ['lib/sys/host.rb']
      gem.platform = Gem::Platform::CURRENT
   else
      gem.files += Dir["ext/**/*.{c,h}"]
      gem.extensions = ['ext/extconf.rb']
      gem.extra_rdoc_files += Dir["ext/**/*.c"]
   end
end

Gem::Builder.new(spec).build
