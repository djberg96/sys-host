require 'win32ole'
require 'socket'
require 'time'

module Sys
   class Host
      class Error < StandardError; end
      
      VERSION = '0.6.1'
      
      fields = %w/
         arp_always_source_route?
         arp_use_ether_snap?
         caption
         database_path
         dead_gw_detect_enabled?
         default_ip_gateway
         default_tos
         default_ttl
         description
         dhcp_enabled?
         dhcp_lease_expires
         dhcp_lease_obtained
         dhcp_server
         dns_domain
         dns_domain_suffix_search_order
         dns_enabled_for_wins_resolution?
         dns_host_name
         dns_server_search_order
         domain_dns_registration_enabled?
         forward_buffer_memory
         full_dns_registration_enabled?
         gateway_cost_metric
         igmp_level
         index
         ip_address
         ip_connection_metric
         ip_enabled?
         ip_filter_security_enabled?
         ip_port_security_enabled?
         ip_sec_permit_ip_protocols
         ip_sec_permit_tcp_ports
         ip_sec_permit_udp_ports
         ip_subnet
         ip_use_zero_broadcast
         ipx_address
         ipx_enabled?
         ipx_frame_type
         ipx_media_type
         ipx_network_number
         ipx_virtual_net_number
         keep_alive_interval
         keep_alive_time
         mac_address
         mtu
         num_forward_packets
         pmtubh_detect_enabled?
         pmtu_discovery_enabled?
         service_name
         setting_id
         tcpip_netbios_options
         tcp_max_connect_retransmissions
         tcp_max_data_retransmissions
         tcp_num_connections
         tcp_use_rfc1122_urgent_pointer?
         tcp_window_size
         wins_enable_lm_hosts_lookup?
         wins_host_lookup_file
         wins_primary_server
         wins_scope_id
         wins_secondary_server
      /
      
      HostInfo = Struct.new("HostInfo", *fields)
      
      # Yields a HostInfo struct for each network adapter on 'host', or an array
      # of HostInfo struct's in non-block form.  The exact members of this struct
      # vary depending on your platform.
      # 
      def self.info(host=Socket.gethostname)
         array = [] unless block_given?
         cs = "winmgmts://#{host}/root/cimv2"

         begin
            wmi = WIN32OLE.connect(cs)
         rescue WIN32OLERuntimeError => err
            raise Error, err
         else
            query = "
               select * from Win32_NetworkAdapterConfiguration
               where IPEnabled = True
            "
            wmi.ExecQuery(query).each{ |nic|
               if nic.DHCPLeaseExpires
                  dhcp_expires = Time.parse(nic.DHCPLeaseExpires.split('.')[0])
               else
                  dhcp_expires = nil
               end
               
               if nic.DHCPLeaseObtained
                  dhcp_lease_obtained = Time.parse(nic.DHCPLeaseObtained.split('.')[0])
               else
                  dhcp_lease_obtained = nil
               end
               
               igmp_level = nil
               case nic.IGMPLevel
                  when 0
                     igmp_level = "None"
                  when 1
                     igmp_level = "IP"
                  when 2
                     igmp_level = "IP & IGMP"
               end
                  
               ipx_frame_type = nil
               case nic.IPXFrameType
                  when 0
                     ipx_frame_type = "Ethernet II"
                  when 1
                     ipx_frame_type = "Ethernet 802.3"
                  when 2
                     ipx_frame_type = "Ethernet 802.2"
                  when 3
                     ipx_frame_type = "Ethernet SNAP"
                  when 255
                     ipx_frame_type = "AUTO"
               end
               
               ipx_media_type = nil
               case nic.IPXMediaType
                  when 0
                     ipx_media_type = "Ethernet"
                  when 1
                     ipx_media_type = "Token Ring"
                  when 2
                     ipx_media_type = "FDDI"
                  when 8
                     ipx_media_type = "ARCNET"
               end
               
               tcpip_netbios_options = nil
               case nic.TcpipNetbiosOptions
                  when 0
                     tcpip_netbios_options = "EnableNetbiosViaDhcp"
                  when 1
                     tcpip_netbios_options = "EnableNetbios"
                  when 2
                     tcpip_netbios_options = "DisableNetbios"
               end
               
               struct = HostInfo.new(
                  nic.ArpAlwaysSourceRoute,
                  nic.ArpUseEtherSNAP,
                  nic.Caption,
                  nic.DatabasePath,
                  nic.DeadGWDetectEnabled,
                  nic.DefaultIPGateway,
                  nic.DefaultTOS,
                  nic.DefaultTTL,
                  nic.Description,
                  nic.DHCPEnabled,
                  dhcp_expires,
                  dhcp_lease_obtained,
                  nic.DHCPServer,
                  nic.DNSDomain,
                  nic.DNSDomainSuffixSearchOrder,
                  nic.DNSEnabledForWINSResolution,
                  nic.DNSHostName,
                  nic.DNSServerSearchOrder,
                  nic.DomainDNSRegistrationEnabled,
                  nic.ForwardBufferMemory,
                  nic.FullDNSRegistrationEnabled,
                  nic.GatewayCostMetric,
                  igmp_level,
                  nic.Index,
                  nic.IpAddress,
                  nic.IPConnectionMetric,
                  nic.IPEnabled,
                  nic.IPFilterSecurityEnabled,
                  nic.IPPortSecurityEnabled,
                  nic.IPSecPermitIPProtocols,
                  nic.IPSecPermitTCPPorts,
                  nic.IPSecPermitUDPPorts,
                  nic.IPSubnet,
                  nic.IPUseZeroBroadcast,
                  nic.IPXAddress,
                  nic.IPXEnabled,
                  ipx_frame_type,
                  ipx_media_type,
                  nic.IPXNetworkNumber,
                  nic.IPXVirtualNetNumber,
                  nic.KeepAliveInterval,
                  nic.KeepAliveTime,
                  nic.MacAddress,
                  nic.MTU,
                  nic.NumForwardPackets,
                  nic.PMTUBHDetectEnabled,
                  nic.PMTUDiscoveryEnabled,
                  nic.ServiceName,
                  nic.SettingID,
                  tcpip_netbios_options,
                  nic.TcpMaxConnectRetransmissions,
                  nic.TcpMaxDataRetransmissions,
                  nic.TcpNumConnections,
                  nic.TcpUseRFC1122UrgentPointer,
                  nic.TcpWindowSize,
                  nic.WINSEnableLMHostsLookup,
                  nic.WINSHostLookupFile,
                  nic.WINSPrimaryServer,
                  nic.WINSScopeID,
                  nic.WINSSecondaryServer
               )

               if block_given?
                  yield struct
               else
                  array << struct
               end
            }
         end

         block_given? ? nil : array
      end
      
      # Returns the hostname of the current host.  This may or not return
      # the FQDN, depending on your system.
      # 
      def self.hostname
         host = nil
         begin
            wmi = WIN32OLE.connect("winmgmts://")
         rescue WIN32OLERuntimeError => e
            raise Error, e
         else
            query = "
               select * from Win32_NetworkAdapterConfiguration
               where IPEnabled = True
            "
            wmi.ExecQuery(query).each{ |nic|
               host = nic.DNSHostName
               break
            }
         end
         host
      end
      
      # Returns a list of unique IP addresses for the current host.
      #
      def self.ip_addr
         ip_addrs = []
         begin
            wmi = WIN32OLE.connect("winmgmts://")
         rescue WIN32OLERuntimeError => e
            raise Error, e
         else
            query = "
               select * from Win32_NetworkAdapterConfiguration
               where IPEnabled = True
            "
            wmi.ExecQuery(query).each{ |nic| ip_addrs << nic.IPAddress }
         end
         ip_addrs.flatten.uniq
      end      

      # Returns the host id (mac address) of the current host.
      #
      def self.host_id
         info[0].mac_address
      end
   end
end
