#ifndef NET_H
#define NET_H

#include <stdint.h>

typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
} __attribute__((packed)) eth_hdr_t;

typedef struct {
    uint8_t ihl_version;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t frag_offset;
    uint8_t ttl;
    uint8_t protocol; 
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed)) ipv4_hdr_t;

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_hdr_t;

// --- Fitur Baru: Soket UDP ---
#define MAX_SOCKETS 8

typedef struct {
    int is_active;
    uint16_t bound_port; // Port yang didengarkan soket ini
    // Di OS asli, ini berisi queue/buffer pesan masuk.
    // Untuk M11, kita panggil callback function untuk kesederhanaan.
    void (*rx_callback)(uint8_t *data, uint32_t len); 
} udp_socket_t;

void net_init();
int udp_bind(uint16_t port, void (*callback)(uint8_t*, uint32_t));
void net_parse_packet(uint8_t *packet, uint32_t length);

#endif
