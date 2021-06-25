require 'rubygems'
require 'rbconfig'
include Config

# Some additional specification options are handled by the gem:create task.

Gem::Specification.new do |spec|
  spec.name        = 'sys-host'
  spec.version     = '0.6.3'
  spec.author      = 'Daniel J. Berger'
  spec.license     = 'Artistic 2.0'
  spec.email       = 'djberg96@gmail.com'
  spec.homepage    = 'http://www.rubyforge.org/projects/sysutils'
  spec.platform    = Gem::Platform::RUBY
  spec.summary     = 'Provides hostname and ip address info for a given host'
  spec.test_file   = 'test/test_sys_host.rb'

  spec.files = %w[
    CHANGES
    MANIFEST
    README.md
    Rakefile
    sys-host.gemspec
    doc/host.txt
    examples/example_sys_host.rb
    test/test_sys_host.rb
  ] 

  spec.extra_rdoc_files  = ['CHANGES', 'README.md', 'MANIFEST']

  spec.add_development_dependency('test-unit')

  spec.description = <<-EOF
    The sys-host library provides information about the current machine that
    your program is running on. Specifically, it returns hostname, IP
    address information, aliases and so on.
  EOF
end
