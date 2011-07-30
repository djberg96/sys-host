/******************************************************************************
 * host.c - Linux code for the sys-host library.
 *
 * Author: Daniel Berger
 *****************************************************************************/
#include <ruby.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SYS_HOST_VERSION "0.6.3"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

#ifndef HOSTENT_BUF
#define HOSTENT_BUF 8192
#endif

VALUE cHostError, sHostInfo;

/* Returns the hostname of the current host.  This may or not return the
 * FQDN, depending on your system.
 */
static VALUE host_hostname()
{
  char host_name[MAXHOSTNAMELEN];
  int rv;

  rv = gethostname(host_name, sizeof(host_name));

  // If you get this exception, you've got big problems.
  if(rv != 0)
    rb_raise(cHostError, "gethostname() call failed");

  return rb_str_new2(host_name);
}

/* Returns a list of IP addresses for the current host (yes, it is
 * possible to have more than one).
 */
static VALUE host_ip_addr()
{
  char host_name[MAXHOSTNAMELEN];
  char str[INET6_ADDRSTRLEN];
  char **pptr;
  struct hostent* hp;
  int rv;
  VALUE v_array = rb_ary_new();
#ifndef HAVE_INET_NTOP
  struct in_addr ipa;
  int n;
#endif

  rv = gethostname(host_name, sizeof(host_name));

  if(rv != 0)
    rb_raise(cHostError, "gethostname() call failed");

#ifdef HAVE_GETHOSTBYNAME_R
{
  struct hostent hpbuf;
  char strbuf[HOSTENT_BUF];
  int err;
  size_t no_op = HOSTENT_BUF;
  gethostbyname_r(host_name, &hpbuf, strbuf, no_op, &hp, &err);
}
#else
  hp = gethostbyname(host_name);
#endif

  if(!hp)
    rb_raise(cHostError,"gethostbyname() call failed");

  pptr = hp->h_addr_list;

#ifdef HAVE_INET_NTOP
  for( ; *pptr != NULL; pptr++){
    rb_ary_push(v_array,
      rb_str_new2(inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str))));
  }
#else
  for(n = 0; hp->h_addr_list[n] != NULL; n++){
    memcpy(&ipa.s_addr, hp->h_addr_list[n], hp->h_length);
    rb_ary_push(v_array, rb_str_new2(inet_ntoa(ipa)));
  }
#endif

  return v_array;
}

/* :call-seq:
 *    Sys::Host.info
 *    Sys::Host.info{ |i| ... }
 *
 * Yields a HostInfo struct containing various bits of information about
 * the local machine for each entry in the hosts table.  In non-block form,
 * returns an array of HostInfo structs.
 *
 * The Struct::HostInfo struct contains 5 fields:
 *
 * * name      (String)
 * * aliases   (Array)
 * * addr_type (Integer) => Typically 2 (AF_INET) or 28 (AF_INET6)
 * * length    (Integer) => Typically 4 (IPv4) or 16 (IPv6)
 * * addr_list (Array)
 */
static VALUE host_info(VALUE klass){
  char ibuf[INET6_ADDRSTRLEN];
  struct hostent* host;
  VALUE v_hostinfo, v_addr, v_aliases;
  VALUE v_array = rb_ary_new();

  sethostent(0);

#ifdef HAVE_GETHOSTENT_R
  struct hostent temp;
  char sbuf[HOSTENT_BUF];
  int err;

  while(!gethostent_r(&temp, sbuf, HOSTENT_BUF, &host, &err)){
#else
  while((host = gethostent())){
#endif
    v_aliases = rb_ary_new();
    v_addr = rb_ary_new();

    while(*host->h_aliases){
      rb_ary_push(v_aliases, rb_str_new2(*host->h_aliases));
      *host->h_aliases++;
    }

    while(*host->h_addr_list){
      inet_ntop(host->h_addrtype, *host->h_addr_list, ibuf, sizeof(ibuf));
      rb_ary_push(v_addr, rb_str_new2(ibuf));
      *host->h_addr_list++;
    }

    v_hostinfo = rb_struct_new(sHostInfo,
      rb_str_new2(host->h_name),
      v_aliases,
      INT2FIX(host->h_addrtype),
      INT2FIX(host->h_length),
      v_addr
    );

    OBJ_FREEZE(v_hostinfo);
    rb_ary_push(v_array, v_hostinfo);
  }      

  endhostent();

  return v_array;
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

void Init_host()
{
  VALUE mSys, cHost;

  /* The Sys module serves only as a toplevel namespace. */
  mSys  = rb_define_module("Sys");

  /* The Host class encapsulates information about your machine, such as
   * the host name and IP address.
   */
  cHost = rb_define_class_under(mSys, "Host", rb_cObject);

  /* This error is raised if any of the Sys::Host methods fail. */
  cHostError = rb_define_class_under(cHost, "Error", rb_eStandardError);

  // Singleton methods

  rb_define_singleton_method(cHost, "hostname", host_hostname, 0);
  rb_define_singleton_method(cHost, "ip_addr", host_ip_addr, 0);
  rb_define_singleton_method(cHost, "info", host_info, 0);

#ifdef HAVE_GETHOSTID
  rb_define_singleton_method(cHost, "host_id", host_host_id, 0);
#endif

  // Structs definitions

  sHostInfo = rb_struct_define(
    "HostInfo",
    "name",
    "aliases",
    "addr_type",
    "length",
    "addr_list",
     NULL
  );

  /* 0.6.3: The version of this library. This is a string, not a number. */
  rb_define_const(cHost, "VERSION", rb_str_new2(SYS_HOST_VERSION));
}
