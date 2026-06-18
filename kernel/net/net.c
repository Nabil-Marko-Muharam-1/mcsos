#include "mcsos/net/net.h"
#include "mcsos/fs/fd.h"

// Tabel Soket OS
static udp_socket_t sockets[MAX_SOCKETS];

static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

static uint16_t ntohs(uint16_t netshort) {
    return ((netshort & 0xFF) << 8) | ((netshort & 0xFF00) >> 8);
}

// Inisialisasi tabel soket menjadi kosong
void net_init() {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i].is_active = 0;
    }
    print_log("[NET] Subsistem jaringan dan tabel soket diinisialisasi.\n");
}

// Fungsi untuk User Space "membuka port" (Listen)
int udp_bind(uint16_t port, void (*callback)(uint8_t*, uint32_t)) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (!sockets[i].is_active) {
            sockets[i].is_active = 1;
            sockets[i].bound_port = port;
            sockets[i].rx_callback = callback;
            return i; // Return socket ID
        }
    }
    return -1; // Penuh
}

// Otak perutean (Routing)
void net_parse_packet(uint8_t *packet, uint32_t length) {
    if (length < sizeof(eth_hdr_t)) return;

    eth_hdr_t *eth = (eth_hdr_t *)packet;
    uint16_t ethertype = ntohs(eth->ethertype);

    if (ethertype == 0x0800) { // IPv4
        ipv4_hdr_t *ip = (ipv4_hdr_t *)(packet + sizeof(eth_hdr_t));
        
        if (ip->protocol == 17) { // UDP
            uint8_t ip_header_len = (ip->ihl_version & 0x0F) * 4;
            udp_hdr_t *udp = (udp_hdr_t *)((uint8_t *)ip + ip_header_len);
            
            // Ekstrak Port Tujuan (Jangan lupa konversi dari Big-Endian jaringan!)
            uint16_t dst_port = ntohs(udp->dst_port);
            
            uint8_t *payload = (uint8_t *)udp + sizeof(udp_hdr_t);
            // Payload length = total UDP length - UDP Header length (8 byte)
            uint32_t payload_len = ntohs(udp->length) - sizeof(udp_hdr_t);

            // Perutean Soket: Cek apakah ada yang mendengarkan Port ini?
            int routed = 0;
            for (int i = 0; i < MAX_SOCKETS; i++) {
                if (sockets[i].is_active && sockets[i].bound_port == dst_port) {
                    // Ada yang mendengarkan! Kirim pesan ke aplikasi tersebut!
                    if (sockets[i].rx_callback) {
                        sockets[i].rx_callback(payload, payload_len);
                        routed = 1;
                    }
                }
            }

            if (!routed) {
                print_log("[NET] Paket UDP masuk ke Port yang ditutup/tidak ada soket.\n");
            }
        }
    }
}
