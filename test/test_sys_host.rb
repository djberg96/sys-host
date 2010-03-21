###########################################################################
# test_sys_host.rb
#
# Test suite for sys-host, all platforms. You should run these tests via
# the 'rake test' task.
###########################################################################
require 'rubygems'
gem 'test-unit'

require 'sys/host'
require 'test/unit'
require 'rbconfig'
include Config
include Sys

class TC_Sys_Host < Test::Unit::TestCase
  def self.startup
    if CONFIG['host_os'] =~ /mswin|solaris|linux|bsd|mach|darwin|osx/i
      @@info_supported = true
    else
      @@info_supported = false
    end

    @@windows = CONFIG['host_os'].match('mswin')
  end

  def test_version
    assert_equal('0.6.3', Host::VERSION)
  end

  def test_hostname_basic
    assert_respond_to(Host, :hostname)
    assert_nothing_raised{ Host.hostname }
  end

  def test_hostname
    assert_kind_of(String, Host.hostname)
    assert_equal(`hostname`.chomp, Host.hostname) # sanity check
  end

  def test_hostname_expected_errors
    assert_raise(ArgumentError){ Host.hostname(true) }
  end

  def test_ip_addr_basic
    assert_respond_to(Host, :ip_addr)
    assert_nothing_raised{ Host.ip_addr }
  end

  def test_ip_addr
    assert_kind_of(Array, Host.ip_addr)
    assert_kind_of(String, Host.ip_addr.first)
  end

  def test_ip_addr_expected_errors
    assert_raise(ArgumentError){ Host.ip_addr(true) }
  end

  def test_host_id_basic
    assert_respond_to(Host, :host_id)
    assert_nothing_raised{ Host.host_id }
  end

  def test_host_id
    type = @@windows ? String : Integer
    assert_kind_of(type, Host.host_id)
  end

  def test_host_id_expected_errors
    assert_raise(ArgumentError){ Host.host_id(true) }
  end

  def test_info_basic
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_respond_to(Host, :info)
    assert_nothing_raised{ Host.info }
  end

  def test_info
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_kind_of(Array, Host.info)
    assert_kind_of(Struct::HostInfo, Host.info.first)
  end

  def test_info_name_member
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_true(Host.info.first.members.map{ |e| e.to_s }.include?('name'))
    assert_kind_of(String, Host.info.first.name)
    assert_true(Host.info.first.name.size > 0)
  end

  def test_info_addr_type_member
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_true(Host.info.first.members.map{ |e| e.to_s }.include?('addr_type'))
    assert_kind_of(Fixnum, Host.info.first.addr_type)
    assert_true(Host.info.first.addr_type >= 0)
  end

  def test_info_aliases_member
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_true(Host.info.first.members.map{ |e| e.to_s }.include?('aliases'))
    assert_kind_of(Array, Host.info.first.aliases)
  end

  def test_info_length_member
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_true(Host.info.first.members.map{ |e| e.to_s }.include?('length'))
    assert_kind_of(Fixnum, Host.info.first.length)
    assert_true(Host.info.first.length >= 4)
  end

  def test_info_addr_list_member
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_true(Host.info.first.members.map{ |e| e.to_s }.include?('addr_list'))
    assert_kind_of(Array, Host.info.first.addr_list)
    assert_true(Host.info.first.addr_list.first.length > 0)
  end

  def test_info_high_iteration
    omit_unless(@@info_supported, 'info test skipped on this platform')
    assert_nothing_raised{ 100.times{ Host.info } }
  end

  def self.shutdown
    @@info_supported = nil
    @@windows = nil
  end
end
