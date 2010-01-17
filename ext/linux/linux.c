/******************************************************************************
 * linux.c (host.c) - Linux specific version for sys-host
 *
 * Author: Daniel Berger
 *****************************************************************************/
#include <ruby.h>
#include <constants.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"
{
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

   /* If you get this exception, you've got big problems */
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
   char str[INET_ADDRSTRLEN];
   char **pptr;
   struct hostent* hp;
   int rv;
   VALUE rbResults = rb_ary_new();
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
      rb_ary_push(rbResults,
         rb_str_new2(inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str))));
   }
#else
   for(n = 0; hp->h_addr_list[n] != NULL; n++){
      memcpy(&ipa.s_addr, hp->h_addr_list[n], hp->h_length);
      rb_ary_push(rbResults, rb_str_new2(inet_ntoa(ipa)));
   }
#endif

   return rbResults;
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
 * * addr_type (Integer)
 * * length    (Integer)
 * * addr_list (Array)
 */
static VALUE host_info(VALUE klass){
   VALUE v_hostinfo = Qnil;
   VALUE v_addr     = rb_ary_new();
   VALUE v_array    = rb_ary_new();
   VALUE v_aliases  = rb_ary_new();

   sethostent(0);

#ifdef HAVE_GETHOSTENT_R
   struct hostent host;
   struct hostent* result;
   char buf[HOSTENT_BUF];
   int err;

   while(!gethostent_r(&host, buf, sizeof(buf), &result, &err)){
      while(*result->h_aliases){
         rb_ary_push(v_aliases, rb_str_new2(*result->h_aliases));
         *result->h_aliases++;
      }

      while(*result->h_addr_list){
         inet_ntop(result->h_addrtype, *result->h_addr_list, buf, sizeof(buf));
         rb_ary_push(v_addr, rb_str_new2(buf));
         *result->h_addr_list++;
      }

      /* We need to dup the arrays because Ruby is holding onto the same
       * reference.
       */
      v_hostinfo = rb_struct_new(sHostInfo,
         rb_str_new2(result->h_name),
         rb_ary_dup(v_aliases),
         INT2FIX(result->h_addrtype),
         INT2FIX(result->h_length),
         rb_ary_dup(v_addr)
      );         
#else
   struct hostent* host;
   char buf[INET_ADDRSTRLEN];

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
         rb_ary_dup(v_aliases),
         INT2FIX(host->h_addrtype),
         INT2FIX(host->h_length),
         rb_ary_dup(v_addr)
      );         
#endif

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

   /* The Sys module serves as a toplevel namespace, nothing more. */
   mSys  = rb_define_module("Sys");

   /* The Host class encapsulates information about your machine, such as
    * the host name and IP address.
    */
   cHost = rb_define_class_under(mSys, "Host", rb_cObject);

   /* This error is raised if any of the Host methods fail. */
   cHostError = rb_define_class_under(cHost, "Error", rb_eStandardError);

   /* 0.6.1: The version of this library. This is a string, not a number. */
   rb_define_const(cHost, "VERSION", rb_str_new2(SYS_HOST_VERSION));

   /* Structs */
   sHostInfo = rb_struct_define("HostInfo", "name", "aliases", "addr_type",
      "length", "addr_list", NULL
   );

   /* Class Methods */
   rb_define_singleton_method(cHost, "hostname", host_hostname, 0);
   rb_define_singleton_method(cHost, "ip_addr", host_ip_addr, 0);
   rb_define_singleton_method(cHost, "info", host_info, 0);

#ifdef HAVE_GETHOSTID
   rb_define_singleton_method(cHost, "host_id", host_host_id, 0);
#endif
}

#ifdef __cplusplus
}
#endif
