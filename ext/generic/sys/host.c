/******************************************************************************
 * host.c - Source for the sys-host library for Unix platforms that are not
 * Linux, BSD, OS X, or Solaris.
 *
 * Author: Daniel Berger
 *
 * This source is mostly borrowed from the linux source for sys-host, except
 * that it does not support the gethostbyname_r() method.
 *****************************************************************************/
#include <ruby.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SYS_HOST_VERSION "0.6.2"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifndef HOSTENT_BUF
#define HOSTENT_BUF 8192
#endif

VALUE cHostError, sHostInfo;

/*
 * Sys::Host.hostname
 *
 * Returns the hostname of the current machine.
 */
static VALUE host_hostname()
{
  char host_name[MAXHOSTNAMELEN];

  // If you get this exception, you've got big problems
  if(gethostname(host_name, MAXHOSTNAMELEN) != 0)
    rb_raise(cHostError, "gethostname() call failed");

  return rb_str_new2(host_name);
}

/*
 * Sys::Host.ip_addr
 *
 * Returns an array of IP addresses associated with the current hostname.
 */
static VALUE host_ip_addr()
{
   char host_name[MAXHOSTNAMELEN];
   char str[INET_ADDRSTRLEN];
   char **pptr;
   struct hostent* hp;
   VALUE v_results = rb_ary_new();
#ifndef HAVE_INET_NTOP
   struct in_addr ipa;
   int n;
#endif

   if(gethostname(host_name, MAXHOSTNAMELEN) != 0)
      rb_raise(cHostError, "gethostname() call failed");

   hp = gethostbyname(host_name);

   if(!hp)
      rb_raise(cHostError, "gethostbyname() call failed");

   pptr = hp->h_addr_list;

#ifdef HAVE_INET_NTOP
   for( ; *pptr != NULL; pptr++) {
      rb_ary_push(v_results,
         rb_str_new2(inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str))));
   }
#else
   for(n = 0; hp->h_addr_list[n] != NULL; n++) {
      memcpy(&ipa.s_addr, hp->h_addr_list[n], hp->h_length);
      rb_ary_push(v_results, rb_str_new2(inet_ntoa(ipa)));
   }
#endif
   return v_results;
   
}

#ifdef HAVE_GETHOSTID
/*
 * Sys::Host.host_id
 *
 * Returns the host id of the current machine.
 */
static VALUE host_host_id(){
   return ULL2NUM(gethostid());
}
#endif

/*
 * Sys::Host.info
 * Sys::Host.info{ |i| ... }
 *
 * Yields a HostInfo struct containing various bits of information about
 * the local machine for each entry in the hosts table.  In non-block form,
 * returns an array of HostInfo structs.
 *
 * The Struct::HostInfo struct contains 5 fields:
 *
 * * name      (String)
 * * aliases   (Array)
 * * addr_type (Integer)
 * * length    (Integer)
 * * addr_list (Array)
 */
static VALUE host_info(VALUE klass){
   struct hostent* host;
   char buf[INET_ADDRSTRLEN];
   VALUE v_hostinfo;
   VALUE v_addr    = rb_ary_new();
   VALUE v_array   = rb_ary_new();
   VALUE v_aliases = rb_ary_new();

   sethostent(0);

   while((host = gethostent())){
      while(*host->h_aliases){
         rb_ary_push(v_aliases, rb_str_new2(*host->h_aliases));
         *host->h_aliases++;
      }

      while(*host->h_addr_list){
         inet_ntop(host->h_addrtype, *host->h_addr_list, buf, sizeof(buf));
         rb_ary_push(v_addr, rb_str_new2(buf));
         *host->h_addr_list++;
      }

      v_hostinfo = rb_struct_new(sHostInfo,
         rb_str_new2(host->h_name),
         v_aliases,
         INT2FIX(host->h_addrtype),
         INT2FIX(host->h_length),
         v_addr
      );

      if(rb_block_given_p())
         rb_yield(v_hostinfo);
      else
         rb_ary_push(v_array, v_hostinfo);

      rb_ary_clear(v_aliases);   
      rb_ary_clear(v_addr);
   }

   endhostent();

   if(rb_block_given_p())
      return Qnil;

   return v_array;
}

void Init_host()
{
  VALUE mSys, cHost;

  /* The Sys module serves as a toplevel namespace, nothing more. */ 
  mSys = rb_define_module("Sys");

  /* The Host class encapsulates information about your machine, such as
   * the host name and IP address.
   */
  cHost = rb_define_class_under(mSys, "Host", rb_cObject);

  /* This error is raised if any of the Host methods fail. */
  cHostError = rb_define_class_under(cHost, "Error", rb_eStandardError);

  /* 0.6.2: The version of this library. This is a string, not a number. */
  rb_define_const(cHost,"VERSION",rb_str_new2(SYS_HOST_VERSION));

  // Singleton methods

  rb_define_singleton_method(cHost, "hostname", host_hostname, 0);
  rb_define_singleton_method(cHost, "ip_addr", host_ip_addr, 0);
  rb_define_singleton_method(cHost, "info", host_info, 0);

#ifdef HAVE_GETHOSTID
  rb_define_singleton_method(cHost, "host_id", host_host_id, 0);
#endif

  // Struct definitions

  sHostInfo = rb_struct_define(
    "HostInfo",
    "name",
    "aliases",
    "addr_type",
    "length",
    "addr_list",
    NULL
  );
}
