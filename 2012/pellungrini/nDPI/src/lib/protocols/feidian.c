/*
 * feidian.c
 * Copyright (C) 2009-2011 by ipoque GmbH
 * 
 * This file is part of OpenDPI, an open source deep packet inspection
 * library based on the PACE technology by ipoque GmbH
 * 
 * OpenDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenDPI.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#include "ndpi_protocols.h"

#ifdef NDPI_PROTOCOL_FEIDIAN

static void ndpi_int_feidian_add_connection(struct ndpi_detection_module_struct *ndpi_struct, 
					    struct ndpi_flow_struct *flow, ndpi_protocol_type_t protocol_type)
{
  ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_FEIDIAN, protocol_type);
}


void ndpi_search_feidian(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow)
{
  struct ndpi_packet_struct *packet = &flow->packet;
	

  //      struct ndpi_id_struct         *src=ndpi_struct->src;
  //      struct ndpi_id_struct         *dst=ndpi_struct->dst;


  if (packet->tcp != NULL) {
    if (packet->tcp->dest == htons(8080) && packet->payload_packet_len == 4
	&& packet->payload[0] == 0x29 && packet->payload[1] == 0x1c
	&& packet->payload[2] == 0x32 && packet->payload[3] == 0x01) {
      NDPI_LOG(NDPI_PROTOCOL_FEIDIAN, ndpi_struct, NDPI_LOG_DEBUG,
	       "Feidian: found the flow (TCP): packet_size: %u; Flowstage: %u\n",
	       packet->payload_packet_len, flow->l4.udp.feidian_stage);
      ndpi_int_feidian_add_connection(ndpi_struct, flow, NDPI_REAL_PROTOCOL);
      return;
    } else if (packet->payload_packet_len > 50 && memcmp(packet->payload, "GET /", 5) == 0) {
      ndpi_parse_packet_line_info(ndpi_struct, flow);
      if (packet->host_line.ptr != NULL && packet->host_line.len == 18
	  && memcmp(packet->host_line.ptr, "config.feidian.com", 18) == 0) {
	ndpi_int_feidian_add_connection(ndpi_struct, flow, NDPI_CORRELATED_PROTOCOL);
	return;
      }
    }
    NDPI_LOG(NDPI_PROTOCOL_FEIDIAN, ndpi_struct, NDPI_LOG_DEBUG,
	     "Feidian: discarted the flow (TCP): packet_size: %u; Flowstage: %u\n",
	     packet->payload_packet_len, flow->l4.udp.feidian_stage);
  } else if (packet->udp != NULL) {
    if (ntohs(packet->udp->source) == 53124 || ntohs(packet->udp->dest) == 53124) {
      if (flow->l4.udp.feidian_stage == 0 && (packet->payload_packet_len == 112)
	  && packet->payload[0] == 0x1c && packet->payload[1] == 0x1c
	  && packet->payload[2] == 0x32 && packet->payload[3] == 0x01) {
	flow->l4.udp.feidian_stage = 1;
	return;
      } else if (flow->l4.udp.feidian_stage == 1
		 && (packet->payload_packet_len == 116 || packet->payload_packet_len == 112)
		 && packet->payload[0] == 0x1c
		 && packet->payload[1] == 0x1c && packet->payload[2] == 0x32 && packet->payload[3] == 0x01) {
	ndpi_int_feidian_add_connection(ndpi_struct, flow, NDPI_REAL_PROTOCOL);
	return;
      }
    }
    NDPI_LOG(NDPI_PROTOCOL_FEIDIAN, ndpi_struct, NDPI_LOG_DEBUG,
	     "Feidian: discarted the flow (UDP): packet_size: %u; Flowstage: %u\n",
	     packet->payload_packet_len, flow->l4.udp.feidian_stage);
  }
  NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_FEIDIAN);
}
#endif
